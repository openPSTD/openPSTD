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

#ifndef OPENPSTD_RK_H
#define OPENPSTD_RK_H

#include <memory>
#include <vector>

#include "GeneralTypes.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            template<typename SimpleType>
            class BlackBoxInterface
            {
            public:
                virtual std::vector<MatrixX<SimpleType> > ComputeRHS(SimpleType time) = 0;
                virtual void SetState(std::vector<MatrixX<SimpleType>> state) = 0;
                virtual std::vector<MatrixX<SimpleType> > GetState() = 0;
                virtual SimpleType GetMaxDT() = 0;
            };

            template<typename SimpleType>
            class RK
            {
            protected:
                std::shared_ptr<BlackBoxInterface<SimpleType>> bb;
            public:
                virtual void SetBB(std::shared_ptr<BlackBoxInterface<SimpleType>> bb){this->bb = bb;};
                virtual void ComputeTimeStep(SimpleType deltaTime) = 0;
                virtual void Calculate(SimpleType FinalTime) = 0;
            };

            template<typename SimpleType>
            class LSERK: public RK<SimpleType>
            {
            private:
                SimpleType Time;
                std::vector<MatrixX<SimpleType>> resu;

                struct RKC {
                public:
                    SimpleType a;
                    SimpleType b;
                    SimpleType c;
                };

                std::vector<RKC> RKConstants;

                void InitializeResU()
                {
                    this->resu.clear();
                    auto u = this->bb->GetState();
                    for(int i = 0; i < u.size(); i++)
                    {
                        auto resu = MatrixX<SimpleType>::Zero(u[i].rows(), u[i].cols());
                        this->resu.push_back(resu);
                    }
                }

            public:
                bool outputMatlab;

                LSERK(): Time(0)
                {
                    RKC tmp;

                    //RK4 constants
                    tmp.a = 0;
                    tmp.b = 1432997174477.0/9575080441755.0;
                    tmp.c = 0;
                    RKConstants.push_back(tmp);

                    tmp.a = -567301805773.0/1357537059087.0;
                    tmp.b = 5161836677717.0/13612068292357.0;
                    tmp.c = 1432997174477.0/9575080441755.0;
                    RKConstants.push_back(tmp);

                    tmp.a = -2404267990393.0/2016746695238.0;
                    tmp.b = 1720146321549.0/2090206949498.0;
                    tmp.c = 2526269341429.0/6820363962896.0;
                    RKConstants.push_back(tmp);

                    tmp.a = -3550918686646.0/2091501179385.0;
                    tmp.b = 3134564353537.0/4481467310338.0;
                    tmp.c = 2006345519317.0/3224310063776.0;
                    RKConstants.push_back(tmp);

                    tmp.a = -1275806237668.0/842570457699.0;
                    tmp.b = 2277821191437.0/14882151754819.0;
                    tmp.c = 2802321613138.0/2924317926251.0;
                    RKConstants.push_back(tmp);
                }

                void ComputeRKStep(int RKi, SimpleType deltaTime)
                {
                    SimpleType a = this->RKConstants[RKi].a;
                    SimpleType b = this->RKConstants[RKi].b;
                    SimpleType c = this->RKConstants[RKi].c;

                    //computing the RHS and getting the current state
                    auto RHS = this->bb->ComputeRHS(this->Time+c*deltaTime);
                    auto u = this->bb->GetState();

                    //initialize resu if necessary
                    if(resu.size() != u.size())
                        this->InitializeResU();

                    for(int i = 0; i < u.size(); i++)
                    {
                        //the acutal RK step
                        resu[i] = a*resu[i]+deltaTime*RHS[i];
                        u[i] = u[i]+b*resu[i];
                    }

                    //update the state
                    this->bb->SetState(u);
                }

                virtual void Calculate(SimpleType FinalTime)
                {
                    SimpleType dt = this->bb->GetMaxDT();
                    SimpleType Nsteps = ceil(FinalTime/dt);
                    dt = FinalTime/Nsteps;

                    for(int tstep = 0; tstep < Nsteps; tstep++)
                    {
                        this->ComputeTimeStep(dt);
                    }
                }

                virtual void ComputeTimeStep(SimpleType deltaTime)
                {
                    for(int RKi = 0; RKi < 5; RKi++)
                    {
                        ComputeRKStep(RKi, deltaTime);
                    }
                    this->Time += deltaTime;
                };
            };
        }
    }
}

#endif //OPENPSTD_RK_H
