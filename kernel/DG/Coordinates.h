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
// Date: 7-11-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#include "GeneralTypes.h"

#ifndef OPENPSTD_COORDINATES_H
#define OPENPSTD_COORDINATES_H

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            /**
             * Transfer from (r,s) -> (a,b) coordinates in triangle
             */
            template <typename SimpleType>
            ArrayXX<SimpleType> rstoab(ArrayXX<SimpleType> rs)
            {
                int Np = rs.rows();
                ArrayXX<SimpleType> ab(Np, 2);
                for (int i = 0; i < Np; ++i)
                {
                    if(rs(i, 1) != 1)
                        ab(i, 0) = 2*(1+rs(i, 0))/(1-rs(i, 1))-1;
                    else
                        ab(i, 0) = -1;
                }
                ab.col(1) = rs.col(1);
                return ab;
            }

            /**
             * From (x,y) in equilateral triangle to (r,s) coordinates in standard triangle
             * @param xy equilateral triangle coordinates (xy)
             * @return coordinates in rs (standard triangle)
             */
            template <typename SimpleType>
            ArrayXX<SimpleType> xytors(ArrayXX<SimpleType> xy)
            {
                int Np = xy.rows();
                ArrayXX<SimpleType> rs(Np, 2);

                auto x = xy.col(0);
                auto y = xy.col(1);

                auto L1 = (sqrt(3.0)*y+1.0)/3.0;
                auto L2 = (-3.0*x - sqrt(3.0)*y+2.0)/6.0;
                auto L3 = ( 3.0*x - sqrt(3.0)*y+2.0)/6.0;

                rs.col(0) = -L2 + L3 - L1;
                rs.col(1) = -L2 - L3 + L1;

                return rs;
            }

            extern template ArrayXX<double> xytors(ArrayXX<double> xy);
            extern template ArrayXX<float> xytors(ArrayXX<float> xy);

            extern template ArrayXX<double> rstoab(ArrayXX<double> xy);
            extern template ArrayXX<float> rstoab(ArrayXX<float> xy);
        }
    }
}
#endif //OPENPSTD_COORDINATES_H
