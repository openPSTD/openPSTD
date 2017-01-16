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
// Date: 3-11-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_NODES_H_H
#define OPENPSTD_NODES_H_H

#include "GeneralTypes.h"
#include "WarpFactor.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            /**
             * Compute (x,y) nodes in equilateral triangle for polynomial of order N.
             */
            template <typename SimpleType>
            ArrayXX<SimpleType> Nodes2D(int N)
            {
                ArrayX<SimpleType> alpopt(15);
                //original numbers from the matlab code
                alpopt << 0,0,1.4152,0.10009999999999999,0.27510000000000001,0.97999999999999998,1.0999000000000001,
                        1.2831999999999999,1.3648,1.4773000000000001,1.4959,1.5743,1.577,1.6223000000000001,
                        1.6257999999999999;

                //Set optimized parameter, alpha, depending on order N
                SimpleType alpha;
                if(N < 15)
                    alpha = alpopt[N-1];
                else
                    alpha = 5.0/3.0;

                //total number of nodes
                int Np = (N+1)*(N+2)/2;

                //Create equidistributed nodes on equilateral triangle
                ArrayX<SimpleType> L1(Np);
                ArrayX<SimpleType> L2(Np);
                ArrayX<SimpleType> L3(Np);

                int sk = 1;
                for(int n = 1; n <= N+1; n++)
                {
                    for(int m = 1; m <= N+2-n; m++)
                    {
                        L1[sk-1] = (n-1)/(SimpleType)N;
                        L3[sk-1] = (m-1)/(SimpleType)N;
                        sk++;
                    }
                }
                L2 = 1-L1-L3;
                ArrayX<SimpleType> x = -L2+L3;
                //Optimized with a constant: 1.7320508075688772 == sqrt(3.0)
                ArrayX<SimpleType> y = (-L2-L3+2*L1)/1.7320508075688772;

                //Compute blending function at each node for each edge
                ArrayX<SimpleType> blend1 = 4*L2*L3;
                ArrayX<SimpleType> blend2 = 4*L1*L3;
                ArrayX<SimpleType> blend3 = 4*L1*L2;

                //Amount of warp for each node, for each edge
                ArrayX<SimpleType> warpf1 = WarpFactor<SimpleType>(N, L3-L2);
                ArrayX<SimpleType> warpf2 = WarpFactor<SimpleType>(N, L1-L3);
                ArrayX<SimpleType> warpf3 = WarpFactor<SimpleType>(N, L2-L1);

                //Combine blend & warp
                ArrayX<SimpleType> warp1 = blend1*warpf1*(1+(alpha*L1).square());
                ArrayX<SimpleType> warp2 = blend2*warpf2*(1+(alpha*L2).square());
                ArrayX<SimpleType> warp3 = blend3*warpf3*(1+(alpha*L3).square());

                //Accumulate deformations associated with each edge
                //x = x + 1*warp1 + cos(2*pi/3)*warp2 + cos(4*pi/3)*warp3;
                //y = y + 0*warp1 + sin(2*pi/3)*warp2 + sin(4*pi/3)*warp3;

                //this is optimized with some constants:
                //cos(2*pi/3) == -0.49999999999999978
                //sin(2*pi/3) == 0.86602540378443871
                //cos(4*pi/3) == -0.50000000000000044
                //sin(4*pi/3) == -0.86602540378443849
                x = x + 1*warp1 + -0.49999999999999978*warp2 + -0.50000000000000044*warp3;
                y = y + 0*warp1 + 0.86602540378443871*warp2 + -0.86602540378443849*warp3;
                ArrayXX<SimpleType> result(Np, 2);
                result << x, y;
                return result;
            }

            extern template ArrayXX<float> Nodes2D(int N);
            extern template ArrayXX<double> Nodes2D(int N);

        }
    }
}

#endif //OPENPSTD_NODES_H_H
