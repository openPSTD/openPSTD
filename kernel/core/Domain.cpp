//////////////////////////////////////////////////////////////////////////
// This file is part of openPSTD.                                       //
//                                                                      //
// openPSTD is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// openPSTD is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Date: 17-9-2015
//
// Authors:
//      Louis van Harten, Omar Richardson
//
//////////////////////////////////////////////////////////////////////////

#include "Domain.h"

using namespace std;
using namespace Eigen;
namespace OpenPSTD {
    namespace Kernel {

        Domain::Domain(shared_ptr<PSTDSettings> settings, int id, const float alpha,
                       Point top_left, Point size, const bool is_pml,
                       shared_ptr<WisdomCache> wnd, map<Direction, EdgeParameters> edge_param_map,
                       const shared_ptr<Domain> pml_for_domain = shared_ptr<Domain>(nullptr)) {

            this->initialize_domain(settings, id, alpha, top_left, size, is_pml, wnd, edge_param_map, pml_for_domain);
        }

        Domain::Domain(shared_ptr<PSTDSettings> settings, int id, const float alpha,
                       vector<float> top_left_vector, vector<float> size_vector, const bool is_pml,
                       shared_ptr<WisdomCache> wnd, map<Direction, EdgeParameters> edge_param_map,
                       const shared_ptr<Domain> pml_for_domain = shared_ptr<Domain>(nullptr)) {
            Point top_left((int) top_left_vector.at(0), (int) top_left_vector.at(1));
            Point size((int) size_vector.at(0), (int) size_vector.at(1));
            this->initialize_domain(settings, id, alpha, top_left, size, is_pml, wnd, edge_param_map, pml_for_domain);
        }

        void Domain::initialize_domain(shared_ptr<PSTDSettings> settings, int id, const float alpha,
                                       Point top_left, Point size, const bool is_pml,
                                       shared_ptr<WisdomCache> wnd,
                                       map<Direction, EdgeParameters> edge_param_map,
                                       const shared_ptr<Domain> pml_for_domain) {
            this->settings = settings;
            this->top_left = top_left;
            this->size = size; // Remember PML domains have a fixed size.
            this->bottom_right = top_left + size;
            this->wnd = wnd;
            this->id = id;
            this->edge_param_map = edge_param_map;
            this->alpha = alpha; // Todo: Usually 1, and barely used. Push to settings when PML domain becomes subclass
            //Todo: (TK): Probably wrong, especially with two neighbouring PML domains
            this->impedance = -((sqrt(1 - alpha) + 1) / (sqrt(1 - alpha) - 1));
            if (is_pml) { // Ugly... Fix when possible
                this->pml_for_domain_list.push_back(pml_for_domain);
            }
            if (this->is_rigid()) {
                this->rho = 1E200;
            }
            else {
                this->rho = this->settings->GetDensityOfAir();
            }
            this->is_pml = is_pml;
            this->is_secondary_pml = false;
            for (auto domain:this->pml_for_domain_list) {
                if (domain->is_pml) {
                    is_secondary_pml = true;
                    // TK: Assert there is only one primary pml domain.
                }
            }
            this->current_values = {};
            this->previous_values = {};
            this->l_values = {};
            this->pml_arrays = {};
            this->clear_fields();
            this->clear_matrices();
            this->clear_pml_arrays();
            this->local = false;
        }

