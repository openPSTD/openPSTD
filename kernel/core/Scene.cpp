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
// Date: 17-9-15
//
//
// Authors: Omar Richardson
//
//////////////////////////////////////////////////////////////////////////

#include "Scene.h"

using namespace std;
namespace OpenPSTD {
    namespace Kernel {
        Scene::Scene(shared_ptr<PSTDSettings> settings) {
            this->settings = settings;
            this->top_left = Point(0, 0);
            this->bottom_right = Point(0, 0);
            this->size = Point(0, 0);
            number_of_domains = 0;
        }

        void Scene::add_pml_domains() {
            int number_of_cells = settings->GetPMLCells();
            vector<Direction> directions{Direction::LEFT, Direction::TOP, Direction::RIGHT, Direction::BOTTOM};
            vector<string> dir_strings{"left", "top", "right", "bottom"};

            vector<shared_ptr<Domain>> first_order_pmls;
            map<shared_ptr<Domain>, Direction> second_order_pml_map;

            for (shared_ptr<Domain> domain:domain_list) {
                if (domain->is_pml) {
                    continue;
                }
                for (unsigned long i = 0; i < directions.size(); i++) {
                    Direction direction = directions.at(i);
                    Eigen::ArrayXXi vacant_range = domain->get_vacant_range(direction); // todo: Test vacant range.
                    for (unsigned long j = 0; j < vacant_range.rows(); j++) {
                        int pml_id = get_new_id();
                        float alpha = domain->edge_param_map[direction].alpha;
                        float pml_alpha = max(alpha, EPSILON);
                        int x_offset, y_offset, z_offset;
                        int x_size, y_size, z_size;
                        switch (direction) {
                            case Direction::LEFT:
                                x_offset = -number_of_cells;
                                y_offset = vacant_range(j, 0) - domain->top_left.y;
                                x_size = number_of_cells;
                                y_size = vacant_range(j, 1) - vacant_range(j, 0);
                                break;
                            case Direction::RIGHT:
                                x_offset = domain->size.x;
                                y_offset = vacant_range(j, 0) - domain->top_left.y;
                                x_size = number_of_cells;
                                y_size = vacant_range(j, 1) - vacant_range(j, 0);
                                break;
                            case Direction::TOP:
                                x_offset = vacant_range(j, 0) - domain->top_left.x;
                                y_offset = -number_of_cells;
                                x_size = vacant_range(j, 1) - vacant_range(j, 0);
                                y_size = number_of_cells;
                                break;
                            case Direction::BOTTOM:
                                x_offset = vacant_range(j, 0) - domain->top_left.x;
                                y_offset = domain->size.y;
                                x_size = vacant_range(j, 1) - vacant_range(j, 0);
                                y_size = number_of_cells;
                                break;
                        }
                        Point pml_offset(x_offset, y_offset);
                        Point pml_top_left = pml_offset + domain->top_left;
                        Point pml_size(x_size, y_size);
                        Point pml_size_pointer = pml_size;
                        //We only add secondary PML domains for primary PML domains that fully cover their air domain.
                        //If primary PML domain does not, it's set to locally reacting.
                        CalcDirection calcDirection = direction_to_calc_direction(direction);
                        bool full_overlap;
                        switch (calcDirection) {
                            case CalcDirection::X:
                                full_overlap = (vacant_range(j, 0) == domain->top_left.y &&
                                                vacant_range(j, 1) == domain->bottom_right.y);
                                break;
                            case CalcDirection::Y:
                                full_overlap = (vacant_range(j, 0) == domain->top_left.x &&
                                                vacant_range(j, 1) == domain->bottom_right.x);
                                break;
                        }

                        shared_ptr<Domain> pml_domain_ptr(
                                new Domain(settings, pml_id, pml_alpha, pml_top_left, pml_size_pointer, true,
                                           domain->wnd, default_edge_parameters, domain));
                        cout << *pml_domain_ptr << endl;
                        first_order_pmls.push_back(pml_domain_ptr);
                        if (!full_overlap) {
                            pml_domain_ptr->local = true;
                        }
                        if (alpha > 0 and full_overlap) {
                            vector<unsigned long> second_dir_its;
                            // Find directions of the orthogonal calculation direction
                            unsigned long dir_1 = (i + 1) % 4;
                            unsigned long dir_2 = (i + 3) % 4;
                            second_dir_its.push_back(dir_1);
                            second_dir_its.push_back(dir_2);
                            for (unsigned long second_dir_it: second_dir_its) {
                                Direction second_dir = directions.at(second_dir_it);
                                int second_pml_id = get_new_id();
                                float other_pml_alpha = domain->edge_param_map[second_dir].alpha;
                                /* TK: An attempt to prevent refraction on the secondary PML corner.
                                 * This is especially effective in the case of a fully absorbent domain
                                 * connecting to a fully reflective edge.
                                 * For arbitrary combinations, this is an approximation
                                 */
                                float second_pml_alpha = min(max(EPSILON, other_pml_alpha),
                                                             max(EPSILON, pml_alpha));
                                //Todo: This expression can be shortened.
                                int sec_x_offset, sec_y_offset, sec_z_offset;

                                switch (second_dir) {
                                    case Direction::LEFT:
                                        sec_x_offset = -number_of_cells;
                                        sec_y_offset = 0;
                                        break;
                                    case Direction::RIGHT:
                                        sec_x_offset = pml_domain_ptr->size.x;
                                        sec_y_offset = 0;
                                        break;
                                    case Direction::TOP:
                                        sec_x_offset = 0;
                                        sec_y_offset = -number_of_cells;
                                        break;
                                    case Direction::BOTTOM:
                                        sec_x_offset = 0;
                                        sec_y_offset = pml_domain_ptr->size.y;
                                        break;
                                }
                                Point sec_pml_offset(sec_x_offset, sec_y_offset);
                                Point sec_pml_top_left = domain->top_left + pml_offset + sec_pml_offset;
                                Point sec_pml_size(number_of_cells, number_of_cells);
                                shared_ptr<Domain> sec_pml_domain(
                                        new Domain(settings, second_pml_id, second_pml_alpha, sec_pml_top_left,
                                                   sec_pml_size,
                                                   true, domain->wnd, default_edge_parameters, pml_domain_ptr));
                                second_order_pml_map[sec_pml_domain] = second_dir;
                                cout << *sec_pml_domain << endl;
                            }
                        }
                    }
                }
            }
            for (shared_ptr<Domain> domain:first_order_pmls) {
                add_domain(domain);
            }

            //Collect the domains with the same top and size.
            map<vector<int>, vector<shared_ptr<Domain>>> domains_by_cornerpoints;

            for (auto &entry: second_order_pml_map) {
                shared_ptr<Domain> parent_domain = entry.first->pml_for_domain_list.at(0);
                Direction second_dir = entry.second;
                if (!parent_domain->get_neighbours_at(second_dir).empty()) {
                    continue;
                }
                auto corner_points = get_corner_points(entry.first);
                domains_by_cornerpoints[corner_points].push_back(entry.first);

            }
            vector<shared_ptr<Domain>> second_order_pml_list;
            for (auto &entry: domains_by_cornerpoints) {
                if (entry.second.size() == 1) {
                    for (auto sec_pml_domain:entry.second) {
                        second_order_pml_list.push_back(sec_pml_domain);
                    }
                    continue;
                }
                vector<shared_ptr<Domain>> processed_domains;
                for (unsigned long i = 0; i < entry.second.size(); i++) {
                    for (unsigned long j = i + 1; j < entry.second.size(); j++) {
                        shared_ptr<Domain> domain_i = entry.second.at(i);
                        shared_ptr<Domain> domain_j = entry.second.at(j);
                        bool processed_i =
                                find(processed_domains.begin(), processed_domains.end(), domain_i) !=
                                processed_domains.end();
                        bool processed_j =
                                find(processed_domains.begin(), processed_domains.end(), domain_j) !=
                                processed_domains.end();
                        if (processed_i or processed_j) {
                            continue;
                        }
                        if (should_merge_domains(domain_i, domain_j)) {
                            processed_domains.push_back(domain_i);
                            processed_domains.push_back(domain_j);
                            for (auto pml_for_domain: domain_j->pml_for_domain_list) {
                                domain_i->pml_for_domain_list.push_back(pml_for_domain);
                            }
                            second_order_pml_list.push_back(domain_i);
                        }
                    }
                }
                for (unsigned long i = 0; i < entry.second.size(); i++) {
                    if (find(processed_domains.begin(), processed_domains.end(), entry.second.at(i)) ==
                        processed_domains.end()) {
                        second_order_pml_list.push_back(entry.second.at(i));
                    }
                }
            }
            for (auto sec_order_pml_domain: second_order_pml_list) {
                domain_list.push_back(sec_order_pml_domain);
                add_domain(sec_order_pml_domain);
            }
        }


