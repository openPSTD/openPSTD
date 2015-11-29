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

namespace Kernel {

    Domain::Domain(std::shared_ptr<PSTDFileSettings> settings, std::string id, const float alpha,
                   std::shared_ptr<Point> top_left, std::shared_ptr<Point> size, const bool is_pml,
                   std::shared_ptr<WaveNumberDiscretizer> wnd,
                   const std::shared_ptr<Domain> pml_for_domain = std::shared_ptr<Domain>(nullptr)) {
        //Todo (Louis): Implementeer dit verder
        this->settings = settings;
        this->top_left = top_left;
        this->size = size;
        this->bottom_right = std::make_shared<Point>(*top_left + *size);
        this->wnd = wnd;
        this->id = id;
        this->impedance = -((std::sqrt(1 - alpha) + 1) / (std::sqrt(1 - alpha) - 1));
        if (is_pml) {
            this->pml_for_domain_list.push_back(pml_for_domain);
        }
        this->find_update_directions();
        this->compute_number_of_neighbours();
    };

    // version of calc that would have a return value.
    Eigen::ArrayXXf Domain::calc(CalcDirection bt, CalculationType ct, std::shared_ptr<Eigen::ArrayXcf> dest) {
        std::vector<std::shared_ptr<Domain>> domains1, domains2;
        if (bt == CalcDirection::X) {
            domains1 = this->left;
            domains2 = this->right;
        } else {
            domains1 = this->bottom;
            domains2 = this->top;
        }

        std::vector<int> own_range = get_range(bt);

        std::shared_ptr<Eigen::ArrayXXf> source;

        if (dest != nullptr || ct == CalculationType::PRESSURE) {
            if (bt == CalcDirection::X) {
                source = extended_zeros(0, 1);
            } else {
                source = extended_zeros(1, 0);
            }
        } else {
            source = extended_zeros(0, 0);
        }

        // loop over all possible combinations of neighbours for this domain (including null on one side)
        std::shared_ptr<Domain> d1, d2;
        for (int i = 0; i != domains1.size() + 1; i++) {
            d1 = (i != domains1.size()) ? domains1[i] : nullptr;
            for (int j = 0; i != domains2.size() + 1; j++) {
                d2 = (i != domains2.size()) ? domains2[i] : nullptr;
                std::vector<std::string> rho_matrix_key;
                rho_matrix_key.push_back(d1->id);
                rho_matrix_key.push_back(d2->id);

                //The range is determined and clipped to the neighbour domain ranges
                std::vector<int> range_intersection = own_range;

                if (d1 != nullptr) {
                    std::vector<int> range1 = d1->get_range(bt);
                    set_intersection(range1.begin(), range1.end(),
                                     range_intersection.begin(), range_intersection.end(),
                                     back_inserter(range_intersection));
                }
                if (d2 != nullptr) {
                    std::vector<int> range2 = d1->get_range(bt);
                    set_intersection(range2.begin(), range2.end(),
                                     range_intersection.begin(), range_intersection.end(),
                                     back_inserter(range_intersection));
                }

                // If there is nothing left after clipping to domains, continue with a different set of domains
                if (range_intersection.size() == 0) {
                    continue;
                }

                // Set up various parameters and intermediates that are needed for the spatial derivatives
                int range_start = *std::min_element(range_intersection.begin(), range_intersection.end());
                int range_end = *std::max_element(range_intersection.begin(), range_intersection.end()) + 1;
                int primary_dimension = (bt == CalcDirection::X) ? this->size->x : this->size->y;
                int N_total = 2 * this->settings->GetWindowSize() + primary_dimension;

                if (ct == CalculationType::PRESSURE) {
                    N_total++;
                } else {
                    primary_dimension++;
                }

                std::shared_ptr<Eigen::ArrayXXf> matrix0 = nullptr, matrix1 = nullptr, matrix2 = nullptr;
                if (ct == CalculationType::VELOCITY && d1 == nullptr && d2 == nullptr) {
                    // For a PML layer parallel to its interface direction the matrix is concatenated with zeros
                    // TODO Louis: Why only zeroes for ct==velocity? Should zeroes also be added in the else{} block?
                    // a PML domain can also have a neighbour, see:
                    //   |             |
                    // __|_____________|___
                    //   |     PML     |
                    //  <--------------->
                    d1 = d2 = shared_from_this();
                    if (bt == CalcDirection::X) {
                        matrix1 = extended_zeros(0, 1);
                        matrix2 = extended_zeros(0, 1);
                    } else {
                        matrix1 = extended_zeros(1, 0);
                        matrix2 = extended_zeros(1, 0);
                    }
                } else {
                    if (d1 == nullptr) {
                        d1 = shared_from_this();
                    }
                    if (d2 == nullptr) {
                        d2 = shared_from_this();
                    }
                }

                if (ct == CalculationType::PRESSURE) {
                    matrix0 = std::make_shared<Eigen::ArrayXXf>(this->current_values.p0);
                } else if (bt == CalcDirection::X) {
                    matrix0 = std::make_shared<Eigen::ArrayXXf>(this->current_values.u0);
                } else {
                    matrix0 = std::make_shared<Eigen::ArrayXXf>(this->current_values.w0);
                }

                // If the matrices are _not_ already filled with zeroes, choose which values to fill them with.
                if (matrix1 == nullptr) {
                    if (ct == CalculationType::PRESSURE) {
                        matrix1 = std::make_shared<Eigen::ArrayXXf>(d1->current_values.p0);
                    } else {
                        if (bt == CalcDirection::X) {
                            matrix1 = std::make_shared<Eigen::ArrayXXf>(d1->current_values.u0);
                        } else {
                            matrix1 = std::make_shared<Eigen::ArrayXXf>(d1->current_values.w0);
                        }
                    }
                }
                if (matrix2 == nullptr) {
                    if (ct == CalculationType::PRESSURE) {
                        matrix2 = std::make_shared<Eigen::ArrayXXf>(d2->current_values.p0);
                    } else {
                        if (bt == CalcDirection::X) {
                            matrix2 = std::make_shared<Eigen::ArrayXXf>(d2->current_values.u0);
                        } else {
                            matrix2 = std::make_shared<Eigen::ArrayXXf>(d2->current_values.w0);
                        }
                    }
                }

                // Set up parameters for the spatial derivative later
                int var_index = 0, direction = 0;
                if (ct == CalculationType::VELOCITY) {
                    var_index = 1;
                }
                if (bt == CalcDirection::X) {
                    direction = 1;
                }

                std::shared_ptr<Eigen::ArrayXcf> derfact;
                if (dest != nullptr) {
                    derfact = dest;
                } else {
                    if (ct == CalculationType::PRESSURE) {
                        derfact = wnd->get_discretization(this->settings->GetGridSpacing(),
                                                          N_total).pressure_deriv_factors;
                    } else {
                        derfact = wnd->get_discretization(this->settings->GetGridSpacing(),
                                                          N_total).velocity_deriv_factors;
                    }
                }

                // Determine which rho matrix instance to use
                std::string rho_array_id;
                if (d1 != nullptr) {
                    if (d2 != nullptr) {
                        rho_array_id = d1->id + this->id + d2->id;
                    } else {
                        rho_array_id = d1->id + this->id;
                    }
                } else {
                    if (d2 != nullptr) {
                        rho_array_id = this->id + d2->id;
                    } else {
                        rho_array_id = this->id;
                    }
                }
                Eigen::Array<float, 4, 2> rho_array;
                if (ct == CalculationType::PRESSURE) {
                    rho_array = this->rho_arrays[rho_array_id].pressure;
                } else {
                    rho_array = this->rho_arrays[rho_array_id].velocity;
                }

                //TODO: set matrix to result of spatderp3
            }
        }
    }

