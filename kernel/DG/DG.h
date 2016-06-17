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
#include <Eigen/src/Core/IO.h>
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
            template<typename SimpleType, typename DEElementStore>
            class Element1D;
            template<typename SimpleType, typename DEElementStore>
            class Face1D;
            template<typename SimpleType, typename DEElementStore>
            class Vertex1D;
            template <typename SimpleType, typename DEElementStore>
            class System1D;

            enum class FaceIndex1D
            {
                Left = 0,
                Right = 1
            };

            class NoElementStore{};

            template <typename SimpleType, typename DEElementStore = NoElementStore>
            class DG1DDE
            {
            public:
                virtual void Initialize(
                        std::shared_ptr<Element1D<SimpleType, DEElementStore>> element) = 0;
                virtual std::vector<VectorX<SimpleType>> CalculateRHS(
                        std::shared_ptr<Element1D<SimpleType, DEElementStore>> element,
                        std::shared_ptr<System1D<SimpleType, DEElementStore>> system,
                        SimpleType time) = 0;
                virtual SimpleType GetMaxDt(SimpleType minDistance, int N) = 0;
                virtual unsigned int GetNumberOfVariables() = 0;
            };

            template <typename SimpleType, typename DEElementStore = NoElementStore>
            class System1D : public BlackBoxInterface<SimpleType>, public std::enable_shared_from_this<System1D<SimpleType, DEElementStore>>
            {
            private:
                int K, N;
                std::shared_ptr<DG1DDE<SimpleType, DEElementStore>> _DE;

            public:
                std::vector<std::shared_ptr<Element1D<SimpleType, DEElementStore>>> Elements;
                std::vector<std::shared_ptr<Vertex1D<SimpleType, DEElementStore>>> Vertices;

                MatrixX<SimpleType> LIFT;
                MatrixX<SimpleType> Dr;

                System1D(int K, int N, SimpleType x1, SimpleType x2, std::shared_ptr<DG1DDE<SimpleType, DEElementStore>> de)
                {
                    this->K = K;
                    this->N = N;
                    this->_DE = de;

                    VectorX<SimpleType> r = JacobiGL<SimpleType>(0, 0, N);
                    MatrixX<SimpleType> V = Vandermonde1D<SimpleType>(N, r);
                    Dr = DMatrix1D(N, r, V);
                    LIFT = Lift1D(N+1, 2, V);

                    //Add K+1 vertices in the system
                    VectorX<SimpleType> x = VectorX<SimpleType>::LinSpaced(K+1, x1, x2);
                    for(int k = 0; k < K+1; k++)
                    {
                        std::shared_ptr<Vertex1D<SimpleType, DEElementStore>> v = std::make_shared<Vertex1D<SimpleType, DEElementStore>>();
                        v->x = x[k];
                        v->Faces = std::vector<std::weak_ptr<Face1D<SimpleType, DEElementStore>>>();
                        v->Faces.reserve(2);
                        Vertices.push_back(v);
                    }

                    //add K elements in the system
                    for(int k = 0; k < K; k++)
                    {
                        std::shared_ptr<Element1D<SimpleType, DEElementStore>> e = std::make_shared<Element1D<SimpleType, DEElementStore>>();
                        e->Init(Vertices[k], Vertices[k+1], N, de->GetNumberOfVariables(), Dr);
                        Elements.push_back(e);
                    }

                    for(int k = 0; k < K; k++)
                    {
                        _DE->Initialize(Elements[k]);
                    }
                }

                virtual std::vector<MatrixX<SimpleType>> ComputeRHS(SimpleType time)
                {
                    std::vector<MatrixX<SimpleType>> result;
                    int nVariables = this->_DE->GetNumberOfVariables();
                    for(int j = 0; j < nVariables; j++)
                    {
                        result.push_back(MatrixX<SimpleType>(this->N+1, this->Elements.size()));
                    }

                    for(int i = 0; i < K; i++)
                    {
                        auto ElementRHS = this->_DE->CalculateRHS(this->Elements[i], this->shared_from_this(), time);
                        for(int j = 0; j < nVariables; j++)
                        {
                            result[j].col(i) = ElementRHS[j];
                        }
                    }

                    return result;
                };

                virtual void SetState(std::vector<MatrixX<SimpleType>> state)
                {
                    //todo test if variable count is correct
                    //int nVariables = this->_DE->GetNumberOfVariables();
                    for(int j = 0; j < state.size(); j++)
                    {
                        MatrixX<SimpleType> u = state[j];
                        for(int i = 0; i < u.cols(); i++)
                        {
                            this->Elements[i]->u[j] = u.col(i);
                        }
                    }
                };

                virtual std::vector<MatrixX<SimpleType>> GetState()
                {
                    int nVariables = this->_DE->GetNumberOfVariables();
                    std::vector<MatrixX<SimpleType>> result;
                    for(int j = 0; j < nVariables; j++)
                    {
                        MatrixX<SimpleType> u(N+1, this->Elements.size());
                        for(int i = 0; i < u.cols(); i++)
                        {
                            u.col(i) = this->Elements[i]->u[j];
                        }
                        result.push_back(u);
                    }
                    return result;
                };

                virtual SimpleType GetMaxDT()
                {
                    SimpleType minDistance = Elements[0]->MinNodeDistance();
                    for(int i = 1; i < Elements.size(); i++)
                    {
                        minDistance = std::min(Elements[i]->MinNodeDistance(), minDistance);
                    }
                    return _DE->GetMaxDt(minDistance, N);
                };

                virtual unsigned int GetNumberOfVariables()
                {
                    return _DE->GetNumberOfVariables();
                };

                virtual void OutputMatlabMetadata()
                {
                    Eigen::IOFormat MatlabFmt(Eigen::FullPrecision, Eigen::DontAlignCols, " ", ";\n", "", "", "[", "]");
                    MatrixX<SimpleType> x(N+1, this->Elements.size());

                    for(int i = 0; i < this->Elements.size(); i++)
                    {
                        x.col(i) = this->Elements[i]->x;
                    }

                    std::cout << 'X' << "(:,:) = ";
                    std::cout << x.format(MatlabFmt) << ";" << std::endl;

                };

                virtual void OutputMatlabData(int index)
                {
                    std::vector<MatrixX<SimpleType>> state = GetState();
                    Eigen::IOFormat MatlabFmt(Eigen::FullPrecision, Eigen::DontAlignCols, " ", ";\n", "", "", "[", "]");
                    for(char i = 0; i < state.size(); i++)
                    {
                        std::cout << (char)('A'+i) << "(:,:," << index << ") = ";
                        std::cout << state[i].format(MatlabFmt) << ";" << std::endl;
                    }
                };
            };

            template <typename SimpleType, typename DEElementStore = NoElementStore>
            class Element1D : public std::enable_shared_from_this<Element1D<SimpleType, DEElementStore>>
            {
            public:
                VectorX<SimpleType> x;
                std::vector<VectorX<SimpleType>> u;
                DEElementStore DEStore;

                MatrixX<SimpleType> Fscale;
                MatrixX<SimpleType> rx;

                std::vector<std::shared_ptr<Face1D<SimpleType, DEElementStore>>> Faces;

                void Init(std::shared_ptr<Vertex1D<SimpleType, DEElementStore>> x1,
                            std::shared_ptr<Vertex1D<SimpleType, DEElementStore>> x2,
                            int N, int nVariables, const MatrixX<SimpleType>& Dr)
                {
                    if(x1->x > x2->x) std::swap(x1, x2);
                    int Np = N+1;

                    //add the faces tot he element
                    Faces.push_back(std::make_shared<Face1D<SimpleType, DEElementStore>>());
                    Faces.push_back(std::make_shared<Face1D<SimpleType, DEElementStore>>());

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

                    MatrixX<SimpleType> X = MatrixX<SimpleType>(x.size(),1);
                    X.col(0) = x;

                    MatrixX<SimpleType> J = GeometricFactors1D_J<SimpleType>(X, Dr);
                    rx = GeometricFactors1D_rx<SimpleType>(J);

                    Fscale = VectorX<SimpleType>(2);
                    Fscale << 1/J(0,0), 1/J(J.size()-1,0);

                    for(int i = 0; i < nVariables; i++)
                    {
                        u.push_back(VectorX<SimpleType>::Zero(N+1));
                    }
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

            template<typename SimpleType, typename DEElementStore = NoElementStore>
            class Face1D: public std::enable_shared_from_this<Face1D<SimpleType, DEElementStore>>
            {
            public:
                VectorX<SimpleType> Normal;

                std::shared_ptr<Face1D<SimpleType, DEElementStore>> GetOtherSideFace()
                {
                    std::shared_ptr<Element1D<SimpleType, DEElementStore>> parent = Element.lock();
                    std::shared_ptr<Face1D<SimpleType, DEElementStore>> t = this->shared_from_this();
                    std::shared_ptr<Vertex1D<SimpleType, DEElementStore>> v = Vertex.lock();
                    for(int i = 0; i < v->Faces.size(); i++)
                    {
                        if(v->Faces[i].lock() != t)
                        {
                            return v->Faces[i].lock();
                        }
                    }
                    return std::shared_ptr<Face1D<SimpleType, DEElementStore>>();
                }

                bool OtherSideExist()
                {
                    auto otherSide = this->GetOtherSideFace();
                    if(otherSide)
                        return true;
                    else
                        return false;
                }

                std::weak_ptr<Element1D<SimpleType, DEElementStore>> Element;
                std::weak_ptr<Vertex1D<SimpleType, DEElementStore>> Vertex;
            };

            template<typename SimpleType, typename DEElementStore = NoElementStore>
            class Vertex1D
            {
            public:
                SimpleType x;
                std::vector<std::weak_ptr<Face1D<SimpleType, DEElementStore>>> Faces;
            };
        }
    }
}

#endif //OPENPSTD_DGELEMENT_H
