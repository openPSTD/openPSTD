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

namespace Kernel {
    Scene::Scene(std::shared_ptr<PSTDFileConfiguration> config) {
        this->config = config;
        this->settings = std::make_shared<PSTDFileSettings>(config->Settings);
    }

    void Scene::add_pml_domains() {
        int number_of_cells = this->settings->GetPMLCells();
        std::vector<Direction> directions{Direction::LEFT, Direction::RIGHT, Direction::TOP, Direction::BOTTOM};
        std::vector<std::string> dir_strings{"left", "top", "right", "bottom"};


        std::vector<std::shared_ptr<Domain>> first_order_pmls;
        std::vector<std::shared_ptr<Domain>> second_order_pmls;

        for (std::shared_ptr<Domain> domain:this->domain_list) {
            if (domain->is_pml) {
                continue;
            }
            for (unsigned long i = 0; i < directions.size(); i++) {
                Direction direction = directions.at(i);
                Eigen::ArrayXXi vacant_range = domain->get_vacant_range(direction);
                for (unsigned long j = 0; j < vacant_range.rows(); j++) {
                    std::ostringstream pml_ss;
                    pml_ss << domain->id << dir_strings.at(i);
                    if (vacant_range.rows() > 1) {
                        pml_ss << "_" << j;
                    }
                    std::string pml_id = pml_ss.str();
                    float alpha = domain->edge_param_map[direction].alpha;
                    float pml_alpha = std::max(alpha, EPSILON);
                    int x_offset, y_offset, z_offset;
                    int x_size, y_size, z_size;
                    switch (direction) {
                        case Direction::LEFT:
                            x_offset = -number_of_cells;
                            y_offset = vacant_range(j, 0) - domain->top_left->y;
                            x_size = number_of_cells;
                            y_size = vacant_range(j, 1) - vacant_range(j, 0);
                            break;
                        case Direction::RIGHT:
                            x_offset = domain->size->x;
                            y_offset = vacant_range(j, 0) - domain->top_left->y;
                            x_size = number_of_cells;
                            y_size = vacant_range(j, 1) - vacant_range(j, 0);
                            break;
                        case Direction::TOP:
                            x_offset = vacant_range(j, 0) - domain->top_left->x;
                            y_offset = domain->size->y;
                            x_size = vacant_range(j, 1) - vacant_range(j, 0);
                            y_size = number_of_cells;
                            break;
                        case Direction::BOTTOM:
                            x_offset = vacant_range(j, 0) - domain->top_left->x;
                            y_offset = -number_of_cells;
                            x_size = vacant_range(j, 1) - vacant_range(j, 0);
                            y_size = number_of_cells;
                            break;
                    }
                    Point pml_offset(x_offset, y_offset);
                    std::shared_ptr<Point> pml_top_left = std::make_shared<Point>(pml_offset + *domain->top_left);
                    Point pml_size(x_size, y_size);
                    std::shared_ptr<Point> pml_size_pointer = std::make_shared<Point>(pml_size);
                    //We only add secondary PML domains for primary PML domains that fully cover their air domain.
                    //If primary PML domain does not, it's set to locally reacting.
                    CalcDirection calcDirection = direction_to_calc_direction(direction);
                    bool full_overlap = false;
                    switch (calcDirection) {
                        case CalcDirection::X:
                            full_overlap = (vacant_range(j, 0) == domain->top_left->y &&
                                            vacant_range(j, 1) == domain->bottom_right->y);
                            break;
                        case CalcDirection::Y:
                            full_overlap = (vacant_range(j, 0) == domain->top_left->x &&
                                            vacant_range(j, 1) == domain->bottom_right->x);
                            break;
                    }
                    Domain *pml_domain = new Domain(this->settings, pml_id, pml_alpha, pml_top_left, pml_size_pointer,
                                                    true, domain->wnd, domain);
                    //std::shared_ptr<Domain> pml_domain_ptr(pml_domain); // Todo: Inaccessible base ...?
                    //first_order_pmls.push_back(pml_domain_ptr);
                    if (!full_overlap) {
                        pml_domain->local = true;
                    }
                    if (alpha > 0 and full_overlap) {
                        std::vector<unsigned long> second_dir_its;
                        unsigned long dir_1 = ((i) + 1) % 4;
                        unsigned long dir_2 = ((i) + 3) % 4;
                        second_dir_its.push_back(dir_1);
                        second_dir_its.push_back(dir_2);
                        for (unsigned long second_dir_it: second_dir_its) {
                            Direction second_dir = directions.at(second_dir_it);
                            std::stringstream second_pml_id_ss;
                            second_pml_id_ss << domain->id << "_" << dir_strings.at(second_dir_it);
                            std::string second_pml_id = second_pml_id_ss.str();
                            float other_pml_alpha = domain->edge_param_map[second_dir].alpha;
                            /* TK: An attempt to prevent refraction on the secondary PML corner.
                             * This is especially effective in the case of a fully absorbent domain
                             * connecting to a fully reflective edge.
                             * For arbitrary combinations, this is an approximation
                             */
                            float second_pml_alpha = std::min(std::max(EPSILON, other_pml_alpha),
                                                              std::max(EPSILON, pml_alpha));
                            //Todo: This really seems equivalent to std::min(std::max(other_pml_alpha,pml_alpha))
                            int sec_x_offset, sec_y_offset, sec_z_offset;

                            switch (second_dir) {
                                case Direction::LEFT:
                                    sec_x_offset = -number_of_cells;
                                    sec_y_offset = 0;
                                    break;
                                case Direction::RIGHT:
                                    sec_x_offset = pml_domain->size->x;
                                    sec_y_offset = 0;
                                    break;
                                case Direction::TOP:
                                    sec_x_offset = 0;
                                    sec_y_offset = -number_of_cells;
                                    break;
                                case Direction::BOTTOM:
                                    sec_x_offset = 0;
                                    sec_y_offset = pml_domain->size->y;
                                    break;
                            }

                            Point sec_pml_offset(sec_x_offset, sec_y_offset);
                            Point sec_pml_size(number_of_cells, number_of_cells);
                            //Domain *sec_pml_domain = new Domain(this->settings,second_pml_id,second_pml_alpha,pml_top_left,pml_size_pointer,true,domain->wnd,pml_domain);
                            // Todo: Fix points (see up) and fix pointers.
                        }
                    }
                }
            }
        }

        for (std::shared_ptr<Domain> domain:first_order_pmls) {
            this->add_domain(domain);
        }

        // Corner points of second pml domains should be unique.
        // Checking with asserts. If true, we change the algorithm.

        for (std::shared_ptr<Domain> sec_pml_domain1: second_order_pmls) {
            for (std::shared_ptr<Domain> sec_pml_domain2:second_order_pmls) {
                if (sec_pml_domain1 != sec_pml_domain2) {
                    auto corners1 = get_corner_points(sec_pml_domain1);
                    auto corners2 = get_corner_points(sec_pml_domain2);
                    assert(corners1 != corners2);
                }
            }
        }


    }

