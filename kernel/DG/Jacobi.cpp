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

#include "Jacobi.h"

using namespace OpenPSTD::Kernel::DG;

template JacobiGQResult<float> OpenPSTD::Kernel::DG::JacobiGQ(float alpha, float beta, int N, bool sort);
template JacobiGQResult<double> OpenPSTD::Kernel::DG::JacobiGQ(double alpha, double beta, int N, bool sort);

template VectorX<float> OpenPSTD::Kernel::DG::JacobiGL(float alpha, float beta, int N);
template VectorX<double> OpenPSTD::Kernel::DG::JacobiGL(double alpha, double beta, int N);

template VectorX<float> OpenPSTD::Kernel::DG::JacobiP(VectorX<float> x, float alpha, float beta, int N);
template VectorX<double> OpenPSTD::Kernel::DG::JacobiP(VectorX<double> x, double alpha, double beta, int N);

template VectorX<float> OpenPSTD::Kernel::DG::GradJacobiP(const VectorX<float> z, float alpha, float beta, int N);
template VectorX<double> OpenPSTD::Kernel::DG::GradJacobiP(const VectorX<double> z, double alpha, double beta, int N);

template struct JacobiGQResult<float>;
template struct JacobiGQResult<double>;