    /**
     * Near-alias to calc(BoundaryType bt, CalculationType ct, std::vector<float> dest), but with
     * a default empty vector as dest.
     */
    void Domain::calc(CalcDirection bt, CalculationType ct) {
        std::shared_ptr<Eigen::ArrayXcf> dest;
        Domain::calc(bt, ct, dest);
    }

    bool Domain::contains_point(Point point) {
        std::vector<float> location = {(float) point.x, (float) point.y, (float) point.z};
        return this->contains_location(location);
    }

    bool Domain::contains_location(std::vector<float> location) {
        for (unsigned long dim = 0; dim < 3; dim++) {
            if (this->top_left->array.at(dim) > location.at(dim) or
                location.at(dim) > this->bottom_right->array.at(dim)) {
                return false;
            }
        }
        return true;
    }

    bool Domain::is_neighbour_of(std::shared_ptr<Domain> domain) {
        for (Direction direction :all_directions) {
            auto dir_nb = get_neighbours_at(direction);
            if (std::find(dir_nb.begin(), dir_nb.end(), domain) != dir_nb.end()) {
                return true;
            }
        }
        return false;
    }

    bool Domain::is_pml_for(std::shared_ptr<Domain> domain) {
        return (std::find(this->pml_for_domain_list.begin(), this->pml_for_domain_list.end(), domain) !=
                this->pml_for_domain_list.end());
    }

