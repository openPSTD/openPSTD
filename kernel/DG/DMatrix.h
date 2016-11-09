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
                return Vr*V.fullPivLu().inverse();
            }


            template <typename SimpleType>
            struct DMatrices2DResult
            {
            public:
                MatrixX<SimpleType> Dr;
                MatrixX<SimpleType> Ds;
            };

            /**
             * Initialize the (r,s) differentiation matrices
             * on the simplex, evaluated at (r,s) at order N
             * @param N
             * @param rs
             * @param V
             * @return
             */
            template <typename SimpleType>
            DMatrices2DResult<SimpleType> DMatrices2D(int N, MatrixX<SimpleType> rs, MatrixX<SimpleType> V)
            {
                DMatrices2DResult<SimpleType> result;

                //[Vr, Vs] = GradVandermonde2D(N, r, s);
                GradVandermonde2DResult<SimpleType> gradV = GradVandermonde2D(N, rs);

                //Dr = Vr/V; Ds = Vs/V;
                MatrixX<SimpleType> vInv = V.fullPivLu().inverse();
                result.Dr = gradV.V2Dr*vInv;
                result.Ds = gradV.V2Ds*vInv;

                return result;
            }
        }
    }
}

#endif //OPENPSTD_DMATRIX_H