        // version of calc that would have a return value.
        ArrayXXf Domain::calc(CalcDirection cd, CalculationType ct, ArrayXcf dest) {
            vector<shared_ptr<Domain>> domains1, domains2;
            if (cd == CalcDirection::X) {
                domains1 = left;
                domains2 = right;
            }
            else {
                domains1 = bottom;
                domains2 = top;
            }

            vector<int> own_range = get_range(cd);

            ArrayXXf source;

            if (dest.cols() != 0) {
                if (cd == CalcDirection::X) {
                    source = extended_zeros(0, 1);
                }
                else {
                    source = extended_zeros(1, 0);
                }
            }
            else {
                if (ct == CalculationType::PRESSURE) {
                    source = this->current_values.p0;
                }
                else {
                    if (cd == CalcDirection::X) {
                        source = this->current_values.vx0;
                    }
                    else {
                        source = this->current_values.vy0;
                    }
                }
            }

            // loop over all possible combinations of neighbours for this domain (including null on one side)
            shared_ptr<Domain> d1, d2;
            for (int i = 0; i != domains1.size() + 1; i++) { //the +1 because a null pointer is also needed
                d1 = (i != domains1.size()) ? domains1[i] : nullptr;
                for (int j = 0; j != domains2.size() + 1; j++) {
                    d2 = (j != domains2.size()) ? domains2[j] : nullptr;
                    vector<int> rho_matrix_key;
                    if (d1!=nullptr) rho_matrix_key.push_back(d1->id);
                    if (d2!=nullptr) rho_matrix_key.push_back(d2->id);

                    //The range is determined and clipped to the neighbour domain ranges
                    vector<int> range_intersection = own_range;

                    if (d1 != nullptr) {
                        vector<int> range1 = d1->get_range(cd);
                        set_intersection(range1.begin(), range1.end(),
                                         range_intersection.begin(), range_intersection.end(),
                                         back_inserter(range_intersection));
                    }
                    if (d2 != nullptr) {
                        vector<int> range2 = d2->get_range(cd);
                        set_intersection(range2.begin(), range2.end(),
                                         range_intersection.begin(), range_intersection.end(),
                                         back_inserter(range_intersection));
                    }

                    // If there is nothing left after clipping to domains, continue with a different set of domains
                    if (range_intersection.size() == 0) {
                        continue;
                    }

                    // Set up various parameters and intermediates that are needed for the spatial derivatives
                    int range_start = *min_element(range_intersection.begin(), range_intersection.end());
                    int range_end = *max_element(range_intersection.begin(), range_intersection.end()) + 1;
                    int primary_dimension = (cd == CalcDirection::X) ? size.x : size.y;
                    int N_total = 2 * settings->GetWindowSize() + primary_dimension;
                    int wlen = settings->GetWindowSize();
                    ArrayXf wind = get_window_coefficients(wlen, settings->GetPatchError());

                    if (ct == CalculationType::PRESSURE) {
                        N_total++;
                    }
                    else {
                        primary_dimension++;
                    }

                    ArrayXXf matrix_main, matrix_side1, matrix_side2;
                    if (ct == CalculationType::VELOCITY && d1 == nullptr && d2 == nullptr) {
                        // For a PML layer parallel to its interface direction the matrix is concatenated with zeros
                        // TODO Louis: Why only zeroes for ct==velocity? Should zeroes also be added in the else{} block?
                        // a PML domain can also have a neighbour, see:
                        //   |             |
                        // __|_____________|___
                        //   |     PML     |
                        //  <--------------->
                        d1 = d2 = shared_from_this();
                        if (cd == CalcDirection::X) {
                            matrix_side1 = extended_zeros(0, 1);
                            matrix_side2 = extended_zeros(0, 1);
                        }
                        else {
                            matrix_side1 = extended_zeros(1, 0);
                            matrix_side2 = extended_zeros(1, 0);
                        }
                    }
                    else {
                        if (d1 == nullptr) {
                            d1 = shared_from_this();
                        }
                        if (d2 == nullptr) {
                            d2 = shared_from_this();
                        }
                    }

                    if (ct == CalculationType::PRESSURE) {
                        matrix_main = current_values.p0;
                    }
                    else if (cd == CalcDirection::X) {
                        matrix_main = current_values.vx0;
                    }
                    else {
                        matrix_main = current_values.vy0;
                    }

                    // If the matrices are _not_ already filled with zeroes, choose which values to fill them with.
                    if (matrix_side1.cols() == 0) {
                        if (ct == CalculationType::PRESSURE) {
                            matrix_side1 = d1->current_values.p0;
                        }
                        else {
                            if (cd == CalcDirection::X) {
                                matrix_side1 = d1->current_values.vx0;
                            }
                            else {
                                matrix_side1 = d1->current_values.vy0;
                            }
                        }
                    }
                    if (matrix_side2.cols() == 0) {
                        if (ct == CalculationType::PRESSURE) {
                            matrix_side2 = d2->current_values.p0;
                        }
                        else {
                            if (cd == CalcDirection::X) {
                                matrix_side2 = d2->current_values.vx0;
                            }
                            else {
                                matrix_side2 = d2->current_values.vy0;
                            }
                        }
                    }

                    ArrayXcf derfact;
                    if (dest.cols() != 0) {
                        derfact = dest;
                    }
                    else {
                        if (ct == CalculationType::PRESSURE) {
                            derfact = wnd->get_discretization(settings->GetGridSpacing(),
                                                              N_total).pressure_deriv_factors;
                        }
                        else {
                            derfact = wnd->get_discretization(settings->GetGridSpacing(),
                                                              N_total).velocity_deriv_factors;
                        }
                    }

                    // TODO check if get_rho_array(...) is slow and if so, cache these values somewhere
                    float max_rho = 1E10;
                    RhoArray rho_array = get_rho_array(d1 != nullptr ? d1->rho : max_rho,
                                                       this->rho,
                                                       d2 != nullptr ? d2->rho : max_rho);

                    // Calculate the spatial derivatives for the current intersection range and store
                    int matrix_main_offset, matrix_side1_offset, matrix_side2_offset;
                    ArrayXXf matrix_main_indexed, matrix_side1_indexed, matrix_side2_indexed;
                    if (cd == CalcDirection::X) {
                        WisdomCache::Planset_FFTW planset = wnd->get_fftw_planset(
                                next_2_power(matrix_main.cols() + 2 * wlen), matrix_main.rows());
                        matrix_main_offset = this->top_left.y;
                        matrix_side1_offset = d1->top_left.y;
                        matrix_side2_offset = d2->top_left.y;

                        int ncols = range_end - range_start;

                        matrix_main_indexed = matrix_main.block(0, range_start - matrix_main_offset,
                                                                matrix_main.rows(), ncols);
                        matrix_side1_indexed = matrix_side1.block(0, range_start - matrix_side1_offset,
                                                                  matrix_side1.rows(), ncols);
                        matrix_side2_indexed = matrix_side2.block(0, range_start - matrix_side2_offset,
                                                                  matrix_side2.rows(), ncols);

                        Eigen:ArrayXXf spatresult = spatderp3(matrix_side1, matrix_main, matrix_side2, derfact,
                                                              rho_array, wind, wlen, ct, cd, planset.plan, planset.plan_inv);
                        source.block(0, range_start - matrix_main_offset, matrix_main.rows(), ncols+1) = spatresult;
                    }
                    else {
                        WisdomCache::Planset_FFTW planset = wnd->get_fftw_planset(
                                next_2_power(matrix_main.rows() + 2 * wlen), matrix_main.cols());
                        matrix_main_offset = this->top_left.x;
                        matrix_side1_offset = d1->top_left.x;
                        matrix_side2_offset = d2->top_left.x;

                        int nrows = range_end - range_start;

                        matrix_main_indexed = matrix_main.block(range_start - matrix_main_offset, 0,
                                                                nrows, matrix_main.cols());
                        matrix_side1_indexed = matrix_side1.block(range_start - matrix_side1_offset, 0,
                                                                  nrows, matrix_side1.cols());
                        matrix_side2_indexed = matrix_side2.block(range_start - matrix_side2_offset, 0,
                                                                  nrows, matrix_side2.cols());

                        ArrayXXf spatresult = spatderp3(matrix_side1, matrix_main, matrix_side2, derfact,
                                                              rho_array, wind, wlen, ct, cd, planset.plan, planset.plan_inv);
                        source.block(range_start - matrix_main_offset, 0, nrows, matrix_main.cols()) = spatresult;
                    }
                }
            }

            if (dest.cols() == 0) {
                if (ct == CalculationType::PRESSURE) {
                    this->current_values.p0 = source;
                }
                else {
                    if (cd == CalcDirection::X) {
                        this->current_values.vx0 = source;
                    }
                    else {
                        this->current_values.vy0 = source;
                    }
                }
            }
            return source;
        }

