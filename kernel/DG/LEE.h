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
// Date: 15-6-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_LEE_H
#define OPENPSTD_LEE_H

#include <Eigen/Dense>

#include "DG.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            template <typename SimpleType>
            class LinearizedEulerEquationsDE: public DG1DDE<SimpleType>
            {
            private:
                MatrixX<SimpleType> CFL_Values;
                SimpleType a1_mat;
                SimpleType a2_mat;
                SimpleType Zimp;
                SimpleType Yimp;

                const int IndexP=0;
                const int IndexU=1;

                const int left=0;
                const int right=1;

                SimpleType GetPlus(int variable, int side, std::shared_ptr<Element1D<SimpleType>> element)
                {
                    if(element->Faces[side]->OtherSideExist())
                    {
                        std::shared_ptr<Face1D<SimpleType>> other = element->Faces[side]->GetOtherSideFace();
                        auto otherElement = other->Element.lock();

                        int index;
                        if(side==left)
                            index = otherElement->x.size()-1;
                        else
                            index = 0;

                        return otherElement->u[variable][index];
                    }
                    else
                    {
                        int index;
                        if(side==left)
                            index = 0;
                        else
                            index = element->x.size()-1;

                        return element->u[variable][index];
                    }
                }

                SimpleType GetMin(int variable, int side, std::shared_ptr<Element1D<SimpleType>> element)
                {
                    int index;
                    if(side==left)
                        index = 0;
                    else
                        index = element->x.size()-1;

                    return element->u[variable][index];
                }

            public:
                SimpleType rec_cycles;
                SimpleType x_exc_point;
                SimpleType n_gau;
                SimpleType sigmax_gau;
                SimpleType ax_gau;
                SimpleType Amp_gau;
                SimpleType dx_PSTD;
                SimpleType dx_Factor;
                SimpleType rho;
                int Precision;

                //speed of sound
                SimpleType c;

                LinearizedEulerEquationsDE():
                        CFL_Values(5, 10),

                        rec_cycles(100000),
                        x_exc_point(-1),
                        n_gau(2.5),
                        Amp_gau(1),
                        dx_PSTD(0.034300000000000),
                        c(343),
                        rho(1.21),
                        dx_Factor(1),
                        Precision(1)
                {
                    CFL_Values <<
                            1.190000000000000	,	0.619000000000000	,	0.394000000000000	,	0.276000000000000	,	0.206000000000000	,	0.160000000000000	,	0.127000000000000	,	0.104000000000000	,	0.087300000000000	,	0.074200000000000	,
                            1.190000000000000	,	0.619000000000000	,	0.394000000000000	,	0.276000000000000	,	0.206000000000000	,	0.160000000000000	,	0.127000000000000	,	0.104000000000000	,	0.087300000000000	,	0.074200000000000	,
                            1.190000000000000	,	0.619000000000000	,	0.394000000000000	,	0.276000000000000	,	0.206000000000000	,	0.160000000000000	,	0.127000000000000	,	0.104000000000000	,	0.087300000000000	,	0.074200000000000	,
                            1.190000000000000	,	0.619000000000000	,	0.394000000000000	,	0.276000000000000	,	0.206000000000000	,	0.160000000000000	,	0.127000000000000	,	0.104000000000000	,	0.087300000000000	,	0.074200000000000	,
                            1.190000000000000	,	0.619000000000000	,	0.394000000000000	,	0.266000000000000	,	0.177000000000000	,	0.130000000000000	,	0.101000000000000	,	0.082200000000000	,	0.069200000000000	,	0.059300000000000	;

                    sigmax_gau = n_gau*dx_PSTD;
                    ax_gau = 1/(2*(sigmax_gau*sigmax_gau));

                    Zimp = rho*c;
                    Yimp = 1/Zimp;

                    a1_mat = rho*c*c;
                    a2_mat = 1/rho;
                }

                virtual void Initialize(std::shared_ptr<Element1D<SimpleType>> element)
                {
                    //p_DG = Amp_gau_DG.*exp(-ax_gau_DG*((x_DG-x_exc_point_DG).^2));
                    element->u[IndexP] = element->x.unaryExpr([&](SimpleType x){return Amp_gau*exp(-ax_gau*pow(x-x_exc_point,2));});
                    //u_DG = -p_DG./(rho*c);
                    element->u[IndexU] = -element->u[IndexP].array()/(rho*c);
                }

                virtual std::vector<VectorX<SimpleType>> CalculateRHS(
                        std::shared_ptr<Element1D<SimpleType>> element,
                        std::shared_ptr<System1D<SimpleType>> system,
                        SimpleType time)
                {
                    VectorX<SimpleType> P = element->u[IndexP];
                    VectorX<SimpleType> U = element->u[IndexU];

                    ArrayX<SimpleType> dp(2);
                    ArrayX<SimpleType> du(2);
                    ArrayX<SimpleType> Nx(2);
                    VectorX<SimpleType> fluxP(2);
                    VectorX<SimpleType> fluxU(2);

                    SimpleType pbc, ubc;

                    //get normal vector
                    Nx << element->Faces[0]->Normal, element->Faces[1]->Normal;

                    //Define field differences at faces
                    //dp(:) = p_DG(vmapM)-p_DG(vmapP);
                    dp <<
                            this->GetMin(IndexP, left, element)-this->GetPlus(IndexP, left, element),
                            this->GetMin(IndexP, right, element)-this->GetPlus(IndexP, right, element);
                    //du(:) = u_DG(vmapM)-u_DG(vmapP);
                    du <<
                            this->GetMin(IndexU, left, element)-this->GetPlus(IndexU, left, element),
                            this->GetMin(IndexU, right, element)-this->GetPlus(IndexU, right, element);

                    //Homogeneous boundary conditions, u=0 (to change BC change sign of ubc and pbc)
                    //pbc = p_DG(vmapB); dp (mapB) = p_DG(vmapB) - pbc;
                    if(!element->Faces[0]->OtherSideExist())
                    {
                        pbc = P[0];
                        dp[0] = P[0] - pbc;
                    }
                    if(!element->Faces[1]->OtherSideExist())
                    {
                        int i = element->x.size()-1;
                        pbc = P[i];
                        dp[1] = P[i] - pbc;
                    }

                    //ubc =  -u_DG(vmapB);du (mapB) = u_DG(vmapB) - ubc;
                    if(!element->Faces[0]->OtherSideExist())
                    {
                        ubc = -U[0];
                        du[0] = U[0] - ubc;
                    }
                    if(!element->Faces[1]->OtherSideExist())
                    {
                        int i = element->x.size()-1;
                        ubc = -U[i];
                        du[1] = U[i] - ubc;
                    }

                    //evaluate upwind fluxes
                    //fluxp = 1./(Zimpm + Zimpp).*(nx.*Zimpp.*du - dp);
                    fluxP = 1/(Zimp+Zimp) * (Nx * Zimp * du - dp);
                    //fluxu = 1./(Yimpm + Yimpp).*(nx.*Yimpp.*dp - du);
                    fluxU = 1/(Yimp+Yimp) * (Nx * Yimp * dp - du);

                    //compute right hand sides of the PDE's
                    //rhsp = (-rx.*(Dr*u_DG) + LIFT*(Fscale.*fluxp)).*a1_mat_DG;
                    VectorX<SimpleType> RHS_P =
                            ((-element->rx).cwiseProduct(system->Dr * U) +
                             system->LIFT * (element->Fscale.cwiseProduct(fluxP))).array() * a1_mat;
                    //rhsu = (-rx.*(Dr*p_DG) + LIFT*(Fscale.*fluxu)).*a2_mat_DG;
                    VectorX<SimpleType> RHS_U =
                            ((-element->rx).cwiseProduct(system->Dr * P) +
                             system->LIFT * (element->Fscale.cwiseProduct(fluxU))).array() * a2_mat;

                    std::vector<VectorX<SimpleType>> result(2);
                    result[IndexP] = RHS_P;
                    result[IndexU] = RHS_U;
                    return result;
                }

                virtual SimpleType GetMaxDt(SimpleType minDistance, int N)
                {
                    //h_DG = dx_factor*dx_PSTD;
                    //CFL_DG = CFL_DG_values(Precision,N);
                    //dt_DG = CFL_DG*h_DG/c;

                    SimpleType h = dx_Factor*dx_PSTD;
                    SimpleType CFL = CFL_Values(Precision-1, N-1);
                    SimpleType dt = CFL*h/c;
                    return dt;
                }

                virtual unsigned int GetNumberOfVariables()
                {
                    return 2;
                }
            };
        }
    }
}

#endif //OPENPSTD_LEE_H