    bool Domain::is_rigid() {
        return this->impedance > 1000; //Little random?
    }

    std::vector<int> Domain::get_range(CalcDirection bt) {
        int a_l, b_l;
        if (bt == CalcDirection::X) {
            a_l = this->top_left->x;
            b_l = this->bottom_right->x;
        } else {
            a_l = this->top_left->y;
            b_l = this->bottom_right->y;
        }
        std::vector<int> tmp((unsigned long) (b_l - a_l));
        std::iota(tmp.begin(), tmp.end(), a_l); // Increases with 1 from a_l
        return tmp;
    }

    std::vector<int> Domain::get_intersection_with(std::shared_ptr<Domain> other_domain, Direction direction) {
        std::vector<int> own_range;
        std::vector<int> other_range;
        switch (direction) {
            case Direction::LEFT:
            case Direction::RIGHT:
                own_range = this->get_range(CalcDirection::X);
                other_range = other_domain->get_range(CalcDirection::X);
                break;
            case Direction::TOP:
            case Direction::BOTTOM:
                own_range = this->get_range(CalcDirection::Y);
                other_range = other_domain->get_range(CalcDirection::Y);
                break;
        }
        std::vector<int> range_intersection = own_range;
        set_intersection(other_range.begin(), other_range.end(), range_intersection.begin(), range_intersection.end(),
                         back_inserter(range_intersection));
        return range_intersection;
    }

    std::shared_ptr<Eigen::ArrayXXf> Domain::extended_zeros(int x, int y, int z) {
        std::shared_ptr<Eigen::ArrayXXf> tmp(new Eigen::ArrayXXf(this->size->x + x, this->size->y + y));
        tmp->setZero();
        return tmp;
    }

    std::vector<std::shared_ptr<Domain>> Domain::get_neighbours_at(Direction direction) {
        switch (direction) {
            case Direction::LEFT:
                return this->left;
            case Direction::RIGHT:
                return this->right;
            case Direction::TOP:
                return this->top;
            case Direction::BOTTOM:
                return this->bottom;
        }
    }

    std::shared_ptr<Domain> Domain::get_neighbour_at(Direction direction, std::vector<float> location) {
        std::shared_ptr<Domain> correct_domain = nullptr;
        auto dir_neighbours = this->get_neighbours_at(direction);
        for (std::shared_ptr<Domain> domain:dir_neighbours) {
            if (domain->contains_location(location)) {
                assert(correct_domain == nullptr);
                correct_domain = domain;
            }
        }
        return correct_domain;
    }


