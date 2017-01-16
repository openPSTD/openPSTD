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

#include "Vandermonde.h"

using namespace OpenPSTD::Kernel::DG;

template MatrixX<float> OpenPSTD::Kernel::DG::Vandermonde1D(int N, VectorX<float> r);
template MatrixX<double> OpenPSTD::Kernel::DG::Vandermonde1D(int N, VectorX<double> r);

template MatrixX<float> OpenPSTD::Kernel::DG::GradVandermonde1D(int N, VectorX<float> r);
template MatrixX<double> OpenPSTD::Kernel::DG::GradVandermonde1D(int N, VectorX<double> r);

template MatrixX<float> OpenPSTD::Kernel::DG::Vandermonde2D(int N, MatrixX<float> rs);
template MatrixX<double> OpenPSTD::Kernel::DG::Vandermonde2D(int N, MatrixX<double> rs);

template GradVandermonde2DResult<float> OpenPSTD::Kernel::DG::GradVandermonde2D(int N, MatrixX<float> rs);
template GradVandermonde2DResult<double> OpenPSTD::Kernel::DG::GradVandermonde2D(int N, MatrixX<double> rs);

template struct GradVandermonde2DResult<float>;
template struct GradVandermonde2DResult<double>;