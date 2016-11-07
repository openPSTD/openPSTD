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
#include "Coordinates.h"
#include "Simplex.h"

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
            MatrixX<SimpleType> Vandermonde1D(int N, VectorX<SimpleType> r)
            {
                MatrixX<SimpleType> result(r.size(), N+1);

                for (int j=1; j<=(N+1); ++j) {
                    int J = j-1;
                    result.col(J) = JacobiP<SimpleType>(r, 0, 0, j-1);
                }
                return result;
            }

            /**
             * Initialize the 1D gradient Vandermonde Matrix.
             */
            template <typename SimpleType>
            MatrixX<SimpleType> GradVandermonde1D(int N, VectorX<SimpleType> r)
            {
                MatrixX<SimpleType> result(r.size(), N+1);

                for (int j=1; j<=(N+1); ++j) {
                    int J = j-1;
                    result.col(J) = GradJacobiP<SimpleType>(r, 0, 0, j-1);
                }
                return result;
            }

            /**
             * Initialize the 2D Vandermonde Matrix,  V_{ij} = phi_j(r_i, s_i);
             * @param N
             * @param rs
             * @return
             */
            template <typename SimpleType>
            MatrixX<SimpleType> Vandermonde2D(int N, MatrixX<SimpleType> rs)
            {
                MatrixX<SimpleType> result(rs.rows(), (N+1)*(N+2)/2);

                // Transfer to (a,b) coordinates
                ArrayXX<SimpleType> ab = rstoab<SimpleType>(rs.array());

                int sk = 0;
                for(int i = 0; i <= N; i++)
                {
                    for (int j = 0; j <= N-i; ++j)
                    {
                        result.col(sk) = Simplex2DP(ab, i, j);
                        sk++;
                    }
                }
                return result;
            }

        }
    }
}

#endif //OPENPSTD_VANDERMONDE_H