        /**
         * Near-alias to calc(CalcDirection cd, CalculationType ct, vector<float> dest), but with
         * a default empty vector as dest.
         */
        void Domain::calc(CalcDirection cd, CalculationType ct) {
            ArrayXcf nulldest;
            Domain::calc(cd, ct, nulldest);
        }

        bool Domain::contains_point(Point point) {
            vector<float> location = {(float) point.x, (float) point.y, (float) point.z};
            return contains_location(location);
        }

        bool Domain::contains_location(vector<float> location) {
            for (unsigned long dim = 0; dim < location.size(); dim++) {
                if (top_left.array.at(dim) > location.at(dim) or
                    location.at(dim) > bottom_right.array.at(dim)) {
                    return false;
                }
            }
            return true;
        }

        bool Domain::is_neighbour_of(shared_ptr<Domain> domain) {
            for (Direction direction :all_directions) {
                auto dir_nb = get_neighbours_at(direction);
                if (find(dir_nb.begin(), dir_nb.end(), domain) != dir_nb.end()) {
                    return true;
                }
            }
            return false;
        }

        bool Domain::is_pml_for(shared_ptr<Domain> domain) {
            return (find(pml_for_domain_list.begin(), pml_for_domain_list.end(), domain) != pml_for_domain_list.end());
        }

