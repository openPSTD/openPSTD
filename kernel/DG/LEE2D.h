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
// Date: 18-11-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_LEE2D_H
#define OPENPSTD_LEE2D_H

#include <Eigen/Dense>
#include "DG2D.h"
#include "Grad.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            template<typename SimpleType>
            class LinearizedEuler2DDEElement
            {
            public:
                bool Reflection;
            };
            template <typename SimpleType>
            ArrayX<SimpleType> isNaN(ArrayX<SimpleType> a)
            {
                ArrayX<SimpleType> result(a.size());
                for (int i = 0; i < a.size(); ++i)
                {
                    if(std::isnan(a(i)))
                        result(i) = 1;
                    else
                        result(i) = 0;
                }
                return result;
            }

            template <typename SimpleType>
            class LinearizedEulerEquationsDE2D: public DG2DDE<SimpleType, LinearizedEuler2DDEElement<SimpleType>>
            {
            private:
                MatrixX<SimpleType> CFL_Values;
                SimpleType a1_mat;
                SimpleType a2_mat;
                SimpleType Zimp;
                SimpleType Yimp;

                const int IndexPX=0;
                const int IndexPY=1;
                const int IndexVX=2;
                const int IndexVY=3;

                VectorX<SimpleType> atan2(VectorX<SimpleType> x, VectorX<SimpleType> y)
                {
                    VectorX<SimpleType> result(x.size());
                    for (int i = 0; i < x.size(); ++i)
                    {
                        result(i) = std::atan2(x(i), y(i));
                    }
                    return result;
                }
            public:
                SimpleType rho;
                SimpleType alpha;
                //% Toulorge thesis. Error magnitude: (1) Emag=1dB; (2) Emag=0.1dB; (3) Emag=0.01dB; (4) Emag=0.001dB; (5) Emag=0.0001dB;
                int Precision;

                //speed of sound
                SimpleType c;

                LinearizedEulerEquationsDE2D(SimpleType c = 343, SimpleType rho = 1.21, int precision = 1) :
                        CFL_Values(5, 10),
                        c(c),
                        rho(rho),
                        Precision(precision),
                        alpha(0)
                {
                    //given in toulorge thesis for optimized RK
                    CFL_Values <<
                               +1.1899999999999999e+00, +6.1899999999999999e-01, +3.9400000000000002e-01, +2.7600000000000002e-01, +2.0599999999999999e-01, +1.6000000000000000e-01, +1.2700000000000000e-01, +1.0400000000000000e-01, +8.7300000000000003e-02, +7.4200000000000002e-02,
                            +1.1899999999999999e+00, +6.1899999999999999e-01, +3.9400000000000002e-01, +2.7600000000000002e-01, +2.0599999999999999e-01, +1.6000000000000000e-01, +1.2700000000000000e-01, +1.0400000000000000e-01, +8.7300000000000003e-02, +7.4200000000000002e-02,
                            +1.1899999999999999e+00, +6.1899999999999999e-01, +3.9400000000000002e-01, +2.7600000000000002e-01, +2.0599999999999999e-01, +1.6000000000000000e-01, +1.2700000000000000e-01, +1.0400000000000000e-01, +8.7300000000000003e-02, +7.4200000000000002e-02,
                            +1.1899999999999999e+00, +6.1899999999999999e-01, +3.9400000000000002e-01, +2.7600000000000002e-01, +2.0599999999999999e-01, +1.6000000000000000e-01, +1.2700000000000000e-01, +1.0400000000000000e-01, +8.7300000000000003e-02, +7.4200000000000002e-02,
                            +1.1899999999999999e+00, +6.1899999999999999e-01, +3.9400000000000002e-01, +2.6600000000000001e-01, +1.7699999999999999e-01, +1.3000000000000000e-01, +1.0100000000000001e-01, +8.2199999999999995e-02, +6.9199999999999998e-02, +5.9299999999999999e-02;

                    a1_mat = rho * c * c;
                    a2_mat = 1 / rho;
                    Zimp = rho * c;
                    Yimp = 1 / Zimp;
                }

                virtual void Initialize(std::shared_ptr<Element2D<SimpleType, LinearizedEuler2DDEElement<SimpleType>>> element,
                                        std::shared_ptr<System2D<SimpleType, LinearizedEuler2DDEElement<SimpleType>>> system)
                {
                    //todo fix all of these variables
                    SimpleType freq_spatial = c/0.0025;
                    SimpleType AMP = 1;
                    SimpleType b_gau_x = 0;
                    SimpleType b_gau_y = 0.5;
                    SimpleType bx_gau = (0.1e-8)*(freq_spatial*freq_spatial);

                    VectorX<SimpleType> x = element->x;
                    VectorX<SimpleType> y = element->y;

                    //p_DG = AMP*exp(-bx_gau*((x-b_gau_x).^2 + (y-b_gau_y).^2));
                    ArrayX<SimpleType> p = AMP*exp((-bx_gau*((x.array()-b_gau_x).square() + (y.array()-b_gau_y).square())));

                    //[THETA,R] = cart2pol(x-b_gau_x,y-b_gau_y);
                    ArrayX<SimpleType> THETA = atan2(x, y);
                    //R is not used so calculation is useless
                    //px_DG = cos(THETA).^2.*p_DG;
                    element->u[IndexPX] = cos(THETA).square()*p;
                    //py_DG = p_DG-px_DG;
                    element->u[IndexPY] = p-element->u[IndexPX].array();
                    //vx_DG = zeros(Np, K);
                    element->u[IndexVX] = VectorX<SimpleType>::Zero(system->GetNp());
                    //vy_DG = zeros(Np, K);
                    element->u[IndexVY] = VectorX<SimpleType>::Zero(system->GetNp());
                };

                virtual std::vector<VectorX<SimpleType>> CalculateRHS(
                        std::shared_ptr<Element2D<SimpleType, LinearizedEuler2DDEElement<SimpleType>>> element,
                        std::shared_ptr<System2D<SimpleType, LinearizedEuler2DDEElement<SimpleType>>> system,
                        SimpleType time)
                {
                    // Getting the fields
                    VectorX<SimpleType> px = element->u[IndexPX];
                    VectorX<SimpleType> py = element->u[IndexPY];
                    VectorX<SimpleType> vx = element->u[IndexVX];
                    VectorX<SimpleType> vy = element->u[IndexVY];
                    VectorX<SimpleType> p = px+py;
                    ArrayX<SimpleType> nx = element->nx;
                    ArrayX<SimpleType> ny = element->ny;

                    // Define field differences at faces
                    ArrayX<SimpleType> dpx = element->GetFaceInternalValues(IndexPX) - element->GetFaceExternalValues(IndexPX);
                    ArrayX<SimpleType> dpy = element->GetFaceInternalValues(IndexPY) - element->GetFaceExternalValues(IndexPY);
                    ArrayX<SimpleType> dvx = element->GetFaceInternalValues(IndexVX) - element->GetFaceExternalValues(IndexVX);
                    ArrayX<SimpleType> dvy = element->GetFaceInternalValues(IndexVY) - element->GetFaceExternalValues(IndexVY);


                    // Impose REFLECTIVE boundary conditions
                    if(element->DEStore.Reflection)
                    {
                        //reflection
                        dpx = isNaN(dpx).select(0, dpx);
                        dpy = isNaN(dpy).select(0, dpx);
                        dvx = isNaN(dvx).select(2*element->GetFaceInternalValues(IndexVX), dpx);
                        dvy = isNaN(dvy).select(2*element->GetFaceInternalValues(IndexVY), dpx);
                    }
                    else
                    {
                        //absorption
                        dpx = isNaN(dpx).select(element->GetFaceInternalValues(IndexPX), dpx);
                        dpy = isNaN(dpy).select(element->GetFaceInternalValues(IndexPY), dpx);
                        dvx = isNaN(dvx).select(element->GetFaceInternalValues(IndexVX), dpx);
                        dvy = isNaN(dvy).select(element->GetFaceInternalValues(IndexVY), dpx);
                    }

                    // evaluate upwind fluxes
                    ArrayX<SimpleType> ndotdv =  nx*dvx+ny*dvy;
                    ArrayX<SimpleType> fluxpx = 1/(Zimp + Zimp)*(nx*Zimp*dvx - alpha*(dpx));
                    ArrayX<SimpleType> fluxpy = 1/(Zimp + Zimp)*(ny*Zimp*dvy - alpha*(dpy));
                    ArrayX<SimpleType> fluxvx = 1/(Yimp + Yimp)*(nx*Yimp*(dpx+dpy) + alpha*(ndotdv*nx-dvx));
                    ArrayX<SimpleType> fluxvy = 1/(Yimp + Yimp)*(ny*Yimp*(dpx+dpy) + alpha*(ndotdv*ny-dvy));

                    // local derivatives of fields
                    auto dp_DG = Grad2D(system->Dr, system->Ds, element->rx, element->ry, element->sx, element->sy, p);
                    VectorX<SimpleType> dpx_DG = dp_DG.ux;
                    VectorX<SimpleType> dpy_DG = dp_DG.uy;
                    VectorX<SimpleType> dvx_DG = element->rx.array()*(system->Dr*(vx)).array() + element->sx.array()*(system->Ds*(vx)).array();
                    VectorX<SimpleType> dvy_DG = element->ry.array()*(system->Dr*(vy)).array() + element->sy.array()*(system->Ds*(vy)).array();

                    // compute right hand sides of the PDE's
                    VectorX<SimpleType> rhspx = (-dvx_DG + system->LIFT*(element->Fscale.array()*fluxpx).matrix())*a1_mat;
                    VectorX<SimpleType> rhspy = (-dvy_DG + system->LIFT*(element->Fscale.array()*fluxpy).matrix())*a1_mat;
                    VectorX<SimpleType> rhsvx = (-dpx_DG + system->LIFT*(element->Fscale.array()*fluxvx).matrix())*a2_mat;
                    VectorX<SimpleType> rhsvy = (-dpy_DG + system->LIFT*(element->Fscale.array()*fluxvy).matrix())*a2_mat;

                    std::vector<VectorX<SimpleType>> result(4);
                    result[IndexPX] = rhspx;
                    result[IndexPY] = rhspy;
                    result[IndexVX] = rhsvx;
                    result[IndexVY] = rhsvy;
                    return result;
                }

                virtual SimpleType GetMaxDt(VectorX<SimpleType> dtscale, int N)
                {
                    //dtscale_min = min(dtscale);
                    SimpleType dtscale_min = dtscale.minCoeff();
                    //CFL_TOU = CFL_DG_values(precision_DG,N);
                    SimpleType CFL = this->CFL_Values(this->Precision, N);
                    //dt_TOU = CFL_TOU*(dtscale_min*2/3)/c;
                    SimpleType dt = CFL*(dtscale_min*2/3)/this->c;
                    //dt_DG = dt_TOU;
                    return dt;
                }
                virtual unsigned int GetNumberOfVariables()
                {
                    return 4;
                }
            };

        }
    }
}

#endif //OPENPSTD_LEE2D_H