        vector<int> Scene::get_corner_points(shared_ptr<Domain> domain) {
            vector<int> corner_points;
            corner_points.push_back(domain->top_left.x);
            corner_points.push_back(domain->top_left.y);
            corner_points.push_back(domain->bottom_right.x);
            corner_points.push_back(domain->bottom_right.y);
            return corner_points;
        }


        bool Scene::should_merge_domains(shared_ptr<Domain> domain1, shared_ptr<Domain> domain2) {
            shared_ptr<Domain> parent_domain1 = get_singular_parent_domain(get_singular_parent_domain(domain1));
            shared_ptr<Domain> parent_domain2 = get_singular_parent_domain(get_singular_parent_domain(domain2));
            return ((parent_domain1 != nullptr) and (parent_domain1->id == parent_domain2->id));
        }

        shared_ptr<Domain> Scene::get_singular_parent_domain(shared_ptr<Domain> domain) {
            if (domain != nullptr) {
                if (domain->is_pml && domain->pml_for_domain_list.size() == 1) {
                    return domain->pml_for_domain_list.at(0);
                }
            }
            return nullptr;
        }


        void Scene::add_receiver(const float x, const float y, const float z) {
            vector<float> grid_like_location = {x, y, z};
            shared_ptr<Domain> container(nullptr);
            for (auto domain:domain_list) {
                if (!domain->is_pml && domain->contains_location(grid_like_location)) {
                    container = domain;
                }
            }
            assert(container != nullptr);
            int id = (int) (receiver_list.size() + 1);
            shared_ptr<Receiver> receiver(new Receiver(grid_like_location, settings, id, container));
            receiver_list.push_back(receiver);
        }

