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

#ifndef OPENPSTD_GEOMETRICFACTORS_H
#define OPENPSTD_GEOMETRICFACTORS_H

#include <Eigen/Dense>
#include "GeneralTypes.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            /**
             * Compute Jacobian for the metric elements for the local mappings of the 1D elements.
             */
            template<typename SimpleType>
            MatrixX<SimpleType> GeometricFactors1D_J(MatrixX<SimpleType> x, MatrixX<SimpleType> Dr)
            {
                return Dr*x;
            }

            /**
             * Compute the metric elements for the local mappings of the 1D elements.
             */
            template<typename SimpleType>
            MatrixX<SimpleType> GeometricFactors1D_rx(MatrixX<SimpleType> J)
            {
                return J.unaryExpr([](SimpleType x){return 1/x;});
            }


            /**
             * Compute the metric elements for the local mappings of the 1D elements.
             */
            template<typename SimpleType>
            MatrixX<SimpleType> GeometricFactors1D_rx(MatrixX<SimpleType> x, MatrixX<SimpleType> Dr)
            {
                return GeometricFactors1D_rx(GeometricFactors1D_J(x, Dr));
            }

        }
    }
}
#endif //OPENPSTD_GEOMETRICFACTORS_H
