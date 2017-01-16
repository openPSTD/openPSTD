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

#include "Simplex.h"

using namespace OpenPSTD::Kernel::DG;

template VectorX<float> OpenPSTD::Kernel::DG::Simplex2DP(ArrayXX<float> ab, int i, int j);
template VectorX<double> OpenPSTD::Kernel::DG::Simplex2DP(ArrayXX<double> ab, int i, int j);

template GradSimplex2DPResult<float> OpenPSTD::Kernel::DG::GradSimplex2DP(ArrayXX<float> ab, int id, int jd);
template GradSimplex2DPResult<double> OpenPSTD::Kernel::DG::GradSimplex2DP(ArrayXX<double> ab, int id, int jd);

template struct GradSimplex2DPResult<float>;
template struct GradSimplex2DPResult<double>;