        bool Domain::is_rigid() {
            return impedance > 1000; //Why this exact value?
        }

        vector<int> Domain::get_range(CalcDirection cd) {
            int a_l, b_l;
            if (cd == CalcDirection::X) {
                a_l = top_left.x;
                b_l = bottom_right.x;
            }
            else {
                a_l = top_left.y;
                b_l = bottom_right.y;
            }
            vector<int> tmp((unsigned long) (b_l - a_l));
            iota(tmp.begin(), tmp.end(), a_l); // Increases with 1 from a_l
            return tmp;
        }

        vector<int> Domain::get_intersection_with(shared_ptr<Domain> other_domain, Direction direction) {
            vector<int> own_range;
            vector<int> other_range;
            switch (direction) {
                case Direction::LEFT:
                case Direction::RIGHT:
                    own_range = get_range(CalcDirection::X);
                    other_range = other_domain->get_range(CalcDirection::X);
                    break;
                case Direction::TOP:
                case Direction::BOTTOM:
                    own_range = get_range(CalcDirection::Y);
                    other_range = other_domain->get_range(CalcDirection::Y);
                    break;
            }
            vector<int> range_intersection = own_range;
            set_intersection(other_range.begin(), other_range.end(), range_intersection.begin(),
                             range_intersection.end(),
                             back_inserter(range_intersection));
            return range_intersection;
        }

        ArrayXXf Domain::extended_zeros(int x, int y, int z) {
            return ArrayXXf::Zero(size.x + x, size.y + y);
        }

        vector<shared_ptr<Domain>> Domain::get_neighbours_at(Direction direction) {
            switch (direction) {
                case Direction::LEFT:
                    return left;
                case Direction::RIGHT:
                    return right;
                case Direction::TOP:
                    return top;
                case Direction::BOTTOM:
                    return bottom;
            }
        }

        shared_ptr<Domain> Domain::get_neighbour_at(Direction direction, vector<float> location) {
            shared_ptr<Domain> correct_domain = nullptr;
            auto dir_neighbours = get_neighbours_at(direction);
            for (shared_ptr<Domain> domain:dir_neighbours) {
                if (domain->contains_location(location)) {
                    assert(correct_domain == nullptr);
                    correct_domain = domain;
                }
            }
            return correct_domain;
        }


