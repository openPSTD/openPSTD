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
// Date: 31-5-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_VANDERMONDE_H
#define OPENPSTD_VANDERMONDE_H

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
             * Initialize the 1D Vandermonde Matrix.
             */
            template <typename SimpleType>
            MatrixX<SimpleType> Vandermonde1D(int N, VectorX<SimpleType> xp)
            {
                MatrixX<SimpleType> result(xp.size(), N+1);

                for (int j=1; j<=(N+1); ++j) {
                    int J = j-1;
                    result.col(J) = JacobiP<SimpleType>(xp, 0, 0, j-1);
                }
                return result;
            }

        }
    }
}

#endif //OPENPSTD_VANDERMONDE_H
