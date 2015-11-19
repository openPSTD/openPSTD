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
// Authors: Omar
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
        std::vector<std::string> dir_strings{"left", "right", "top", "bottom"};


        std::vector<std::shared_ptr<Domain>> first_order_pmls;
        std::vector<std::shared_ptr<Domain>> second_order_pmls;

        for (std::shared_ptr<Domain> domain:this->domain_list) {
            if (domain->is_pml) {
                continue;
            }
            for (int i = 0; i < directions.size(); i++) {
                Direction direction = directions.at(i);
                Eigen::ArrayXXi vacant_range = domain->get_vacant_range(direction);
                for (int i = 0; i < vacant_range.rows(); i++) {
                    std::ostringstream pml_ss;
                    pml_ss << domain->id << dir_strings.at(i);
                    if (vacant_range.rows() > 1) {
                        pml_ss << "_" << i;
                    }
                    int x, y, z;
                    switch (direction) {
                        case Direction::LEFT:
                            x = -number_of_cells;
                            y = vacant_range(i, 0) - domain->top_left->y;
                            break;
                        case Direction::RIGHT:
                            x = domain->size->x;
                            y = vacant_range(i, 0) - domain->top_left->y;
                            break;
                        case Direction::TOP:
                            x = vacant_range(i, 0) - domain->top_left->x;
                            y = domain->size->y;
                            break;
                        case Direction::BOTTOM:
                            x = vacant_range(i, 0) - domain->top_left->x;
                            y = -number_of_cells;
                            break;
                    }
                    //todo (0mar) : Finish
                }
            }
        }

    }

    void Scene::add_domain(std::shared_ptr<Domain> domain) {
        for (int i = 0; i < this->domain_list.size(); i++) {
            std::shared_ptr<Domain> other_domain = domain_list.at(i);
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
                    // One is PML for multiple domains. Continue? Todo...
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


    void Scene::add_receiver(const float x, const float y, const float z) {
        std::vector<float> grid_like_location = {x, y, z};
        std::shared_ptr<Domain> container(nullptr);
        for (int i = 0; i < this->domain_list.size(); i++) {
            std::shared_ptr<Domain> domain = this->domain_list.at(i);
            if (domain->is_pml && domain->contains_location(grid_like_location)) {
                container = domain;
            }
        }
        assert(container != nullptr);
        int id = receiver_list.size() + 1;
        std::shared_ptr<Receiver> receiver(new Receiver(grid_like_location, this->settings, id, container));
        this->receiver_list.push_back(receiver);
    }

    void Scene::add_speaker(const float x, const float y, const float z) {
        // Do not really need to be on the heap. Doing it now for consistency with Receiver.
        std::vector<float> grid_like_location = {x, y, z};
        std::shared_ptr<Speaker> speaker(new Speaker(grid_like_location));
        for (int i = 0; i < this->domain_list.size(); i++) {
            speaker->addDomainContribution(this->domain_list.at(i));
        }
        this->speaker_list.push_back(speaker);
    }

    void Scene::compute_rho_matrices() {
        for (int i = 0; i < this->domain_list.size(); i++) {
            this->domain_list.at(i)->calc_rho_matrices();
        }
    }


}