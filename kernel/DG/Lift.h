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
// Date: 1-6-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_LIFT_H
#define OPENPSTD_LIFT_H

#include <Eigen/Dense>
#include "GeneralTypes.h"
#include "Vandermonde.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            /**
             * Compute surface integral term in DG formulation.
             * @param Np: Number of points in an element
             * @param NFaces: Number of faces of an element
             * @param V: Vandermonde matrix
             */
            template <typename SimpleType>
            MatrixX<SimpleType> Lift1D(int Np, int NFaces, MatrixX<SimpleType> V)
            {
                MatrixX<SimpleType> Emat = MatrixX<SimpleType>::Zero(Np, NFaces);
                Emat(0, 0) = 1;
                Emat(Np-1, 1) = 1;
                return V*(V.transpose()*Emat);
            }

            /**
             * Compute surface to volume lift term for DG formulation
             * @param N Order of calculations
             * @param NFaces Number of faces, (should always be 3
             * @param V The 2D vandermonde matrix
             * @param rs The positions in rs space (identy
             * @return the surface to volume lift term
             */
            template <typename SimpleType>
            MatrixX<SimpleType> Lift2D(int N, int NFaces, MatrixX<SimpleType> V, MatrixX<SimpleType> rs)
            {
                int Np = (N+1)*(N+2)/2;
                int Nfp = N+1;

                MatrixX<SimpleType> Emat = MatrixX<SimpleType>::Zero(Np, NFaces*Nfp);
                MatrixX<SimpleType> V1D;
                int j;

                //% face 1
                //fmask1   = find( abs(s+1) < NODETOL)';
                //faceR = r(Fmask(:,1));
                j = 0;
                VectorX<SimpleType> faceR(Nfp);
                for(int i = 0; i < rs.rows(); i++)
                {
                    if(std::abs(rs(i, 1)+1) < NODETOL)
                    {
                        faceR(j) = rs(i, 0);
                        j++;
                    }
                }
                //V1D = Vandermonde1D(N, faceR);
                V1D = Vandermonde1D(N, faceR);
                //massEdge1 = inv(V1D*V1D');
                MatrixX<SimpleType> massEdge1 = (V1D*V1D.transpose()).fullPivLu().inverse();
                //Emat(Fmask(:,1),1:Nfp) = massEdge1;
                j = 0;
                for(int i = 0; i < rs.rows(); i++)
                {
                    if(std::abs(rs(i, 1)+1) < NODETOL)
                    {
                        Emat.block(i, 0*Nfp, 1, Nfp) = massEdge1.row(j);
                        j++;
                    }
                }


                //% face 2
                //fmask2   = find( abs(r+s) < NODETOL)';
                //faceR = r(Fmask(:,2));
                j = 0;
                VectorX<SimpleType> faceR2(Nfp);
                for(int i = 0; i < rs.rows(); i++)
                {
                    if(std::abs(rs(i, 0)+rs(i, 1)) < NODETOL)
                    {
                        faceR2(j) = rs(i, 0);
                        j++;
                    }
                }
                //V1D = Vandermonde1D(N, faceR);
                V1D = Vandermonde1D(N, faceR2);
                //massEdge2 = inv(V1D*V1D');
                MatrixX<SimpleType> massEdge2 = (V1D*V1D.transpose()).fullPivLu().inverse();
                //Emat(Fmask(:,2),Nfp+1:2*Nfp) = massEdge2;
                j = 0;
                for(int i = 0; i < rs.rows(); i++)
                {
                    if(std::abs(rs(i, 0)+rs(i, 1)) < NODETOL)
                    {
                        Emat.block(i, 1*Nfp, 1, Nfp) = massEdge1.row(j);
                        j++;
                    }
                }

                //% face 3
                //fmask3   = find( abs(r+1) < NODETOL)';
                //faceS = s(Fmask(:,3));
                j = 0;
                VectorX<SimpleType> faceS(Nfp);
                for(int i = 0; i < rs.rows(); i++)
                {
                    if(std::abs(rs(i, 0)+1) < NODETOL)
                    {
                        faceS(j) = rs(i, 1);
                        j++;
                    }
                }
                //V1D = Vandermonde1D(N, faceS);
                V1D = Vandermonde1D(N, faceS);
                //massEdge3 = inv(V1D*V1D');
                MatrixX<SimpleType> massEdge3 = (V1D*V1D.transpose()).fullPivLu().inverse();
                //Emat(Fmask(:,3),2*Nfp+1:3*Nfp) = massEdge3;
                j = 0;
                for(int i = 0; i < rs.rows(); i++)
                {
                    if(std::abs(rs(i, 0)+1) < NODETOL)
                    {
                        Emat.block(i, 2*Nfp, 1, Nfp) = massEdge1.row(j);
                        j++;
                    }
                }

                //% inv(mass matrix)*\I_n (L_i,L_j)_{edge_n}
                //LIFT = V*(V'*Emat);
                return V*(V.transpose()*Emat);
            }
        }
    }
}

#endif //OPENPSTD_LIFT_H
