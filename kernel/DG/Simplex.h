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

#ifndef OPENPSTD_SIMPLEX_H
#define OPENPSTD_SIMPLEX_H

#include <cmath>
#include <Eigen/Dense>
#include "GeneralTypes.h"
#include "Jacobi.h"


namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            /**
             * Evaluate 2D orthonormal polynomial on simplex at (a,b) of order (i,j).
             * @param a
             * @param b
             * @param i
             * @param j
             * @return
             */
            template <typename SimpleType>
            VectorX<SimpleType> Simplex2DP(ArrayXX<SimpleType> ab, int i, int j)
            {
                VectorX<SimpleType> a = ab.col(0);
                VectorX<SimpleType> b = ab.col(1);
                ArrayX<SimpleType> h1 = JacobiP(a, (SimpleType)0, (SimpleType)0, i);
                ArrayX<SimpleType> h2 = JacobiP(b, (SimpleType)2*i+1, (SimpleType)0, j);
                ArrayX<SimpleType> tmp = b.unaryExpr([i](SimpleType x){return std::pow(1-x, i); });
                return 1.4142135623730951*h1*h2*tmp;//1.4142135623730951 == sqrt(2.0)

            }
        }
    }
}
#endif //OPENPSTD_SIMPLEX_H
