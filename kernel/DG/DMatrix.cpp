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

#include "DMatrix.h"

using namespace OpenPSTD::Kernel::DG;

template MatrixX<float> OpenPSTD::Kernel::DG::DMatrix1D(int N, VectorX<float> r, MatrixX<float> V);
template MatrixX<double> OpenPSTD::Kernel::DG::DMatrix1D(int N, VectorX<double> r, MatrixX<double> V);

template DMatrices2DResult<float> OpenPSTD::Kernel::DG::DMatrices2D(int N, MatrixX<float> rs, MatrixX<float> V);
template DMatrices2DResult<double> OpenPSTD::Kernel::DG::DMatrices2D(int N, MatrixX<double> rs, MatrixX<double> V);

template struct DMatrices2DResult<float>;
template struct DMatrices2DResult<double>;
