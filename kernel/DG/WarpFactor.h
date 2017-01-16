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
// Date: 22-6-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_WARPFACTOR_H
#define OPENPSTD_WARPFACTOR_H

#include <Eigen/Dense>
#include "GeneralTypes.h"
#include "Vandermonde.h"
#include "Jacobi.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            /**
             * Compute scaled warp function at order N based on rout interpolation nodes.
             * @param N: Order
             */
            template<typename SimpleType>
            ArrayX<SimpleType> WarpFactor(int N, VectorX<SimpleType> rout)
            {
                // Compute LGL and equidistant node distribution
                //LGLr = JacobiGL(0,0,N); req  = linspace(-1,1,N+1)';
                VectorX<SimpleType> LGLr = JacobiGL<SimpleType>(0,0,N);
                VectorX<SimpleType> req = VectorX<SimpleType>::LinSpaced(N+1, -1, 1);

                // Compute V based on req
                //Veq = Vandermonde1D(N,req);
                MatrixX<SimpleType> Veq = Vandermonde1D<SimpleType>(N,req);

                // Evaluate Lagrange polynomial at rout
                //Nr = length(rout); Pmat = zeros(N+1,Nr);
                int Nr = rout.size();
                MatrixX<SimpleType> Pmat = MatrixX<SimpleType>::Zero(N+1, Nr);
                for(int i = 0; i < N+1; i++)
                {
                    //Pmat(i,:) = JacobiP(rout, 0, 0, i-1)';
                    //i is already in the loop changed
                    Pmat.row(i) = JacobiP<SimpleType>(rout, 0, 0, i);
                }

                //Lmat = Veq'\Pmat;
                Veq.transposeInPlace();
                //Veq isn't used anymore
                MatrixX<SimpleType> Lmat = Veq.fullPivLu().solve(Pmat);

                // Compute warp factor
                //warp = Lmat'*(LGLr - req);
                Lmat.transposeInPlace();
                //Lmat isn't used anymore
                VectorX<SimpleType> warp = Lmat*(LGLr - req);

                // Scale factor
                //zerof = (abs(rout)<1.0-1.0e-10);
                ArrayX<SimpleType> zerof = rout.unaryExpr([](SimpleType x){ return (fabs(x)<1?1:0)-1.0e-10; });
                //sf = 1.0 - (zerof.*rout).^2;
                ArrayX<SimpleType> sf = 1.0 - (zerof*rout.array()).square();
                //warp = warp./sf + warp.*(zerof-1);
                warp = warp.array()/sf + warp.array()*(zerof-1);

                return warp;
            }

            extern template ArrayX<float> WarpFactor(int N, VectorX<float> rout);
            extern template ArrayX<double> WarpFactor(int N, VectorX<double> rout);

        }
    }
}

#endif //OPENPSTD_WARPFACTOR_H
