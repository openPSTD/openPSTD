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
// Date: 9-11-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_GRAD_H
#define OPENPSTD_GRAD_H

#include <cmath>
#include <Eigen/Dense>
#include "GeneralTypes.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            template <typename SimpleType>
            struct Grad2DResult
            {
            public:
                VectorX<SimpleType> ux;
                VectorX<SimpleType> uy;
            };

            /**
             * Compute 2D gradient field of scalar u
             */
            template <typename SimpleType>
            Grad2DResult<SimpleType> Grad2D(
                    MatrixX<SimpleType> Dr,
                    MatrixX<SimpleType> Ds,
                    VectorX<SimpleType> rx,
                    VectorX<SimpleType> ry,
                    VectorX<SimpleType> sx,
                    VectorX<SimpleType> sy,
                    VectorX<SimpleType> u)
            {
                Grad2DResult<SimpleType> result;
                VectorX<SimpleType> ur = Dr*u;
                VectorX<SimpleType> us = Ds*u;

                result.ux = rx.array()*ur.array() + sx.array()*us.array();
                result.uy = ry.array()*ur.array() + sy.array()*us.array();

                return result;
            }

            extern template Grad2DResult<float> Grad2D(
                                                        MatrixX<float> Dr,
                                                        MatrixX<float> Ds,
                                                        VectorX<float> rx,
                                                        VectorX<float> ry,
                                                        VectorX<float> sx,
                                                        VectorX<float> sy,
                                                        VectorX<float> u);
            extern template Grad2DResult<double> Grad2D(
                                                        MatrixX<double> Dr,
                                                        MatrixX<double> Ds,
                                                        VectorX<double> rx,
                                                        VectorX<double> ry,
                                                        VectorX<double> sx,
                                                        VectorX<double> sy,
                                                        VectorX<double> u);

            extern template struct Grad2DResult<float>;
            extern template struct Grad2DResult<double>;
        }
    }
}

#endif //OPENPSTD_GRAD_H
