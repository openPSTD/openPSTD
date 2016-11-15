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
// Date: 15-11-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_DG2D_H
#define OPENPSTD_DG2D_H

#include <Eigen/Dense>
#include <Eigen/src/Core/IO.h>
#include <memory>

#include "GeneralTypes.h"
#include "RK.h"
#include "Nodes.h"
#include "Coordinates.h"
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
            template<typename SimpleType, typename DEElementStore>
            class Element2D;
            template<typename SimpleType, typename DEElementStore>
            class Face2D;
            template<typename SimpleType, typename DEElementStore>
            class Edge2D;
            template<typename SimpleType, typename DEElementStore>
            class Vertex2D;
            template <typename SimpleType, typename DEElementStore>
            class System2D;

            class DG2DNoElementStore{};

            template <typename SimpleType, typename DEElementStore = DG2DNoElementStore>
            class DG2DDE
            {
                virtual void Initialize(
                        std::shared_ptr<Element2D<SimpleType, DEElementStore>> element) = 0;
                virtual std::vector<VectorX<SimpleType>> CalculateRHS(
                        std::shared_ptr<Element2D<SimpleType, DEElementStore>> element,
                        std::shared_ptr<System2D<SimpleType, DEElementStore>> system,
                        SimpleType time) = 0;
                virtual SimpleType GetMaxDt(VectorX<SimpleType> dtscale, int N) = 0;
                virtual unsigned int GetNumberOfVariables() = 0;
            };

            template<typename SimpleType, typename DEElementStore = DG2DNoElementStore>
            class System2D : public BlackBoxInterface<SimpleType>, public std::enable_shared_from_this<System2D<SimpleType, DEElementStore>>
            {
            private:
                int K, N;
                std::shared_ptr<DG2DDE<SimpleType, DEElementStore>> _DE;

            public:
                std::vector<std::shared_ptr<Vertex2D<SimpleType, DEElementStore>>> Vertices;
                std::vector<std::shared_ptr<Edge2D<SimpleType, DEElementStore>>> Edges;
                std::vector<std::shared_ptr<Element2D<SimpleType, DEElementStore>>> Element;

                MatrixX<SimpleType> LIFT;
                MatrixX<SimpleType> Dr;
                MatrixX<SimpleType> Ds;

                System1D(int N, VectorX<SimpleType> x1, VectorX<SimpleType> x2, std::shared_ptr<DG2DDE<SimpleType, DEElementStore>> de)
                {
                    this->N = N;
                    this->_DE = de;

                    // Compute nodal set
                    MatrixX<SimpleType> xy = Nodes2D(N);
                    MatrixX<SimpleType> rs = xytors(N);

                    // Build reference element matrices
                    V = Vandermonde2D(N,r,s);
                    auto DMatrices = DMatrices2D(N, rs, V);
                    this->Dr = DMatrices.Dr;
                    this->Ds = DMatrices.Ds;
                    this->LIFT = Lift2D(N, 3, V, rs);
                }

                void BuildSquareGrid(VectorX<SimpleType> x1, VectorX<SimpleType> x2, SimpleType rectSize)
                {
                    //build "unstructured" grid
                    int widthElements = (x2[0]-x1[0])/rectSize;
                    int heightElements = (x2[1]-x1[1])/rectSize;

                    int widthVertices = widthElements + 1;
                    int heightVertices = heightElements + 1;

                    for (int j = 0; j < heightVertices; ++j)
                    {
                        for (int i = 0; i < widthVertices; ++i)
                        {
                            auto v = std::make_shared<Vertex2D<SimpleType, DEElementStore>>();
                            v->Position[0] = x1[0] + i * rectSize;
                            v->Position[1] = x1[1] + j * rectSize;
                            this->Vertices.push_back(v);
                        }
                    }

                    for (int j = 0; j < heightElements; ++j)
                    {
                        for (int i = 0; i < widthElements; ++i)
                        {
                            if (j == 0)
                            {
                                auto t = std::make_shared<Edge2D<SimpleType, DEElementStore>>(
                                        this->Vertices[widthVertices * (j + 0) + (i + 0)],
                                        this->Vertices[widthVertices * (j + 0) + (i + 1)]);
                                Edges.push_back(t);
                            }
                            auto r = std::make_shared<Edge2D<SimpleType, DEElementStore>>(
                                    this->Vertices[widthVertices * (j + 0) + (i + 1)],
                                    this->Vertices[widthVertices * (j + 1) + (i + 1)]);
                            Edges.push_back(r);
                            if (j == 0)
                            {
                                auto b = std::make_shared<Edge2D<SimpleType, DEElementStore>>(
                                        this->Vertices[widthVertices * (j + 1) + (i + 1)],
                                        this->Vertices[widthVertices * (j + 1) + (i + 0)]);
                                Edges.push_back(b);
                            }
                            auto l = std::make_shared<Edge2D<SimpleType, DEElementStore>>(
                                    this->Vertices[widthVertices * (j + 1) + (i + 0)],
                                    this->Vertices[widthVertices * (j + 0) + (i + 0)]);
                            Edges.push_back(l);
                            auto d = std::make_shared<Edge2D<SimpleType, DEElementStore>>(
                                    this->Vertices[widthVertices * (j + 0) + (i + 0)],
                                    this->Vertices[widthVertices * (j + 1) + (i + 1)]);
                            Edges.push_back(d);
                        }
                    }

                    for (int j = 0; j < heightElements; ++j)
                    {
                        for (int i = 0; i < widthElements; ++i)
                        {
                            std::vector<std::shared_ptr<Vertex2D<SimpleType, DEElementStore>>> v;
                            v.push_back(this->Vertices[widthVertices * (j + 0) + (i + 0)]);
                            v.push_back(this->Vertices[widthVertices * (j + 0) + (i + 1)]);
                            v.push_back(this->Vertices[widthVertices * (j + 1) + (i + 1)]);
                            auto tr = std::make_shared<Element2D<SimpleType, DEElementStore>>();
                            Element.push_back(tr);
                            tr->init(v, N, de->GetNumberOfVariables(), rs, Dr, Ds);

                            v.clear();
                            v.push_back(this->Vertices[widthVertices * (j + 0) + (i + 0)]);
                            v.push_back(this->Vertices[widthVertices * (j + 1) + (i + 1)]);
                            v.push_back(this->Vertices[widthVertices * (j + 1) + (i + 0)]);
                            auto bl = std::make_shared<Element2D<SimpleType, DEElementStore>>();
                            Element.push_back(bl);
                            bl->init(v, N, de->GetNumberOfVariables(), rs, Dr, Ds);
                        }
                    }

                    for(int k = 0; k < Element.size(); k++)
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

                    for(int i = 0; i < Element.size(); i++)
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
                    //todo test if variable size is correct
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
                    //todo implement DTScale
                    VectorX<SimpleType> dtscale(Element.size());
                    return _DE->GetMaxDt(dtscale, N);
                };

                virtual unsigned int GetNumberOfVariables()
                {
                    return _DE->GetNumberOfVariables();
                };
            };

            template<typename SimpleType, typename DEElementStore>
            class Element2D : public std::enable_shared_from_this<Element2D<SimpleType, DEElementStore>>
            {
            public:
                std::vector<std::shared_ptr<Face2D<SimpleType, DEElementStore>>> Faces;
                std::vector<std::weak_ptr<Vertex2D<SimpleType, DEElementStore>>> Vertices;
                VectorX<SimpleType> x;
                VectorX<SimpleType> y;
                VectorX<SimpleType> rx;
                VectorX<SimpleType> sx;
                VectorX<SimpleType> ry;
                VectorX<SimpleType> sy;
                VectorX<SimpleType> J;
                std::vector<VectorX<SimpleType>> u;
                DEElementStore DEStore;
                MatrixX<SimpleType> Fscale;

                void init(std::vector<std::shared_ptr<Vertex2D<SimpleType, DEElementStore>>> v, int N, int nVariables,
                          const MatrixX<SimpleType> &rs, const MatrixX<SimpleType> &Dr, const MatrixX<SimpleType> &Ds)
                {
                    for(int i = 0; i < v.size(); i++)
                    {
                        this->Vertices.push_back(v[i]);
                        auto f = std::make_shared<Face2D<SimpleType, DEElementStore>>();
                        Faces.push_back(f);
                        f->Element = this->shared_from_this();
                        //todo fix normals
                        //f->Normal = ;
                        f->Edge = v[i].FindEdgeTo(v[(i+1)%v.size()]);
                    }

                    ArrayX<SimpleType> r = rs.col(0);
                    ArrayX<SimpleType> s = rs.col(1);
                    //x = 0.5*(-(r+s)*VX(va)+(1+r)*VX(vb)+(1+s)*VX(vc));
                    x = 0.5*(
                            -(r+s)*this->Vertices[0]->Position[0]
                            +(r+1)*this->Vertices[1]->Position[0]
                            +(s+1)*this->Vertices[2]->Position[0]);
                    //y = 0.5*(-(r+s)*VY(va)+(1+r)*VY(vb)+(1+s)*VY(vc));
                    y = 0.5*(
                            -(r+s)*this->Vertices[0]->Position[1]
                            +(r+1)*this->Vertices[1]->Position[1]
                            +(s+1)*this->Vertices[2]->Position[1]);

                    auto geometric = GeometricFactors2D(x, y, Dr, Ds);
                    this->J = geometric.J;
                    this->rx = geometric.rx;
                    this->sx = geometric.sx;
                    this->ry = geometric.ry;
                    this->sy = geometric.sy;

                    //todo calculate Fscale

                    for(int i = 0; i < nVariables; i++)
                    {
                        u.push_back(VectorX<SimpleType>::Zero(N+1));
                    }
                }

                MatrixX<SimpleType> CalcNormals()
                {
                    return MatrixX<SimpleType>();
                }
            };

            template<typename SimpleType, typename DEElementStore>
            class Face2D: public std::enable_shared_from_this<Face2D<SimpleType, DEElementStore>>
            {
            public:
                VectorX<SimpleType> Normal;

                std::weak_ptr<Element2D<SimpleType, DEElementStore>> Element;
                std::weak_ptr<Edge2D<SimpleType, DEElementStore>> Edge;

                std::shared_ptr<Face2D<SimpleType, DEElementStore>> GetOtherSideFace()
                {
                    std::shared_ptr<Face2D<SimpleType, DEElementStore>> t = this->shared_from_this();
                    std::shared_ptr<Edge2D<SimpleType, DEElementStore>> e = Edge.lock();
                    for(int i = 0; i < e->Faces.size(); i++)
                    {
                        if(e->Faces[i].lock() != t)
                        {
                            return v->Faces[i].lock();
                        }
                    }
                    return std::shared_ptr<Face2D<SimpleType, DEElementStore>>();
                }

                bool OtherSideExist()
                {
                    auto otherSide = this->GetOtherSideFace();
                    if(otherSide)
                        return true;
                    else
                        return false;
                }
            };

            template<typename SimpleType, typename DEElementStore>
            class Edge2D: public std::enable_shared_from_this<System2D<SimpleType, DEElementStore>>
            {
            public:
                Edge2D(std::weak_ptr<Vertex2D<SimpleType, DEElementStore>> v1, std::weak_ptr<Vertex2D<SimpleType, DEElementStore>> v2)
                {
                    this->v1 = v1;
                    this->v2 = v2;

                    this->v1.lock()->Edges.push_back(this->shared_from_this());
                    this->v2.lock()->Edges.push_back(this->shared_from_this());
                }

                std::vector<std::weak_ptr<Face2D<SimpleType, DEElementStore>>> Faces;

                std::weak_ptr<Vertex2D<SimpleType, DEElementStore>> v1;
                std::weak_ptr<Vertex2D<SimpleType, DEElementStore>> v2;

                bool IsConnectedTo(std::weak_ptr<Vertex2D<SimpleType, DEElementStore>> v)
                {
                    return v == v1 || v == v2;
                }
            };

            template<typename SimpleType, typename DEElementStore>
            class Vertex2D
            {
            public:
                VectorX<SimpleType> Position;
                std::vector<std::weak_ptr<Edge2D<SimpleType, DEElementStore>>> Edges;

                std::shared_ptr<Edge2D<SimpleType, DEElementStore>> FindEdgeTo(Vertex2D v)
                {
                    auto it = std::find(Edges.begin(), Edges.end(), [v](const Edge2D & e) { return e.IsConnectedTo(v); });
                    return it->lock();
                };
            };
        }
    }
}
#endif //OPENPSTD_DG2D_H
