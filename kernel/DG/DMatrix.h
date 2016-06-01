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
// Date: 1-6-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_DMATRIX_H
#define OPENPSTD_DMATRIX_H

#include <Eigen/Dense>
#include "GeneralTypes.h"
#include "Vandermonde.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            /**
             * Initialize the (r) differentiation matrices
             * on the interval, evaluated at (r) at order N.
             */
            template <typename SimpleType>
            MatrixX<SimpleType> DMatrix1D(int N, VectorX<SimpleType> r, MatrixX<SimpleType> V)
            {
                MatrixX<SimpleType> Vr = GradVandermonde1D(N, r);
                //return Vr.cwiseQuotient(V);
                return Vr*V.inverse();
            }
        }
    }
}

#endif //OPENPSTD_DMATRIX_H
