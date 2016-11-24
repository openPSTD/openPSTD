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
#include <vector>
#include <iostream>

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
            public:
                virtual void Initialize(
                        std::shared_ptr<Element2D<SimpleType, DEElementStore>> element,
                        std::shared_ptr<System2D<SimpleType, DEElementStore>> system) = 0;
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

                VectorX<SimpleType> CalculateDTScale()
                {
                    VectorX<SimpleType> dtscale(this->Element.size());
                    for (int i = 0; i < this->Element.size(); ++i)
                    {
                        dtscale[i] = this->Element[i]->GetDTScale();
                    }
                    return dtscale;
                }

            public:
                std::vector<std::shared_ptr<Vertex2D<SimpleType, DEElementStore>>> Vertices;
                std::vector<std::shared_ptr<Edge2D<SimpleType, DEElementStore>>> Edges;
                std::vector<std::shared_ptr<Element2D<SimpleType, DEElementStore>>> Element;

                MatrixX<SimpleType> LIFT;
                MatrixX<SimpleType> Dr;
                MatrixX<SimpleType> Ds;
                MatrixX<SimpleType> rs;

                System2D(int N, std::shared_ptr<DG2DDE<SimpleType, DEElementStore>> de)
                {
                    this->N = N;
                    this->_DE = de;

                    // Compute nodal set
                    MatrixX<SimpleType> xy = Nodes2D<SimpleType>(N);
                    this->rs = xytors<SimpleType>(xy);

                    // Build reference element matrices
                    MatrixX<SimpleType> V = Vandermonde2D<SimpleType>(N,rs);
                    auto DMatrices = DMatrices2D<SimpleType>(N, rs, V);
                    this->Dr = DMatrices.Dr;
                    this->Ds = DMatrices.Ds;
                    this->LIFT = Lift2D<SimpleType>(N, 3, V, rs);
                }

                void InitializeElements()
                {
                    for(int k = 0; k < Element.size(); k++)
                    {
                        this->Element[k]->InitVariables(this->shared_from_this(), N, _DE->GetNumberOfVariables(), rs, Dr, Ds);
                        _DE->Initialize(this->Element[k], this->shared_from_this());
                    }
                }

                virtual std::vector<MatrixX<SimpleType>> ComputeRHS(SimpleType time)
                {
                    std::vector<MatrixX<SimpleType>> result;
                    int nVariables = this->_DE->GetNumberOfVariables();
                    for(int j = 0; j < nVariables; j++)
                    {
                        result.push_back(MatrixX<SimpleType>(this->GetNp(), this->Element.size()));
                    }

                    for(int i = 0; i < Element.size(); i++)
                    {
                        auto ElementRHS = this->_DE->CalculateRHS(this->Element[i], this->shared_from_this(), time);
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
                            this->Element[i]->u[j] = u.col(i);
                        }
                    }
                };

                virtual std::vector<MatrixX<SimpleType>> GetState()
                {
                    int nVariables = this->_DE->GetNumberOfVariables();
                    std::vector<MatrixX<SimpleType>> result;
                    for(int j = 0; j < nVariables; j++)
                    {
                        MatrixX<SimpleType> u(this->GetNp(), this->Element.size());
                        for(int i = 0; i < u.cols(); i++)
                        {
                            u.col(i) = this->Element[i]->u[j];
                        }
                        result.push_back(u);
                    }
                    return result;
                };

                virtual SimpleType GetMaxDT()
                {
                    VectorX<SimpleType> dtscale = CalculateDTScale();
                    return _DE->GetMaxDt(dtscale, N);
                };

                virtual unsigned int GetNumberOfVariables()
                {
                    return _DE->GetNumberOfVariables();
                };

                unsigned int GetN()
                {
                    return this->N;
                }

                unsigned int GetNp()
                {
                    return (this->N+1)*(this->N+2)/2;
                }

                unsigned int GetNfp()
                {
                    return this->N+1;
                }

                virtual void OutputMatlabMetadata()
                {
                    Eigen::IOFormat MatlabFmt(Eigen::FullPrecision, Eigen::DontAlignCols, " ", ";\n", "", "", "[", "]");

                    MatrixX<SimpleType> x(this->GetNp(), this->Element.size());
                    MatrixX<SimpleType> y(this->GetNp(), this->Element.size());
                    for(int i = 0; i < this->Element.size(); i++)
                    {
                        x.col(i) = this->Element[i]->x;
                        y.col(i) = this->Element[i]->y;
                    }

                    std::cout << 'X' << "(:,:) = ";
                    std::cout << x.format(MatlabFmt) << ";" << std::endl;

                    std::cout << 'Y' << "(:,:) = ";
                    std::cout << y.format(MatlabFmt) << ";" << std::endl;
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

            template<typename SimpleType, typename DEElementStore = DG2DNoElementStore>
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
                VectorX<SimpleType> nx;
                VectorX<SimpleType> ny;
                VectorX<SimpleType> sJ;
                VectorX<SimpleType> Fscale;
                std::vector<VectorX<SimpleType>> u;
                DEElementStore DEStore;

                Element2D(std::vector<std::shared_ptr<Vertex2D<SimpleType, DEElementStore>>> v)
                {
                    for(int i = 0; i < v.size(); i++)
                    {
                        this->Vertices.push_back(v[i]);
                    }
                }

                void InitVariables(std::shared_ptr<System2D<SimpleType, DEElementStore>> system, int N, int nVariables,
                                   const MatrixX<SimpleType> &rs, const MatrixX<SimpleType> &Dr, const MatrixX<SimpleType> &Ds)
                {
                    for(int i = 0; i < this->Vertices.size(); i++)
                    {
                        std::shared_ptr<Edge2D<SimpleType, DEElementStore>> edge =
                                this->Vertices[i].lock()->FindEdgeToOrCreate(system, this->Vertices[(i + 1) % this->Vertices.size()].lock());

                        auto f = std::make_shared<Face2D<SimpleType, DEElementStore>>();
                        Faces.push_back(f);
                        f->Element = this->shared_from_this();

                        f->Edge = edge;
                        edge->Faces.push_back(f);
                    }

                    ArrayX<SimpleType> r = rs.col(0);
                    ArrayX<SimpleType> s = rs.col(1);
                    //x = 0.5*(-(r+s)*VX(va)+(1+r)*VX(vb)+(1+s)*VX(vc));
                    x = 0.5*(
                            -(r+s)*this->Vertices[0].lock()->Position[0]
                            +(r+1)*this->Vertices[1].lock()->Position[0]
                            +(s+1)*this->Vertices[2].lock()->Position[0]);
                    //y = 0.5*(-(r+s)*VY(va)+(1+r)*VY(vb)+(1+s)*VY(vc));
                    y = 0.5*(
                            -(r+s)*this->Vertices[0].lock()->Position[1]
                            +(r+1)*this->Vertices[1].lock()->Position[1]
                            +(s+1)*this->Vertices[2].lock()->Position[1]);

                    auto geometric = GeometricFactors2D<SimpleType>(x, y, Dr, Ds);
                    this->J = geometric.J;
                    this->rx = geometric.rx;
                    this->sx = geometric.sx;
                    this->ry = geometric.ry;
                    this->sy = geometric.sy;

                    this->CalcNormal(N, rs, Dr, Ds);
                    this->CalcFscale(N, rs);

                    for(int i = 0; i < nVariables; i++)
                    {
                        u.push_back(VectorX<SimpleType>::Zero(rs.rows()));
                    }

                    //face 1
                    for(int i = 0; i < rs.rows(); i++)
                    {
                        if(std::abs(s(i)+1) < NODETOL) //fmask1   = find( abs(s+1) < NODETOL)';
                        {
                            Faces[0]->NodesOnFace.push_back(i);
                        }
                    }

                    //face 2
                    for(int i = 0; i < rs.rows(); i++)
                    {
                        if(std::abs(r(i)+s(i)) < NODETOL) //fmask2   = find( abs(r+s) < NODETOL)';
                        {
                            Faces[1]->NodesOnFace.push_back(i);
                        }
                    }

                    //face 3
                    for(int i = 0; i < rs.rows(); i++)
                    {
                        if(std::abs(r(i)+1) < NODETOL) //fmask3   = find( abs(r+1) < NODETOL)';
                        {
                            Faces[2]->NodesOnFace.push_back(i);
                        }
                    }
                }

                SimpleType GetDTScale()
                {
                    SimpleType len1 = std::sqrt(std::pow(Vertices[0].lock()->Position[0]-Vertices[1].lock()->Position[0], 2)+
                                                std::pow(Vertices[0].lock()->Position[1]-Vertices[1].lock()->Position[1], 2));
                    SimpleType len2 = std::sqrt(std::pow(Vertices[1].lock()->Position[0]-Vertices[2].lock()->Position[0], 2)+
                                                std::pow(Vertices[1].lock()->Position[1]-Vertices[2].lock()->Position[1], 2));
                    SimpleType len3 = std::sqrt(std::pow(Vertices[2].lock()->Position[0]-Vertices[0].lock()->Position[0], 2)+
                                                std::pow(Vertices[2].lock()->Position[1]-Vertices[0].lock()->Position[1], 2));
                    SimpleType sper = (len1+len2+len3)/2.0;
                    SimpleType Area = std::sqrt(sper*(sper-len1)*(sper-len2)*(sper-len3));

                    return Area/sper;
                }

                VectorX<SimpleType> GetFaceInternalValues(int i)
                {
                    std::vector<VectorX<SimpleType>> values;
                    for (int j = 0; j < Faces.size(); ++j)
                    {
                        values.push_back(Faces[j]->GetValuesOfNodes(i));
                    }

                    int count = 0;
                    for (int j = 0; j < values.size(); ++j)
                    {
                        count += values[j].size();
                    }

                    VectorX<SimpleType> result(count);
                    count = 0;
                    for (int j = 0; j < values.size(); ++j)
                    {
                        result.segment(count, values[j].size()) = values[j];
                        count += values[j].size();
                    }
                    return result;
                }

                VectorX<SimpleType> GetFaceExternalValues(int i)
                {
                    std::vector<VectorX<SimpleType>> values;
                    for (int j = 0; j < Faces.size(); ++j)
                    {
                        values.push_back(Faces[j]->GetValuesOfOtherSide(i));
                    }

                    int count = 0;
                    for (int j = 0; j < values.size(); ++j)
                    {
                        count += values[j].size();
                    }

                    VectorX<SimpleType> result(count);
                    count = 0;
                    for (int j = 0; j < values.size(); ++j)
                    {
                        result.segment(count, values[j].size()) = values[j];
                        count += values[j].size();
                    }
                    return result;
                }

            private:
                void CalcNormal(int N, const MatrixX<SimpleType> &rs, const MatrixX<SimpleType> &Dr, const MatrixX<SimpleType> &Ds)
                {
                    int Nfp = N+1;

                    VectorX<SimpleType> xr = Dr*x;
                    VectorX<SimpleType> yr = Dr*y;
                    VectorX<SimpleType> xs = Ds*x;
                    VectorX<SimpleType> ys = Ds*y;

                    VectorX<SimpleType> r = rs.col(0);
                    VectorX<SimpleType> s = rs.col(1);

                    int j = 0;
                    VectorX<SimpleType> fxr(3*Nfp);
                    VectorX<SimpleType> fyr(3*Nfp);
                    VectorX<SimpleType> fxs(3*Nfp);
                    VectorX<SimpleType> fys(3*Nfp);

                    //face 1
                    for(int i = 0; i < rs.rows(); i++)
                    {
                        if(std::abs(s(i)+1) < NODETOL) //fmask1   = find( abs(s+1) < NODETOL)';
                        {
                            fxr(j) = xr(i);
                            fyr(j) = yr(i);
                            fxs(j) = xs(i);
                            fys(j) = ys(i);
                            j++;
                        }
                    }

                    //face 2
                    for(int i = 0; i < rs.rows(); i++)
                    {
                        if(std::abs(r(i)+s(i)) < NODETOL) //fmask2   = find( abs(r+s) < NODETOL)';
                        {
                            fxr(j) = xr(i);
                            fyr(j) = yr(i);
                            fxs(j) = xs(i);
                            fys(j) = ys(i);
                            j++;
                        }
                    }

                    //face 3
                    for(int i = 0; i < rs.rows(); i++)
                    {
                        if(std::abs(r(i)+1) < NODETOL) //fmask3   = find( abs(r+1) < NODETOL)';
                        {
                            fxr(j) = xr(i);
                            fyr(j) = yr(i);
                            fxs(j) = xs(i);
                            fys(j) = ys(i);
                            j++;
                        }
                    }

                    ArrayX<SimpleType> nx(3*Nfp);
                    ArrayX<SimpleType> ny(3*Nfp);

                    //% face 1
                    //nx(fid1, :) =  fyr(fid1, :); ny(fid1, :) = -fxr(fid1, :);
                    nx.segment(0*Nfp, Nfp) = fyr.segment(0*Nfp, Nfp);
                    ny.segment(0*Nfp, Nfp) = -fxr.segment(0*Nfp, Nfp);

                    //% face 2
                    //nx(fid2, :) =  fys(fid2, :)-fyr(fid2, :); ny(fid2, :) = -fxs(fid2, :)+fxr(fid2, :);
                    nx.segment(1*Nfp, Nfp) = fys.segment(1*Nfp, Nfp)-fyr.segment(1*Nfp, Nfp);
                    ny.segment(1*Nfp, Nfp) = -fxs.segment(1*Nfp, Nfp)+fxr.segment(1*Nfp, Nfp);

                    //% face 3
                    //nx(fid3, :) = -fys(fid3, :); ny(fid3, :) =  fxs(fid3, :);
                    nx.segment(2*Nfp, Nfp) = -fys.segment(2*Nfp, Nfp);
                    ny.segment(2*Nfp, Nfp) = fxs.segment(2*Nfp, Nfp);

                    // % normalise
                    // sJ = sqrt(nx.*nx+ny.*ny); nx = nx./sJ; ny = ny./sJ;
                    ArrayX<SimpleType> sJ = (nx.square()+ny.square()).sqrt();
                    nx = nx/sJ;
                    ny = ny/sJ;

                    this->nx = nx;
                    this->ny = ny;
                    this->sJ = sJ;
                }

                void CalcFscale(int N, const MatrixX<SimpleType> &rs)
                {
                    VectorX<SimpleType> r = rs.col(0);
                    VectorX<SimpleType> s = rs.col(1);

                    int Nfp = N+1;
                    this->Fscale = VectorX<SimpleType>(3*Nfp);

                    int j = 0;

                    //face 1
                    for(int i = 0; i < rs.rows(); i++)
                    {
                        if(std::abs(s(i)+1) < NODETOL) //fmask1   = find( abs(s+1) < NODETOL)';
                        {
                            this->Fscale(j) = sJ(j)/J(i);
                            j++;
                        }
                    }

                    //face 2
                    for(int i = 0; i < rs.rows(); i++)
                    {
                        if(std::abs(r(i)+s(i)) < NODETOL) //fmask2   = find( abs(r+s) < NODETOL)';
                        {
                            this->Fscale(j) = sJ(j)/J(i);
                            j++;
                        }
                    }

                    //face 3
                    for(int i = 0; i < rs.rows(); i++)
                    {
                        if(std::abs(r(i)+1) < NODETOL) //fmask3   = find( abs(r+1) < NODETOL)';
                        {
                            this->Fscale(j) = sJ(j)/J(i);
                            j++;
                        }
                    }
                }
            };

            template<typename SimpleType, typename DEElementStore = DG2DNoElementStore>
            class Face2D: public std::enable_shared_from_this<Face2D<SimpleType, DEElementStore>>
            {
            public:
                std::weak_ptr<Element2D<SimpleType, DEElementStore>> Element;
                std::weak_ptr<Edge2D<SimpleType, DEElementStore>> Edge;

                std::vector<int> NodesOnFace;

                std::shared_ptr<Face2D<SimpleType, DEElementStore>> GetOtherSideFace()
                {
                    std::shared_ptr<Face2D<SimpleType, DEElementStore>> t = this->shared_from_this();
                    std::shared_ptr<Edge2D<SimpleType, DEElementStore>> e = Edge.lock();
                    for(int i = 0; i < e->Faces.size(); i++)
                    {
                        if(e->Faces[i].lock() != t)
                        {
                            return e->Faces[i].lock();
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

                VectorX<SimpleType> GetValuesOfNodes(int i)
                {
                    VectorX<SimpleType> result(NodesOnFace.size());
                    auto e = Element.lock();
                    for (int j = 0; j < NodesOnFace.size(); ++j)
                    {
                        result[j] = e->u[i](NodesOnFace[j]);
                    }
                    return result;
                }

                VectorX<SimpleType> GetValuesOfOtherSide(int i)
                {
                    if (OtherSideExist())
                    {
                        return GetOtherSideFace()->GetValuesOfNodes(i);
                    }
                    else
                    {
                        return VectorX<SimpleType>::Constant(NodesOnFace.size(), nan(""));
                    }
                }
            };

            template<typename SimpleType, typename DEElementStore = DG2DNoElementStore>
            class Edge2D: public std::enable_shared_from_this<Edge2D<SimpleType, DEElementStore>>
            {
            public:
                Edge2D(std::weak_ptr<Vertex2D<SimpleType, DEElementStore>> v1, std::weak_ptr<Vertex2D<SimpleType, DEElementStore>> v2)
                {
                    this->v1 = v1;
                    this->v2 = v2;
                }

                void RegisterEdgeWithVertices()
                {
                    auto v1s = this->v1.lock();
                    v1s->Edges.push_back(this->shared_from_this());
                    auto v2s = this->v2.lock();
                    v2s->Edges.push_back(this->shared_from_this());
                }

                std::vector<std::weak_ptr<Face2D<SimpleType, DEElementStore>>> Faces;

                std::weak_ptr<Vertex2D<SimpleType, DEElementStore>> v1;
                std::weak_ptr<Vertex2D<SimpleType, DEElementStore>> v2;

                bool IsConnectedTo(std::weak_ptr<Vertex2D<SimpleType, DEElementStore>> v)
                {
                    return v.lock() == v1.lock() || v.lock() == v2.lock();
                }
            };

            template<typename SimpleType, typename DEElementStore = DG2DNoElementStore>
            class Vertex2D: public std::enable_shared_from_this<Vertex2D<SimpleType, DEElementStore>>
            {
            public:
                VectorX<SimpleType> Position;
                std::vector<std::weak_ptr<Edge2D<SimpleType, DEElementStore>>> Edges;

                Vertex2D(SimpleType x, SimpleType y)
                    :Position(2)
                {
                    Position << x, y;
                }

                std::shared_ptr<Edge2D<SimpleType, DEElementStore>> FindEdgeToOrCreate(
                        std::shared_ptr<System2D<SimpleType, DEElementStore>> system,
                        std::shared_ptr<Vertex2D<SimpleType, DEElementStore>> v)
                {
                    for (int i = 0; i < Edges.size(); ++i)
                    {
                        if (Edges[i].lock()->IsConnectedTo(v))
                        {
                            return Edges[i].lock();
                        }
                    }
                    //no edges are found
                    //create new edge
                    std::shared_ptr<Edge2D<SimpleType, DEElementStore>> result = std::make_shared<Edge2D<SimpleType, DEElementStore>>(
                            this->shared_from_this(), v);
                    result->RegisterEdgeWithVertices();
                    system->Edges.push_back(result);
                    return result;
                };

                std::shared_ptr<Edge2D<SimpleType, DEElementStore>> FindEdgeTo(std::shared_ptr<Vertex2D<SimpleType, DEElementStore>> v)
                {
                    for (int i = 0; i < Edges.size(); ++i)
                    {
                        if(Edges[i].lock()->IsConnectedTo(v))
                        {
                            return Edges[i].lock();
                        }
                    }
                    return std::shared_ptr<Edge2D<SimpleType, DEElementStore>>();
                };
            };
        }
    }
}
#endif //OPENPSTD_DG2D_H