        void Scene::add_speaker(const float x, const float y, const float z) {
            // Do not really need to be on the heap. Doing it now for consistency with Receiver.

            // Put 0,0 at the actual point 0,0 instead of in the middle of the first pressure sample
            float dx_2 = 0.5; // we're in pressure grid coordinates here, so -0.5 is really 0.5
            vector<float> grid_like_location = {x - dx_2, y - dx_2, z - dx_2};
            shared_ptr<Speaker> speaker(new Speaker(grid_like_location));
            for (unsigned long i = 0; i < domain_list.size(); i++) {
                speaker->addDomainContribution(domain_list.at(i));
                // Todo: Only add when speaker in domain
            }
            speaker_list.push_back(speaker);
        }

        void Scene::compute_pml_matrices() {
            for (auto domain:domain_list) {
                if (domain->is_pml) {
                    domain->compute_pml_matrices();
                }
            }
        }

        void Scene::apply_pml_matrices() {
            for (auto domain:domain_list) {
                if (domain->is_pml) {
                    domain->apply_pml_matrices();
                }
            }
        }

        void Scene::add_domain(shared_ptr<Domain> domain) {
            if (not domain->is_pml) {
                top_left = Point(min(top_left.x, domain->top_left.x),
                                 min(top_left.y, domain->top_left.y));
                bottom_right = Point(max(bottom_right.x, domain->bottom_right.x),
                                     max(bottom_right.y, domain->bottom_right.y));
                size = Point(bottom_right.x - top_left.x, bottom_right.y - top_left.y);
            }
            for (unsigned long i = 0; i < domain_list.size(); i++) {
                shared_ptr<Domain> other_domain = domain_list.at(i);
                if (domain->is_secondary_pml && other_domain->is_secondary_pml) {
                    // Cannot interact, since no secondary PML domains are adjacent
                    continue;
                }
                else if (other_domain->is_secondary_pml && !domain->is_pml) {
                    // Cannot interact, since a regular domain does not touch a secondary PML domain
                    continue;
                }
                if (domain->is_pml && other_domain->is_pml) {
                    bool pml_for_domain = other_domain->is_pml_for(domain);
                    bool pml_for_other_domain = domain->is_pml_for(other_domain);
                    if ((domain->is_secondary_pml && pml_for_other_domain) ||
                        (other_domain->is_secondary_pml && pml_for_domain)) {
                        // Important case: Domain is pml for second domain. Pass
                    }
                    else if (other_domain->pml_for_domain_list.size() != 1 || domain->pml_for_domain_list.size() != 1) {
                        // One is PML for multiple domains. Continue? Todo: check algorithm
                        continue;
                    }
                    else if (!other_domain->pml_for_domain_list.at(0)->is_neighbour_of(
                            domain->pml_for_domain_list.at(0))) {
                        continue;
                    }
                    else if (!domain->pml_for_domain_list.at(0)->is_neighbour_of(
                            other_domain->pml_for_domain_list.at(0))) {
                        //Same
                        continue;
                    }
                    if (domain->is_secondary_pml != other_domain->is_secondary_pml) {
                        //
                        //continue;
                    }
                }
                CalcDirection bt;
                Direction orientation;
                vector<int> intersection;
                bool is_neighbour = true;
                if (domain->bottom_right.x == other_domain->top_left.x) {
                    orientation = Direction::RIGHT;
                    bt = CalcDirection::X;
                }
                else if (domain->top_left.x == other_domain->bottom_right.x) {
                    orientation = Direction::LEFT;
                    bt = CalcDirection::X;
                }
                else if (domain->bottom_right.y == other_domain->top_left.y) {
                    orientation = Direction::BOTTOM;
                    bt = CalcDirection::Y;
                }
                else if (domain->top_left.y == other_domain->bottom_right.y) {
                    orientation = Direction::TOP;
                    bt = CalcDirection::Y;
                }
                else {
                    is_neighbour = false;
                }
                bool other_domain_pml_for_different_domain =
                        other_domain->is_pml && !domain->is_pml && !other_domain->is_pml_for(domain);
                bool domain_pml_for_different_domain =
                        domain->is_pml && !other_domain->is_pml && !domain->is_pml_for(other_domain);
                if (is_neighbour && !other_domain_pml_for_different_domain && !domain_pml_for_different_domain) {
                    intersection = domain->get_intersection_with(other_domain, orientation);
                    if (intersection.size()) {
                        shared_ptr<Boundary> boundary(new Boundary(domain, other_domain, bt));
                        boundary_list.push_back(boundary);
                        domain->add_neighbour_at(other_domain, orientation);
                        other_domain->add_neighbour_at(domain, get_opposite(orientation));
                    }
                }
            }
            domain_list.push_back(domain);
        }