        void Domain::compute_number_of_neighbours() {
            num_neighbour_domains = 0;
            num_pml_neighbour_domains = 0;
            for (Direction direction: all_directions) {
                for (auto domain: get_neighbours_at(direction)) {
                    num_neighbour_domains++;
                    if (domain->is_pml) {
                        num_pml_neighbour_domains++;
                    }
                }
            }
        }

        int Domain::number_of_neighbours(bool count_pml) {
            if (count_pml) {
                return num_neighbour_domains;
            }
            else {
                return num_neighbour_domains - num_pml_neighbour_domains;
            }
        }

        void Domain::add_neighbour_at(shared_ptr<Domain> domain, Direction direction) {
            switch (direction) {
                case Direction::LEFT:
                    left.push_back(domain);
                    break;
                case Direction::RIGHT:
                    right.push_back(domain);
                    break;
                case Direction::TOP:
                    top.push_back(domain);
                    break;
                case Direction::BOTTOM:
                    bottom.push_back(domain);
                    break;
            }
        }

        ArrayXXi Domain::get_vacant_range(Direction direction) {
            vector<shared_ptr<Domain>> neighbour_list;
            CalcDirection calc_dir = direction_to_calc_direction(direction);
            vector<int> range = get_range(calc_dir);
            set<int> range_set(range.begin(), range.end());
            switch (direction) {
                case Direction::LEFT:
                    neighbour_list = left;
                    break;
                case Direction::RIGHT:
                    neighbour_list = right;
                    break;
                case Direction::TOP:
                    neighbour_list = top;
                    break;
                case Direction::BOTTOM:
                    neighbour_list = bottom;
                    break;
            }
            for (shared_ptr<Domain> domain: neighbour_list) {
                vector<int> neighbour_range = domain->get_range(calc_dir);
                set<int> neighbour_range_set(neighbour_range.begin(), neighbour_range.end());
                set<int> set_diff;
                set_difference(range_set.begin(), range_set.end(), neighbour_range_set.begin(),
                               neighbour_range_set.end(), inserter(set_diff, set_diff.end()));
                range_set.swap(set_diff);
            }
            ArrayXXi vacant_range(range_set.size(), 2);
            range.assign(range_set.begin(), range_set.end());
            int iterator = 0;
            for (unsigned long i = 0; i < range.size(); i++) {
                if (i == 0 || range.at(i - 1) + 1 < range.at(i)) {
                    vacant_range(0, 0) = range.at(i);
                    vacant_range(0, 1) = range.at(i);
                    iterator++;
                }
                if (i == range.size() - 1 || range.at(i + 1) - 1 > range.at(i)) {
                    vacant_range(iterator - 1, 1) = range.at(i) + 1;
                }
            }
            return vacant_range.topRows(iterator);
        }


        void Domain::find_update_directions() {
            for (CalcDirection calc_dir: all_calc_directions) {
                bool should_update = false;
                if (number_of_neighbours(false) == 1 and is_pml) {
                    if (has_horizontal_attenuation and calc_dir == CalcDirection::X) {
                        // Todo: make sure we calculate in direction orthogonal to boundary
                        should_update = true;
                    }
                    else if (!has_horizontal_attenuation and calc_dir == CalcDirection::Y) {
                        should_update = true;
                    }
                    else if (local) {
                        should_update = false;
                    }
                    else {
                        for (Direction direction: all_directions) {
                            vector<shared_ptr<Domain>> dir_neighbours = get_neighbours_at(direction);
                            if (dir_neighbours.size() == 1 and !dir_neighbours.at(0)->is_pml) {
                                vector<shared_ptr<Domain>> opp_neighbours =
                                        dir_neighbours.at(0)->get_neighbours_at(get_opposite(direction));
                                assert(opp_neighbours.size() == 1 and id == opp_neighbours.at(0)->id);
                                should_update = !dir_neighbours.at(0)->edge_param_map[direction].locally_reacting;
                            }
                        }
                    }
                }
                this->should_update[calc_dir] = should_update;
            }
        }