    std::vector<int> Scene::get_corner_points(std::shared_ptr<Domain> domain) {
        std::vector<int> corner_points;
        corner_points.push_back(domain->top_left->x);
        corner_points.push_back(domain->top_left->y);
        corner_points.push_back(domain->bottom_right->x);
        corner_points.push_back(domain->bottom_right->y);
        return corner_points;
    }

    bool Scene::should_merge_domains(std::shared_ptr<Domain> domain1, std::shared_ptr<Domain> domain2) {
        std::shared_ptr<Domain> parent_domain1 = get_singular_parent_domain(domain1);
        std::shared_ptr<Domain> parent_domain2 = get_singular_parent_domain(domain2);
        return (domain1 != nullptr && domain1->id == domain2->id);
    }

    std::shared_ptr<Domain> Scene::get_singular_parent_domain(std::shared_ptr<Domain> domain) {
        if (domain != nullptr) {
            if (domain->is_pml && domain->pml_for_domain_list.size() == 1) {
                return domain->pml_for_domain_list.at(0);
            }
        }
        return nullptr;
    }


    void Scene::add_receiver(const float x, const float y, const float z) {
        std::vector<float> grid_like_location = {x, y, z};
        std::shared_ptr<Domain> container(nullptr);
        for (unsigned long i = 0; i < this->domain_list.size(); i++) {
            std::shared_ptr<Domain> domain = this->domain_list.at(i);
            if (domain->is_pml && domain->contains_location(grid_like_location)) {
                container = domain;
            }
        }
        assert(container != nullptr);
        int id = (int) (receiver_list.size() + 1);
        std::shared_ptr<Receiver> receiver(new Receiver(grid_like_location, this->settings, id, container));
        this->receiver_list.push_back(receiver);
    }

