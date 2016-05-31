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
// Date: 30-5-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_JACOBI_H
#define OPENPSTD_JACOBI_H

#include <Eigen/Dense>

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            template <typename SimpleType>
            using VectorX = Eigen::Matrix<SimpleType, Eigen::Dynamic, 1>;

            template <typename SimpleType>
            using MatrixX = Eigen::Matrix<SimpleType, Eigen::Dynamic, Eigen::Dynamic>;

            template <typename SimpleType>
            struct JacobiGQResult
            {
            public:
                VectorX<SimpleType> X;
                VectorX<SimpleType> W;
            };



            //template <typename SimpleType>
            //JacobiGQResult<SimpleType> JacobiGQ(double alpha, double beta, int N, bool sort);

            JacobiGQResult<double> JacobiGQ(double alpha, double beta, int N, bool sort);

            //VectorXd& JacobiP(const VectorXd& x, double alpha, double beta, int N);


            inline double SQ(double x) { return x*x; }
        }
    }

}


#endif //OPENPSTD_JACOBI_H
