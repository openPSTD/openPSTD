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
// Date: 10-6-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_MAXWELL_H
#define OPENPSTD_MAXWELL_H

#include <Eigen/Dense>

#include "DG.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            template<typename SimpleType>
            class MaxwellDEElement
            {
            public:
                VectorX<SimpleType> Eps;
                VectorX<SimpleType> Mu;
                VectorX<SimpleType> Zimp;
            };


            template <typename SimpleType>
            class MaxwellDE: public DG1DDE<SimpleType, MaxwellDEElement<SimpleType>>
            {
            private:
                const int IndexE=0;
                const int IndexH=1;
                const int IndexZimp=2;

                const int left=0;
                const int right=1;

                SimpleType GetPlus(int variable, int side, std::shared_ptr<Element1D<SimpleType, MaxwellDEElement<SimpleType>>> element)
                {
                    if(element->Faces[side]->OtherSideExist())
                    {
                        std::shared_ptr<Face1D<SimpleType, MaxwellDEElement<SimpleType>>> other = element->Faces[side]->GetOtherSideFace();
                        auto otherElement = other->Element.lock();

                        int index;
                        if(side==left)
                            index = otherElement->x.size()-1;
                        else
                            index = 0;

                        if(variable == IndexZimp)
                            return otherElement->DEStore.Zimp[index];
                        else
                            return otherElement->u[variable][index];
                    }
                    else
                    {
                        int index;
                        if(side==left)
                            index = 0;
                        else
                            index = element->x.size()-1;

                        if(variable == IndexZimp)
                            return element->DEStore.Zimp[index];
                        else
                            return element->u[variable][index];
                    }
                }

                SimpleType GetMin(int variable, int side, std::shared_ptr<Element1D<SimpleType, MaxwellDEElement<SimpleType>>> element)
                {
                    int index;
                    if(side==left)
                        index = 0;
                    else
                        index = element->x.size()-1;

                    if(variable == IndexZimp)
                        return element->DEStore.Zimp[index];
                    else
                        return element->u[variable][index];
                }

            public:
                virtual void Initialize(std::shared_ptr<Element1D<SimpleType, MaxwellDEElement<SimpleType>>> element)
                {
                    //E = sin(pi*x).*(x<0); H = zeros(Np,K);
                    element->u[IndexE] = element->x.unaryExpr([](SimpleType a){return a<0?sin(M_PI*a):0;});
                    element->u[IndexH] = VectorX<SimpleType>::Zero(element->x.size());

                    //mu1 = ones(1,K);
                    element->DEStore.Mu = VectorX<SimpleType>::Ones(element->x.size());
                    //eps1 = [ones(1,K/2), 2*ones(1,K/2)];
                    element->DEStore.Eps = VectorX<SimpleType>::Ones(element->x.size());
                    if(element->x[0] >= 0)
                        element->DEStore.Eps = 2*element->DEStore.Eps;

                    //Compute impedance
                    //Zimp = sqrt(mu./eps);
                    element->DEStore.Zimp = (element->DEStore.Mu.array()/element->DEStore.Eps.array()).sqrt();
                }

                virtual std::vector<VectorX<SimpleType>> CalculateRHS(
                        std::shared_ptr<Element1D<SimpleType, MaxwellDEElement<SimpleType>>> element,
                        std::shared_ptr<System1D<SimpleType, MaxwellDEElement<SimpleType>>> system,
                        SimpleType time)
                {
                    VectorX<SimpleType> E = element->u[IndexE];
                    VectorX<SimpleType> H = element->u[IndexH];

                    VectorX<SimpleType> dE(2);
                    VectorX<SimpleType> dH(2);
                    VectorX<SimpleType> Zimpm(2);
                    VectorX<SimpleType> Zimpp(2);
                    VectorX<SimpleType> Yimpm(2);
                    VectorX<SimpleType> Yimpp(2);
                    VectorX<SimpleType> Nx(2);
                    VectorX<SimpleType> fluxE(2);
                    VectorX<SimpleType> fluxH(2);

                    Nx << element->Faces[0]->Normal, element->Faces[1]->Normal;

                    //dE(:) = E(vmapM)-E(vmapP); dH(:) = H(vmapM)-H(vmapP);
                    dE <<
                       this->GetMin(IndexE, left, element)-this->GetPlus(IndexE, left, element),
                       this->GetMin(IndexE, right, element)-this->GetPlus(IndexE, right, element);
                    dH <<
                       this->GetMin(IndexH, left, element)-this->GetPlus(IndexH, left, element),
                       this->GetMin(IndexH, right, element)-this->GetPlus(IndexH, right, element);

                    //Zimpm(:) = Zimp(vmapM); Zimpp(:) = Zimp(vmapP);
                    Zimpm << this->GetMin(IndexZimp, left, element), this->GetMin(IndexZimp, right, element);
                    Zimpp << this->GetPlus(IndexZimp, left, element), this->GetPlus(IndexZimp, right, element);
                    //Yimpm(:) = 1./Zimpm(:); Yimpp(:) = 1./Zimpp(:);
                    Yimpm = VectorX<double>::Ones(Zimpm.size()).cwiseQuotient(Zimpm);
                    Yimpp = VectorX<double>::Ones(Zimpp.size()).cwiseQuotient(Zimpp);

                    //Ebc = -E(vmapB); dE (mapB) = E(vmapB) - Ebc;
                    SimpleType Ebc, Hbc;
                    if(!element->Faces[0]->OtherSideExist())
                    {
                        Ebc = -E[0];
                        dE[0] = E[0] - Ebc;
                    }
                    if(!element->Faces[1]->OtherSideExist())
                    {
                        int i = element->x.size()-1;
                        Ebc = -E[i];
                        dE[1] = E[i] - Ebc;
                    }

                    //Hbc =  H(vmapB); dH (mapB) = H(vmapB) - Hbc;
                    if(!element->Faces[0]->OtherSideExist())
                    {
                        Hbc = H[0];
                        dH[0] = H[0] - Hbc;
                    }
                    if(!element->Faces[1]->OtherSideExist())
                    {
                        int i = element->x.size()-1;
                        Hbc = H[i];
                        dH[1] = H[i] - Hbc;
                    }

                    //fluxE = 1./(Zimpm + Zimpp).*(nx.*Zimpp.*dH - dE);
                    fluxE = 1/(Zimpm+Zimpp).array() * (Nx.array() * Zimpp.array() * dH.array() - dE.array());
                    //fluxH = 1./(Yimpm + Yimpp).*(nx.*Yimpp.*dE - dH);
                    fluxH = 1/(Yimpm+Yimpp).array() * (Nx.array() * Yimpp.array() * dE.array() - dH.array());

                    //rhsE = (-rx.*(Dr*H) + LIFT*(Fscale.*fluxE))./eps;
                    VectorX<SimpleType> RHS_E =
                            ((-element->rx).cwiseProduct(system->Dr * element->u[IndexH]) +
                             system->LIFT * (element->Fscale.cwiseProduct(fluxE))).array() / element->DEStore.Eps.array();
                    //rhsH = (-rx.*(Dr*E) + LIFT*(Fscale.*fluxH))./mu;
                    VectorX<SimpleType> RHS_H =
                            ((-element->rx).cwiseProduct(system->Dr * element->u[IndexE]) +
                             system->LIFT * (element->Fscale.cwiseProduct(fluxH))).array() / element->DEStore.Mu.array();

                    std::vector<VectorX<SimpleType>> result(2);
                    result[IndexE] = RHS_E;
                    result[IndexH] = RHS_H;
                    return result;
                }

                virtual SimpleType GetMaxDt(SimpleType minDistance, int N)
                {
                    SimpleType CFL = 1.0;
                    SimpleType dt = CFL*minDistance;
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

#endif //OPENPSTD_MAXWELL_H
