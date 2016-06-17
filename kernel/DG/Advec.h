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
// Date: 7-6-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_ADVEC_H
#define OPENPSTD_ADVEC_H

#include <Eigen/Dense>

#include "DG.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            template <typename SimpleType>
            class AdvecDE: public DG1DDE<SimpleType>
            {
            private:
                SimpleType CalculateFlux(std::shared_ptr<Element1D<SimpleType, NoElementStore>> element, FaceIndex1D f, SimpleType a,
                                         SimpleType alpha, SimpleType uin)
                {
                    int I = f==FaceIndex1D::Left?0:element->x.size()-1;
                    SimpleType Normal = element->Faces[(int)f]->Normal[0];

                    SimpleType result = (element->u[0][I]-uin)*(a*Normal-(1-alpha)*abs(a*Normal))/2;

                    return result;
                }

                SimpleType CalculateFlux(std::shared_ptr<Element1D<SimpleType, NoElementStore>> element, FaceIndex1D f, SimpleType a,
                                         SimpleType alpha)
                {
                    auto face = element->Faces[f==FaceIndex1D::Left?0:1];
                    auto otherFace = face->GetOtherSideFace();
                    std::shared_ptr<Element1D<SimpleType, NoElementStore>> other = otherFace->Element.lock();
                    int OtherSideIndex = f==FaceIndex1D::Right?0:other->u[0].size()-1;
                    SimpleType uin = other->u[0][OtherSideIndex];
                    SimpleType result = CalculateFlux(element, f, a, alpha, uin);
                    return result;
                }

                SimpleType CalculateInput(std::shared_ptr<Element1D<SimpleType, NoElementStore>> element, SimpleType a,
                                          SimpleType alpha, SimpleType time)
                {
                    int I = 0;
                    SimpleType uin = -sin(a*time);
                    return CalculateFlux(element, FaceIndex1D::Left, a, alpha, uin);
                }

                SimpleType CalculateOutput()
                {
                    return 0;
                }

            public:
                SimpleType Alpha = 1;
                SimpleType a = M_PI*2;

                virtual void Initialize(
                        std::shared_ptr<Element1D<SimpleType, NoElementStore>> element)
                {
                    element->u[0] = element->x.array().sin();
                }
                virtual std::vector<VectorX<SimpleType>> CalculateRHS(
                        std::shared_ptr<Element1D<SimpleType, NoElementStore>> element,
                        std::shared_ptr<System1D<SimpleType, NoElementStore>> system,
                        SimpleType time)
                {
                    VectorX<SimpleType> flux(2);

                    if (element->Faces[0]->OtherSideExist())
                        flux[0] = CalculateFlux(element, FaceIndex1D::Left, a, Alpha);
                    else
                        flux[0] = CalculateInput(element, a, Alpha, time);

                    if (element->Faces[1]->OtherSideExist())
                        flux[1] = CalculateFlux(element, FaceIndex1D::Right, a, Alpha);
                    else
                        flux[1] = CalculateOutput();

                    VectorX<SimpleType> RHS = -a * (element->rx.cwiseProduct(system->Dr * element->u[0])) +
                                              system->LIFT * (element->Fscale.cwiseProduct(flux));

                    std::vector<VectorX<SimpleType>> result;
                    result.push_back(RHS);
                    return result;
                }

                virtual SimpleType GetMaxDt(SimpleType minDistance, int N)
                {
                    SimpleType CFL = 0.75;
                    SimpleType dt = CFL/a*minDistance;
                    dt = dt/2;
                    return dt;
                }

                virtual unsigned int GetNumberOfVariables()
                {
                    return 1;
                }
            };
        }
    }
}

#endif //OPENPSTD_ADVEC_H