        Eigen::ArrayXXf Scene::get_pressure_field() {
            return get_field('p');
        }

        Eigen::ArrayXXf Scene::get_field(char field_type) {
            Eigen::ArrayXXf field(size.y, size.x);
            for (auto domain:domain_list) {
                if (not domain->is_pml) {
                    Point offset = domain->top_left - top_left;
                    switch (field_type) {
                        case 'p':
                            field.block(offset.y, offset.x, domain->size.y,
                                        domain->size.x) += domain->current_values.p0;
                            break;
                        default:
                            //No other fields are required (yet). However, leaving open for extension.
                            break;
                    }
                }
            }
            return field;
        }

        shared_ptr<Domain> Scene::get_domain(int id) {
            // Probably not a necessary function.
            shared_ptr<Domain> correct_domain;
            for (auto domain:domain_list) {
                if (domain->id == id) {
                    assert(correct_domain == nullptr);
                    correct_domain = domain;
                }
            }
            return correct_domain;
        }

        ostream &operator<<(ostream &str, Scene const &v) {
            return str << "Scene: " << v.domain_list.size() << " domains, " << v.speaker_list.size() << " speakers, " <<
                   v.receiver_list.size() << " receivers";

        }

        int Scene::get_new_id() {
            number_of_domains++;
            return number_of_domains - 1;
        }
    }
}