        void Domain::clear_matrices() {
            l_values.Lpx = extended_zeros(0, 1);
            l_values.Lpy = extended_zeros(1, 0);
            l_values.Lvx = extended_zeros(0, 0);
            l_values.Lvy = extended_zeros(0, 0);
        }

        void Domain::clear_fields() {
            current_values.p0 = extended_zeros(0, 0);
            current_values.px0 = extended_zeros(0, 0);
            current_values.py0 = extended_zeros(0, 0);
            current_values.vx0 = extended_zeros(0, 1);
            current_values.vy0 = extended_zeros(1, 0);

            previous_values = {}; // Do we def need to empty this?
        }


        void Domain::clear_pml_arrays() {
            pml_arrays.px = extended_zeros(0, 0);
            pml_arrays.py = extended_zeros(0, 0);
            pml_arrays.vx = extended_zeros(0, 1);
            pml_arrays.vy = extended_zeros(1, 0);

        }

        void Domain::compute_pml_matrices() {
            //Todo (0mar): Refactor this method? It's asymmetric and spaghetty
            /*
             * TK: Only calculate PML matrices for PML domains with a single non-pml neighbour
             * or for secondary PML domains with a single PML neighbour.
             */
            int number_normal_neighbour_domains = num_neighbour_domains - num_pml_neighbour_domains;
            if (is_secondary_pml) {
                is_corner_domain = num_neighbour_domains == 2;
                /*
                 * TK: If this domain has its neighbour to the left or to the right,
                 * a PML matrix is obtained for horizontal attenuation.
                 */
                has_horizontal_attenuation = !left.empty() or !right.empty();
                if (is_corner_domain) {
                    /**
                     * TK: Corner PML domains should have a horizontal as well as a vertical component.
                     * In particular: not to neighbours in the same direction.
                     */
                    assert(has_horizontal_attenuation and (!top.empty() or !bottom.empty()));
                    needs_reversed_attenuation = vector<bool>(); // Init necessary?
                    needs_reversed_attenuation.push_back(!left.empty());
                    needs_reversed_attenuation.push_back(!bottom.empty());
                }
                else {
                    //TK: If this neighbour is located to the left or bottom, the attenuation is reversed.
                    needs_reversed_attenuation.push_back(!left.empty() or !bottom.empty());
                }
            }
            else {
                is_corner_domain = false;
                /*
                 * TK: If this domain has a neighbour to the left or right
                 * and this neighbour is not a PML domain,
                 * a PML matrix is obtained for horizontal (OR: Vertical?) attenuation.
                 */
                bool all_air_left = !left.empty() and get_num_pmls_in_direction(Direction::LEFT) == 0;
                bool all_air_right = !right.empty() and get_num_pmls_in_direction(Direction::RIGHT) == 0;
                bool all_air_bottom = !bottom.empty() and get_num_pmls_in_direction(Direction::BOTTOM) == 0;

                has_horizontal_attenuation = all_air_left or all_air_right;
                needs_reversed_attenuation.push_back(all_air_left or all_air_bottom);
                if (is_secondary_pml and is_corner_domain) {
                    // TK: PML is the product of horizontal and vertical attenuation.
                    create_attenuation_array(CalcDirection::X, needs_reversed_attenuation.at(0),
                                             pml_arrays.px, pml_arrays.vx);
                    create_attenuation_array(CalcDirection::Y, needs_reversed_attenuation.at(1),
                                             pml_arrays.py, pml_arrays.vx);
                }
                else {
                    CalcDirection calc_dir = CalcDirection::Y;
                    if (has_horizontal_attenuation) {
                        calc_dir = CalcDirection::X;
                    }
                    switch (calc_dir) {
                        case CalcDirection::X:
                            create_attenuation_array(calc_dir, needs_reversed_attenuation.at(0),
                                                     pml_arrays.px, pml_arrays.vx);
                            pml_arrays.py = ArrayXXf::Ones(size.y, size.x);//Change if unique
                            pml_arrays.vy = ArrayXXf::Ones(size.y+1, size.x); //TODO check if x/y is correct here
                            break;
                        case CalcDirection::Y:
                            create_attenuation_array(calc_dir, needs_reversed_attenuation.at(0),
                                                     pml_arrays.py, pml_arrays.vy);
                            pml_arrays.px = ArrayXXf::Ones(size.y, size.x);//Change if unique
                            pml_arrays.vx = ArrayXXf::Ones(size.y, size.x+1);//Change if unique
                            break;
                    }
                }
            }
        }

