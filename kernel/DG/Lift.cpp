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

#include "Lift.h"

using namespace OpenPSTD::Kernel::DG;

template MatrixX<float> OpenPSTD::Kernel::DG::Lift1D(int Np, int NFaces, MatrixX<float> V);
template MatrixX<double> OpenPSTD::Kernel::DG::Lift1D(int Np, int NFaces, MatrixX<double> V);

template MatrixX<float> OpenPSTD::Kernel::DG::Lift2D(int N, int NFaces, MatrixX<float> V, MatrixX<float> rs);
template MatrixX<double> OpenPSTD::Kernel::DG::Lift2D(int N, int NFaces, MatrixX<double> V, MatrixX<double> rs);
