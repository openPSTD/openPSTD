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
// Date: 8-12-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#include "Grad.h"

using namespace OpenPSTD::Kernel::DG;

template Grad2DResult<float> OpenPSTD::Kernel::DG::Grad2D(
        MatrixX<float> Dr,
        MatrixX<float> Ds,
        VectorX<float> rx,
        VectorX<float> ry,
        VectorX<float> sx,
        VectorX<float> sy,
        VectorX<float> u);
template Grad2DResult<double> OpenPSTD::Kernel::DG::Grad2D(
        MatrixX<double> Dr,
        MatrixX<double> Ds,
        VectorX<double> rx,
        VectorX<double> ry,
        VectorX<double> sx,
        VectorX<double> sy,
        VectorX<double> u);

template struct Grad2DResult<float>;
template struct Grad2DResult<double>;
