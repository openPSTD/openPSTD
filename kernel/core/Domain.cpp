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
//
//
// Authors:
//
//
//////////////////////////////////////////////////////////////////////////

//
// Created by louis on 16-9-15.
//

#include "Domain.h"

namespace Kernel {

    Domain::Domain(const Config cnf, const int id, const double alpha,
                   const int topleft[], const int size[], const bool is_pml,
                   const std::shared_ptr<Domain> pml_for = std::shared_ptr<Domain>(nullptr)) {
        //TODO implementeer dit
    };

    void Domain::calc(BoundaryType bt, CalculationType ct, std::vector<float> dest) {
        std::vector<std::shared_ptr<Domain>> d1, d2;
        if (bt == HORIZONTAL) {
            d1 = this->left;
            d2 = this->right;
        } else {
            d1 = this->bottom;
            d2 = this->top;
        }


        //TODO (Louis) finish this method
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
        if (bt == HORIZONTAL) {
            a_l = this->topleft.x;
            b_l = this->bottomright.x;
        } else {
            a_l = this->topleft.y;
            b_l = this->bottomright.y;
        }
        std::vector<int> tmp(b_l - a_l);
        std::iota(tmp.begin(),tmp.end(), a_l);

        return tmp;
    }
}