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

#ifndef OPENPSTD_LIFT_H
#define OPENPSTD_LIFT_H

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
             * Compute surface integral term in DG formulation.
             * @param Np: Number of points in an element
             * @param NFaces: Number of faces of an element
             * @param V: Vandermonde matrix
             */
            template <typename SimpleType>
            MatrixX<SimpleType> Lift1D(int Np, int NFaces, MatrixX<SimpleType> V)
            {
                MatrixX<SimpleType> Emat = MatrixX<SimpleType>::Zero(Np, NFaces);
                Emat(0, 0) = 1;
                Emat(Np-1, 1) = 1;
                return V*(V.transpose()*Emat);
            }
        }
    }
}

#endif //OPENPSTD_LIFT_H
