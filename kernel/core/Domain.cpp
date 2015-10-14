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
// Date:
//      17-9-2015
//
// Authors:
//      Louis van Harten
//
//////////////////////////////////////////////////////////////////////////

#include "Domain.h"

namespace Kernel {

    Domain::Domain(std::shared_ptr<PSTDFileSettings> settings, const int id, const double alpha,
                   std::shared_ptr<Point> top_left, std::shared_ptr<Point> size, const bool is_pml,
                   std::shared_ptr<WaveNumberDiscretizer> wnd,
                   const std::shared_ptr<Domain> pml_for = std::shared_ptr<Domain>(nullptr)
    ) {
        //Todo (Louis): Implementeer dit verder
        this->settings = settings;
        this->top_left = top_left;
        this->size = size;
        this->bottom_right = bottom_right;
        this->wnd = wnd;
    };

    void Domain::calc(BoundaryType bt, CalculationType ct, std::vector<float> dest) {
        std::vector<std::shared_ptr<Domain>> domains1, domains2;
        if (bt == BoundaryType::HORIZONTAL) {
            domains1 = this->left;
            domains2 = this->right;
        } else {
            domains1 = this->bottom;
            domains2 = this->top;
        }

        std::vector<int> own_range = get_range(bt);

        Eigen::ArrayXXf source;

        if (!dest.empty() || ct == CalculationType::PRESSURE) {
            if (bt == BoundaryType::HORIZONTAL) {
                source = extended_zeros(0,1);
            } else {
                source = extended_zeros(1,0);
            }
        } else {
            source = extended_zeros(0,0);
        }

        // Note: this does not iterate over an empty set for either domain like the Python version
        // used to do, as surrounding domains are always added to domains calc is called on.
        // TODO, urgent: not actually sure if this is correct anymore, check before proceeding
        for (int i = 0; i != domains1.size(); i++ ) {
            for (int j = 0; i != domains2.size(); j++) {
                std::vector<std::shared_ptr<Domain>> rho_matrix_key;
                rho_matrix_key.push_back(domains1[i]);
                rho_matrix_key.push_back(domains2[j]);

                std::vector<int> range_intersection, range1, range2;
                range1 = domains1[i]->get_range(bt);
                range2 = domains2[j]->get_range(bt);
                set_intersection(range1.begin(), range1.end(),
                                 range2.begin(), range2.end(),
                                 back_inserter(range_intersection));

                if (range_intersection.size() == 0) {
                    continue;
                }

                int range_start = *std::min_element(range_intersection.begin(),range_intersection.end());
                int range_end = *std::max_element(range_intersection.begin(),range_intersection.end())+1;
                int primary_dimension = (bt == BoundaryType::HORIZONTAL) ? this->size->x : this->size->y;
                int N_total = 2*this->settings->GetWindowSize();

                if (ct == CalculationType::PRESSURE) {
                    N_total++;
                } else {
                    primary_dimension++;
                }

                //TODO (Louis) finish this method
            }
        }
    }

    /**
     * Near-alias to calc(BoundaryType bt, CalculationType ct, std::vector<float> dest), but with
     * a default empty vector as dest.
     */
    void Domain::calc(BoundaryType bt, CalculationType ct) {
        std::vector<float> dest;
        Domain::calc(bt, ct, dest);
    }


    std::vector<int> Domain::get_range(BoundaryType bt) {
        int a_l,b_l;
        if (bt == BoundaryType::HORIZONTAL) {
            a_l = this->top_left->x;
            b_l = this->bottom_right->x;
        } else {
            a_l = this->top_left->y;
            b_l = this->bottom_right->y;
        }
        std::vector<int> tmp(b_l - a_l);
        std::iota(tmp.begin(),tmp.end(), a_l);
        return tmp;
    }

    Eigen::ArrayXXf Domain::extended_zeros(int x, int y, int z) {
        Eigen::ArrayXXf tmp(this->size->x + x, this->size->y + y);
        tmp.setZero();
        return tmp;
    }

    std::vector<std::shared_ptr<Domain>> Domain::get_neighbours_at(Direction direction) {
        // Make one that obtains a vector of domains, and one that obtains a domain in a location
        std::vector<std::shared_ptr<Domain>> s;
        //NYI
        return s;
    }

    std::shared_ptr<Domain> Domain::get_neighbour_at(Direction direction, std::vector<double> location) {
        return NULL;
    }
}