    void Domain::compute_number_of_neighbours() {
        this->number_of_domains = 0;
        this->number_of_pml_domains = 0;
        for (Direction direction: all_directions) {
            for (auto domain: this->get_neighbours_at(direction)) {
                this->number_of_domains++;
                if (domain->is_pml) {
                    this->number_of_pml_domains++;
                }
            }
        }
    }

    int Domain::number_of_neighbours(bool count_pml) {
        if (count_pml) {
            return number_of_domains;
        } else {
            return number_of_domains - number_of_pml_domains;
        }
    }

    void Domain::add_neighbour_at(std::shared_ptr<Domain> domain, Direction direction) {
        switch (direction) {
            case Direction::LEFT:
                this->left.push_back(domain);
                break;
            case Direction::RIGHT:
                this->right.push_back(domain);
                break;
            case Direction::TOP:
                this->top.push_back(domain);
                break;
            case Direction::BOTTOM:
                this->bottom.push_back(domain);
                break;
        }
    }

    void Domain::compute_rho_arrays() {
        //struct containing functions calculating the index strings to keep the for loop below somewhat readable.
        struct index_strings {
            std::string id(std::shared_ptr<Domain> d1, Domain *dm, std::shared_ptr<Domain> d2) {
                return d1->id + dm->id + d2->id;
            }

            std::string id(std::shared_ptr<Domain> d1, Domain *dm) {
                return d1->id + dm->id;
            }

            std::string id(Domain *dm, std::shared_ptr<Domain> d2) {
                return dm->id + d2->id;
            }

            std::string id(Domain *dm) {
                return dm->id;
            }
        };
        index_strings x;
        std::vector<std::shared_ptr<Domain>> left_domains = this->left;
        std::vector<std::shared_ptr<Domain>> right_domains = this->right;
        std::vector<std::shared_ptr<Domain>> top_domains = this->top;
        std::vector<std::shared_ptr<Domain>> bottom_domains = this->bottom;

        float max_float = std::numeric_limits<float>::max();

        // Checks if sets of adjacent domains are non-zero and calculates the rho_arrays accordingly
        // TODO (optional) refactor: there is probably a prettier solution than if/else'ing this much
        if (left_domains.size()) {
            for (std::shared_ptr<Domain> d1 : left_domains) {
                if (right_domains.size()) {
                    for (std::shared_ptr<Domain> d2 : right_domains) {
                        std::vector<float> rhos = {d1->rho, d2->rho};
                        this->rho_arrays[x.id(d1, this, d2)] = get_rho_array(rhos[0], this->rho, rhos[1]);
                    }
                } else {
                    std::vector<float> rhos = {d1->rho, max_float};
                    this->rho_arrays[x.id(d1, this)] = get_rho_array(rhos[0], this->rho, rhos[1]);
                }
            }
        } else {
            if (right_domains.size()) {
                for (std::shared_ptr<Domain> d2 : right_domains) {
                    std::vector<float> rhos = {max_float, d2->rho};
                    this->rho_arrays[x.id(this, d2)] = get_rho_array(rhos[0], this->rho, rhos[1]);
                }
            } else {
                std::vector<float> rhos = {max_float, max_float};
                this->rho_arrays[x.id(this)] = get_rho_array(rhos[0], this->rho, rhos[1]);
            }
        }

        if (bottom_domains.size()) {
            for (std::shared_ptr<Domain> d1 : bottom_domains) {
                if (top_domains.size()) {
                    for (std::shared_ptr<Domain> d2 : top_domains) {
                        std::vector<float> rhos = {d1->rho, d2->rho};
                        this->rho_arrays[x.id(d1, this, d2)] = get_rho_array(rhos[0], this->rho, rhos[1]);
                    }
                } else {
                    std::vector<float> rhos = {d1->rho, max_float};
                    this->rho_arrays[x.id(d1, this)] = get_rho_array(rhos[0], this->rho, rhos[1]);
                }
            }
        } else {
            if (top_domains.size()) {
                for (std::shared_ptr<Domain> d2 : top_domains) {
                    std::vector<float> rhos = {max_float, d2->rho};
                    this->rho_arrays[x.id(this, d2)] = get_rho_array(rhos[0], this->rho, rhos[1]);
                }
            } else {
                std::vector<float> rhos = {max_float, max_float};
                this->rho_arrays[x.id(this)] = get_rho_array(rhos[0], this->rho, rhos[1]);
            }
        }
    }

