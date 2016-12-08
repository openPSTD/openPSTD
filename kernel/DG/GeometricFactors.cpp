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

#include "GeometricFactors.h"

using namespace OpenPSTD::Kernel::DG;

template MatrixX<float> OpenPSTD::Kernel::DG::GeometricFactors1D_J(const MatrixX<float>& x, const MatrixX<float>& Dr);
template MatrixX<double> OpenPSTD::Kernel::DG::GeometricFactors1D_J(const MatrixX<double>& x, const MatrixX<double>& Dr);

template MatrixX<float> OpenPSTD::Kernel::DG::GeometricFactors1D_rx(const MatrixX<float>& J);
template MatrixX<double> OpenPSTD::Kernel::DG::GeometricFactors1D_rx(const MatrixX<double>& J);

template MatrixX<float> OpenPSTD::Kernel::DG::GeometricFactors1D_rx(const MatrixX<float>& x, const MatrixX<float>& Dr);
template MatrixX<double> OpenPSTD::Kernel::DG::GeometricFactors1D_rx(const MatrixX<double>& x, const MatrixX<double>& Dr);

template struct GeometricFactors2DResult<float>;
template struct GeometricFactors2DResult<double>;

template GeometricFactors2DResult<float> OpenPSTD::Kernel::DG::GeometricFactors2D(const MatrixX<float>& x, const MatrixX<float>& y, const MatrixX<float>& Dr, const MatrixX<float>& Ds);
template GeometricFactors2DResult<double> OpenPSTD::Kernel::DG::GeometricFactors2D(const MatrixX<double>& x, const MatrixX<double>& y, const MatrixX<double>& Dr, const MatrixX<double>& Ds);