    void Scene::add_speaker(const float x, const float y, const float z) {
        // Do not really need to be on the heap. Doing it now for consistency with Receiver.
        std::vector<float> grid_like_location = {x, y, z};
        std::shared_ptr<Speaker> speaker(new Speaker(grid_like_location));
        for (unsigned long i = 0; i < this->domain_list.size(); i++) {
            speaker->addDomainContribution(this->domain_list.at(i));
        }
        this->speaker_list.push_back(speaker);
    }

    void Scene::compute_rho_matrices() {
        for (unsigned long i = 0; i < this->domain_list.size(); i++) {
            this->domain_list.at(i)->calc_rho_matrices();
        }
    }


    void Scene::add_domain(std::shared_ptr<Domain> domain) {
        for (unsigned long i = 0; i < this->domain_list.size(); i++) {
            std::shared_ptr<Domain> other_domain = this->domain_list.at(i);
            if (domain->is_sec_pml && other_domain->is_sec_pml) {
                // Cannot interact, since no secondary PML domains are adjacent
                continue;
            } else if (domain->is_sec_pml && !other_domain->is_pml) {
                // Cannot interact, since a regular domain does not touch a secondary PML domain
                continue;
            } else if (other_domain->is_sec_pml && !domain->is_pml) {
                // Cannot interact, since a regular domain does not touch a secondary PML domain
                continue;
            }
            if (domain->is_pml && other_domain->is_pml) {
                bool pml_for_domain = other_domain->is_pml_for(domain);
                bool pml_for_other_domain = domain->is_pml_for(other_domain);
                if ((domain->is_sec_pml && pml_for_other_domain) || (other_domain->is_sec_pml && pml_for_domain)) {
                    // Important case: Domain is pml for second domain. Pass
                } else if (other_domain->pml_for_domain_list.size() != 1 || domain->pml_for_domain_list.size() != 1) {
                    // One is PML for multiple domains. Continue? Todo: check
                    continue;
                } else if (!other_domain->pml_for_domain_list.at(0)->is_neighbour_of(
                        domain->pml_for_domain_list.at(0))) {
                    //Todo: Why just check the first one?
                    continue;
                }
                else if (!domain->pml_for_domain_list.at(0)->is_neighbour_of(other_domain->pml_for_domain_list.at(0))) {
                    //Same
                    continue;
                }
                if (domain->is_sec_pml != other_domain->is_sec_pml) {
                    //
                    continue;
                }
            }
            CalcDirection bt;
            Direction orientation;
            std::vector<int> intersection;
            bool is_neighbour = true;
            if (domain->bottom_right->x == other_domain->top_left->x) {
                orientation = Direction::RIGHT;
                bt = CalcDirection::Y;
            } else if (domain->top_left->x == other_domain->bottom_right->x) {
                orientation = Direction::LEFT;
                bt = CalcDirection::Y;
            } else if (domain->bottom_right->y == other_domain->top_left->y) {
                orientation = Direction::BOTTOM;
                bt = CalcDirection::X;
            } else if (domain->top_left->y == other_domain->bottom_right->y) {
                orientation = Direction::TOP;
                bt = CalcDirection::X;
            } else {
                is_neighbour = false;
            }
            bool other_domain_pml_for_different_domain =
                    other_domain->is_pml && !domain->is_pml && !other_domain->is_pml_for(domain);
            bool domain_pml_for_different_domain =
                    domain->is_pml && !other_domain->is_pml && !domain->is_pml_for(other_domain);
            if (is_neighbour && !other_domain_pml_for_different_domain && !domain_pml_for_different_domain) {
                intersection = domain->get_intersection_with(other_domain, orientation);
                if (intersection.size()) {
                    std::shared_ptr<Boundary> boundary(new Boundary(domain, other_domain, bt));
                    this->boundary_list.push_back(boundary);
                    domain->add_neighbour_at(other_domain, orientation);
                    other_domain->add_neighbour_at(domain, get_opposite(orientation));
                }
            }
        }
        this->domain_list.push_back(domain);
    }
}