    Eigen::ArrayXXi Domain::get_vacant_range(Direction direction) {
        std::vector<std::shared_ptr<Domain>> neighbour_list;
        CalcDirection calc_dir = direction_to_calc_direction(direction);
        std::vector<int> range = this->get_range(calc_dir);
        std::set<int> range_set(range.begin(), range.end());
        switch (direction) {
            case Direction::LEFT:
                neighbour_list = this->left;
                break;
            case Direction::RIGHT:
                neighbour_list = this->right;
                break;
            case Direction::TOP:
                neighbour_list = this->top;
                break;
            case Direction::BOTTOM:
                neighbour_list = this->bottom;
                break;
        }
        for (std::shared_ptr<Domain> domain: neighbour_list) {
            std::vector<int> neighbour_range = domain->get_range(calc_dir);
            std::set<int> neighbour_range_set(neighbour_range.begin(), neighbour_range.end());
            std::set<int> set_diff;
            std::set_difference(range_set.begin(), range_set.end(), neighbour_range_set.begin(),
                                neighbour_range_set.end(), std::inserter(set_diff, set_diff.end()));
            range_set.swap(set_diff);
        }
        Eigen::ArrayXXi vacant_range(range_set.size(), 2);
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
        std::vector<CalcDirection> calc_directions = {CalcDirection::X, CalcDirection::Y};
        std::vector<Direction> directions = {Direction::LEFT, Direction::RIGHT, Direction::TOP, Direction::BOTTOM};
        for (CalcDirection calc_dir: calc_directions) {
            bool should_update;
            if (this->number_of_neighbours(false) == 1 and this->is_pml) {
                if (this->is_horizontal and calc_dir == CalcDirection::X) {
                    // Todo: make sure we calculate in direction orthogonal to boundary
                    should_update = true;
                } else if (!this->is_horizontal and calc_dir == CalcDirection::Y) {
                    should_update = true;
                } else if (this->local) {
                    should_update = false;
                } else {
                    for (Direction direction: directions) {
                        std::vector<std::shared_ptr<Domain>> dir_neighbours = this->get_neighbours_at(direction);
                        if (dir_neighbours.size() == 1 and !dir_neighbours.at(0)->is_pml) {
                            std::vector<std::shared_ptr<Domain>> opp_neighbours =
                                    dir_neighbours.at(0)->get_neighbours_at(get_opposite(direction));
                            assert(opp_neighbours.size() == 1 and this->id == opp_neighbours.at(0)->id);
                            should_update = !dir_neighbours.at(0)->edge_param_map[direction].locally_reacting;
                        }
                    }
                }
            }
            this->should_update[calc_dir] = should_update; // Should have been initialized
        }
    }

    void Domain::clear_matrices() {
        this->l_values.Lpx = extended_zeros(0, 1);
        this->l_values.Lpy = extended_zeros(1, 0);
        this->l_values.Lvx = extended_zeros(0, 0);
        this->l_values.Lvy = extended_zeros(0, 0);
    }

    void Domain::push_values() {
        //Todo: can be optimized, prev values and current values can turn into pointers.
        this->previous_values = this->current_values;
    }

    void Domain::apply_pml_matrices() {

    }

    void Domain::compute_pml_matrices() {

    }
}