        void Domain::apply_pml_matrices() //Todo: Rename to pml_arrays
        {
            assert(number_of_neighbours(false) == 1 and is_pml or number_of_neighbours(true) <= 2 and
                   is_secondary_pml);
            // The pressure and velocity matrices are multiplied by the PML values.
            current_values.px0 *= pml_arrays.px;
            current_values.py0 *= pml_arrays.py;
            current_values.vx0 *= pml_arrays.vx;
            current_values.vy0 *= pml_arrays.vy;
        }


        void Domain::push_values() {
            previous_values = current_values;
        }


        int Domain::get_num_pmls_in_direction(Direction direction) {
            int num_pml_doms = 0;
            for (auto domain: get_neighbours_at(direction)) {
                if (domain->is_pml) {
                    num_pml_doms++;
                }
            }
            return num_pml_doms;
        }

        void Domain::create_attenuation_array(CalcDirection calc_dir, bool ascending, ArrayXXf &pml_pressure,
                                              ArrayXXf &pml_velocity) {
            /*
             * 0mar: Most of this method only needs to be computed once for all domains.
             * However, the computations are not that big and only executed in the initialization phase.
             */

            //Pressure defined in cell centers
            auto pressure_range =
                    VectorXf::LinSpaced(settings->GetPMLCells(), 0.5, float(settings->GetPMLCells() - 0.5)).array() /
                    settings->GetPMLCells();
            //Velocity defined in cell edges
            auto velocity_range =
                    VectorXf::LinSpaced(settings->GetPMLCells() + 1, 0, float(settings->GetPMLCells())).array() /
                    settings->GetPMLCells();
            ArrayXXf alpha_pml_pressure = settings->GetAttenuationOfPMLCells() * pressure_range.pow(4);
            ArrayXXf alpha_pml_velocity =
                    settings->GetDensityOfAir() * settings->GetAttenuationOfPMLCells() * velocity_range.pow(4);
            ArrayXXf pressure_pml_factors = (-alpha_pml_pressure * settings->GetTimeStep() /
                                             settings->GetDensityOfAir()).exp();
            ArrayXXf velocity_pml_factors = (-alpha_pml_velocity * settings->GetTimeStep()).exp();
            if (!ascending) {
                //Reverse if the attenuation takes place in the other direction
                pressure_pml_factors.reverseInPlace();
                velocity_pml_factors.reverseInPlace();
            }
            switch (calc_dir) {
                //Replicate matrices to size of domain
                case CalcDirection::X:
                    pml_pressure = pressure_pml_factors.transpose().replicate(size.x, 1);
                    pml_velocity = velocity_pml_factors.transpose().replicate(size.x, 1);
                    break;
                case CalcDirection::Y:
                    pml_pressure = pressure_pml_factors.replicate(1, size.y);
                    pml_velocity = velocity_pml_factors.replicate(1, size.y);
                    break;
            }
        }

        ostream &operator<<(ostream &str, Domain const &v) {
            str << "Domain " << v.id;
            if (v.is_pml) {
                str << " (pml)";
            }
            str << ", top left " << v.top_left << ", bottom right " << v.bottom_right;
            return str;
        }

        void Domain::post_initialization() {
            compute_number_of_neighbours();
            find_update_directions();
        }
    }
}
