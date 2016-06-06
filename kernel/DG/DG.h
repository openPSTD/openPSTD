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
// Date: 2-6-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_DGELEMENT_H
#define OPENPSTD_DGELEMENT_H

#include <Eigen/Dense>
#include <memory>
#include <vector>
#include <iostream>

#include "GeneralTypes.h"
#include "Jacobi.h"
#include "Vandermonde.h"
#include "DMatrix.h"
#include "Lift.h"
#include "GeometricFactors.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {

            template <typename SimpleType>
            class Element1D;
            template<typename SimpleType>
            class Face1D;
            template<typename SimpleType>
            class Vertex1D;
            template <typename SimpleType>
            class System1D;

            enum class FaceIndex1D
            {
                Left = 0,
                Right = 1
            };

            template <typename SimpleType>
            struct RK {
            public:
                SimpleType a;
                SimpleType b;
                SimpleType c;
            };

            template <typename SimpleType>
            class System1D
            {
            public:
                std::vector<std::shared_ptr<Element1D<SimpleType>>> Elements;
                std::vector<std::shared_ptr<Vertex1D<SimpleType>>> Vertices;

                std::vector<RK<SimpleType>> RK4;

                void Init(int K, int N, SimpleType x1, SimpleType x2)
                {
                    RK<double> tmp;

                    //RK4 constants
                    tmp.a = 0;
                    tmp.b = 1432997174477.0/9575080441755.0;
                    tmp.c = 0;
                    RK4.push_back(tmp);

                    tmp.a = -567301805773.0/1357537059087.0;
                    tmp.b = 5161836677717.0/13612068292357.0;
                    tmp.c = 1432997174477.0/9575080441755.0;
                    RK4.push_back(tmp);

                    tmp.a = -2404267990393.0/2016746695238.0;
                    tmp.b = 1720146321549.0/2090206949498.0;
                    tmp.c = 2526269341429.0/6820363962896.0;
                    RK4.push_back(tmp);

                    tmp.a = -3550918686646.0/2091501179385.0;
                    tmp.b = 3134564353537.0/4481467310338.0;
                    tmp.c = 2006345519317.0/3224310063776.0;
                    RK4.push_back(tmp);

                    tmp.a = -1275806237668.0/842570457699.0;
                    tmp.b = 2277821191437.0/14882151754819.0;
                    tmp.c = 2802321613138.0/2924317926251.0;
                    RK4.push_back(tmp);

                    //Add K+1 vertices in the system
                    VectorX<SimpleType> x = VectorX<SimpleType>::LinSpaced(K+1, x1, x2);
                    for(int k = 0; k < K+1; k++)
                    {
                        std::shared_ptr<Vertex1D<SimpleType>> v = std::make_shared<Vertex1D<SimpleType>>();
                        v->x = x[k];
                        v->Faces = std::vector<std::weak_ptr<Face1D<SimpleType>>>();
                        v->Faces.reserve(2);
                        Vertices.push_back(v);
                    }

                    //add K elements in the system
                    for(int k = 0; k < K; k++)
                    {
                        std::shared_ptr<Element1D<SimpleType>> e = std::make_shared<Element1D<SimpleType>>();
                        e->Init(Vertices[k], Vertices[k+1], N);
                        Elements.push_back(e);
                    }
                }

                void DoRKStep(int RKi, SimpleType T, SimpleType deltaT)
                {
                    SimpleType a = M_PI*2;
                    SimpleType alpha = 1;

                    for(int i = 0; i < Elements.size(); i++)
                    {
                        if(i == 0)
                            Elements[i]->CalculateInput(a, alpha, T+deltaT*RK4[RKi].c);
                        else
                            Elements[i]->CalculateFlux(FaceIndex1D::Left, a, alpha);

                        if(i == Elements.size()-1)
                            Elements[i]->CalculateOutput();
                        else
                            Elements[i]->CalculateFlux(FaceIndex1D::Right, a, alpha);

                        Elements[i]->CalculateRHS(a, alpha);
                    }

                    for(int i = 0; i < Elements.size(); i++)
                    {
                        Elements[i]->DoRKStep(deltaT, RK4[RKi].a, RK4[RKi].b);
                    }
                }

                void DoTimeStep(SimpleType T, SimpleType deltaT)
                {
                    for(int RKi = 0; RKi < 5; RKi++)
                    {
                        DoRKStep(RKi, T, deltaT);
                    }
                }

                void Calculate(SimpleType FinalTime, bool outputMatlab = false)
                {
                    if(outputMatlab)
                    {
                        std::cout << "x(:) = [";
                        for(int i = 0; i < Elements.size(); i++)
                        {
                            VectorX<SimpleType> x = Elements[i]->x;
                            x.conservativeResize(x.size()-1);
                            std::cout << x.transpose() << " ";
                        }
                        std::cout << "];" << std::endl;

                        std::cout << "A(:,1) = [";
                        for(int i = 0; i < Elements.size(); i++)
                        {
                            VectorX<SimpleType> u = Elements[i]->u;
                            u.conservativeResize(u.size()-1);
                            std::cout << u.transpose() << " ";
                        }
                        std::cout << "];" << std::endl;
                    }

                    SimpleType a = 2*M_PI;

                    SimpleType minDistance = Elements[0]->MinNodeDistance();
                    for(int i = 1; i < Elements.size(); i++)
                    {
                        minDistance = std::min(Elements[i]->MinNodeDistance(), minDistance);
                    }
                    SimpleType CFL = 0.75;
                    SimpleType dt = CFL/a*minDistance;
                    dt = dt/2;
                    SimpleType Nsteps = ceil(FinalTime/dt);
                    dt = FinalTime/Nsteps;

                    for(int tstep = 0; tstep < Nsteps; tstep++)
                    {

                        DoTimeStep(tstep*dt, dt);
                        if(outputMatlab)
                        {
                            std::cout << "A(:," << tstep+2 << ") = [";
                            for(int i = 0; i < Elements.size(); i++)
                            {
                                VectorX<SimpleType> u = Elements[i]->u;
                                u.conservativeResize(u.size()-1);
                                std::cout << u.transpose() << " ";
                            }
                            std::cout << "];" << std::endl;
                        }
                    }
                }
            };

            template <typename SimpleType>
            class Element1D : public std::enable_shared_from_this<Element1D<SimpleType>>
            {
            public:
                VectorX<SimpleType> x;
                VectorX<SimpleType> u;
                VectorX<SimpleType> resu;
                VectorX<SimpleType> RHS;
                VectorX<SimpleType> flux;

                MatrixX<SimpleType> Fscale;
                MatrixX<SimpleType> LIFT;
                MatrixX<SimpleType> Dr;
                MatrixX<SimpleType> rx;

                std::vector<std::shared_ptr<Face1D<SimpleType>>> Faces;

                void Init(std::shared_ptr<Vertex1D<SimpleType>> x1, std::shared_ptr<Vertex1D<SimpleType>> x2, int N)
                {
                    if(x1->x > x2->x) std::swap(x1, x2);
                    int Np = N+1;

                    //add the faces tot he element
                    Faces.push_back(std::make_shared<Face1D<SimpleType>>());
                    Faces.push_back(std::make_shared<Face1D<SimpleType>>());

                    Faces[0]->Normal = VectorX<SimpleType>(1);
                    Faces[0]->Normal << -1;
                    Faces[0]->Element = this->shared_from_this();
                    Faces[0]->Vertex = x1;

                    Faces[1]->Normal = VectorX<SimpleType>(1);
                    Faces[1]->Normal << 1;
                    Faces[1]->Element = this->shared_from_this();
                    Faces[1]->Vertex = x2;

                    x1->Faces.push_back(Faces[0]);
                    x2->Faces.push_back(Faces[1]);

                    VectorX<SimpleType> r = JacobiGL<SimpleType>(0, 0, N);
                    x = VectorX<SimpleType>::Ones(Np).array()*x1->x+(r.array()+1)/2*(x2->x-x1->x);

                    MatrixX<SimpleType> V = Vandermonde1D<SimpleType>(N, r);
                    Dr = DMatrix1D(N, r, V);
                    LIFT = Lift1D(Np, 2, V);

                    MatrixX<SimpleType> X = MatrixX<SimpleType>(x.size(),1);
                    X.col(0) = x;
                    MatrixX<SimpleType> J = GeometricFactors1D_J<SimpleType>(X, Dr);
                    rx = GeometricFactors1D_rx<SimpleType>(J);

                    Fscale = VectorX<SimpleType>(2);
                    Fscale << 1/J(0,0), 1/J(J.size()-1,0);

                    u = x.array().sin();
                    resu = VectorX<SimpleType>::Zero(Np);
                    RHS = VectorX<SimpleType>::Zero(Np);
                    flux = VectorX<SimpleType>::Zero(2);
                }

                void CalculateFlux(FaceIndex1D f, SimpleType a, SimpleType alpha, SimpleType uin)
                {
                    int I = f==FaceIndex1D::Left?0:x.size()-1;;
                    flux[(int)f] = (u[I]-uin)*(a*this->Faces[(int)f]->Normal[0]-(1-alpha)*abs(a*this->Faces[(int)f]->Normal[0]))/2;
                }

                void CalculateFlux(FaceIndex1D f, SimpleType a, SimpleType alpha)
                {
                    auto face = Faces[f==FaceIndex1D::Left?0:1];
                    auto otherFace = face->GetOtherSideFace();
                    std::shared_ptr<Element1D<SimpleType>> other = otherFace->Element.lock();
                    int OtherSideIndex = f==FaceIndex1D::Right?0:other->u.size()-1;
                    SimpleType uin = other->u[OtherSideIndex];
                    CalculateFlux(f, a, alpha, uin);
                }

                void CalculateInput(SimpleType a, SimpleType alpha, SimpleType time)
                {
                    int I = 0;
                    SimpleType uin = -sin(a*time);
                    CalculateFlux(FaceIndex1D::Left, a, alpha, uin);
                }

                void CalculateOutput()
                {
                    flux[flux.size()-1] = 0;
                }

                void CalculateRHS(SimpleType a, SimpleType alpha)
                {
                    this->RHS = -a*(this->rx.cwiseProduct(Dr*u)) + LIFT*(Fscale.cwiseProduct(flux));
                }

                void DoRKStep(SimpleType deltaT, SimpleType a, SimpleType b)
                {
                    resu = a*resu+deltaT*RHS;
                    u = u+b*resu;
                }

                SimpleType MinNodeDistance()
                {
                    SimpleType minValue = fabs(x[0]-x[1]);
                    for(int i = 1; i < x.size()-1; i++)
                    {
                        minValue = std::min(fabs(x[i]-x[i+1]), minValue);
                    }
                    return minValue;
                }

            };

            template<typename SimpleType>
            class Face1D: public std::enable_shared_from_this<Face1D<SimpleType>>
            {
            public:
                VectorX<SimpleType> Normal;

                std::shared_ptr<Face1D<SimpleType>> GetOtherSideFace()
                {
                    std::shared_ptr<Element1D<SimpleType>> parent = Element.lock();
                    std::shared_ptr<Face1D<SimpleType>> t = this->shared_from_this();
                    std::shared_ptr<Vertex1D<SimpleType>> v = Vertex.lock();
                    for(int i = 0; i < v->Faces.size(); i++)
                    {
                        if(v->Faces[i].lock() != t)
                        {
                            return v->Faces[i].lock();
                        }
                    }
                    return std::shared_ptr<Face1D<SimpleType>>();
                }

                std::weak_ptr<Element1D<SimpleType>> Element;
                std::weak_ptr<Vertex1D<SimpleType>> Vertex;
            };

            template<typename SimpleType>
            class Vertex1D
            {
            public:
                SimpleType x;
                std::vector<std::weak_ptr<Face1D<SimpleType>>> Faces;
            };
        }
    }
}

#endif //OPENPSTD_DGELEMENT_H
