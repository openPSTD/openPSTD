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
#include "RK.h"

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
            class System1D : public BlackBoxInterface<SimpleType>
            {
            public:
                std::vector<std::shared_ptr<Element1D<SimpleType>>> Elements;
                std::vector<std::shared_ptr<Vertex1D<SimpleType>>> Vertices;

                virtual std::vector<MatrixX<SimpleType>> ComputeRHS(SimpleType time)
                {
                    SimpleType a = M_PI*2;
                    SimpleType alpha = 1;

                    for(int i = 0; i < Elements.size(); i++)
                    {
                        if(i == 0)
                            Elements[i]->CalculateInput(a, alpha, time);
                        else
                            Elements[i]->CalculateFlux(FaceIndex1D::Left, a, alpha);

                        if(i == Elements.size()-1)
                            Elements[i]->CalculateOutput();
                        else
                            Elements[i]->CalculateFlux(FaceIndex1D::Right, a, alpha);

                        Elements[i]->CalculateRHS(a, alpha);
                    }

                    MatrixX<SimpleType> rhs(this->Elements[0]->RHS.size(), this->Elements.size());
                    for(int i = 0; i < rhs.cols(); i++)
                    {
                        rhs.col(i) = this->Elements[i]->RHS;
                    }
                    std::vector<MatrixX<SimpleType>> result;
                    result.push_back(rhs);
                    return result;
                };

                virtual void SetState(std::vector<MatrixX<SimpleType>> state)
                {
                    MatrixX<SimpleType> u = state[0];
                    for(int i = 0; i < u.cols(); i++)
                    {
                        this->Elements[i]->u = u.col(i);
                    }
                };

                virtual std::vector<MatrixX<SimpleType>> GetState()
                {
                    MatrixX<SimpleType> u(this->Elements[0]->u.size(), this->Elements.size());
                    for(int i = 0; i < u.cols(); i++)
                    {
                        u.col(i) = this->Elements[i]->u;
                    }
                    std::vector<MatrixX<SimpleType>> result;
                    result.push_back(u);
                    return result;
                };

                virtual SimpleType GetMaxDT()
                {
                    SimpleType a = M_PI*2;

                    SimpleType minDistance = Elements[0]->MinNodeDistance();
                    for(int i = 1; i < Elements.size(); i++)
                    {
                        minDistance = std::min(Elements[i]->MinNodeDistance(), minDistance);
                    }
                    SimpleType CFL = 0.75;
                    SimpleType dt = CFL/a*minDistance;
                    dt = dt/2;
                    return dt;
                };

                void Init(int K, int N, SimpleType x1, SimpleType x2)
                {
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
            };

            template <typename SimpleType>
            class Element1D : public std::enable_shared_from_this<Element1D<SimpleType>>
            {
            public:
                VectorX<SimpleType> x;
                VectorX<SimpleType> u;
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
