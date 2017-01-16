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
// Date: 23-11-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_DG2DBUILDERS_H
#define OPENPSTD_DG2DBUILDERS_H

#include <Eigen/Dense>
#include <memory>
#include <vector>
#include "DG2D.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        namespace DG
        {
            /*template<typename SimpleType, typename DEElementStore = DG2DNoElementStore>
            class DG2DBuilder
            {
            public:
                virtual std::shared_ptr<System2D<SimpleType, DEElementStore> > Build(int N, std::shared_ptr<DG2DDE<SimpleType, DEElementStore> > de) = 0;
            };*/

            template<typename SimpleType, typename DEElementStore = DG2DNoElementStore>
            class SimpleDG2DBuilder
            {
            public:
                std::vector<OpenPSTD::Kernel::DG::VectorX<SimpleType>> Vertices;
                std::vector<std::vector<int>> Elements;

                std::shared_ptr<OpenPSTD::Kernel::DG::System2D<SimpleType, DEElementStore>> Build(
                        int N, std::shared_ptr<OpenPSTD::Kernel::DG::DG2DDE<SimpleType, DEElementStore>> de)
                {
                    std::shared_ptr<System2D<SimpleType, DEElementStore>> system = std::make_shared<System2D<SimpleType, DEElementStore>>(N, de);
                    for (int i = 0; i < Vertices.size(); ++i)
                    {
                        auto v = std::make_shared<Vertex2D<SimpleType, DEElementStore>>(Vertices[i]);
                        system->Vertices.push_back(v);
                    }

                    for (int i = 0; i < Elements.size(); ++i)
                    {
                        std::vector<std::shared_ptr<Vertex2D<SimpleType, DEElementStore>>> v;
                        for (int j = 0; j < Elements[i].size(); ++j)
                        {
                            v.push_back(system->Vertices[Elements[i][j]]);
                        }
                        auto element = std::make_shared<Element2D<SimpleType, DEElementStore>>(v);
                        element->index = i;
                        system->Element.push_back(element);
                    }

                    system->InitializeElements();
                    return system;
                }
            };

            template<typename SimpleType, typename DEElementStore = DG2DNoElementStore>
            class SquareGridBuilder
            {
            private:
                VectorX<SimpleType> _x1;
                VectorX<SimpleType> _x2;
                SimpleType _rectSize;
            public:
                SquareGridBuilder(VectorX<SimpleType> x1, VectorX<SimpleType> x2, SimpleType rectSize) :
                        _x1(x1), _x2(x2), _rectSize(rectSize)
                {

                }

                std::shared_ptr<OpenPSTD::Kernel::DG::System2D<SimpleType, DEElementStore>> Build(
                        int N, std::shared_ptr<OpenPSTD::Kernel::DG::DG2DDE<SimpleType, DEElementStore>> de)
                {
                    std::shared_ptr<System2D<SimpleType, DEElementStore>> system =
                            std::make_shared<System2D<SimpleType, DEElementStore>>(N, de);

                    //build "unstructured" grid
                    int widthElements = (_x2[0] - _x1[0]) / _rectSize;
                    int heightElements = (_x2[1] - _x1[1]) / _rectSize;

                    int widthVertices = widthElements + 1;
                    int heightVertices = heightElements + 1;

                    for (int j = 0; j < heightVertices; ++j)
                    {
                        for (int i = 0; i < widthVertices; ++i)
                        {
                            auto v = std::make_shared<Vertex2D<SimpleType, DEElementStore>>(_x1[0] + i * _rectSize,
                                                                                            _x1[1] + j * _rectSize);
                            system->Vertices.push_back(v);
                        }
                    }

                    int index = 0;

                    for (int j = 0; j < heightElements; ++j)
                    {
                        for (int i = 0; i < widthElements; ++i)
                        {
                            std::vector<std::shared_ptr<Vertex2D<SimpleType, DEElementStore>>> v;
                            v.push_back(system->Vertices[widthVertices * (j + 0) + (i + 0)]);
                            v.push_back(system->Vertices[widthVertices * (j + 0) + (i + 1)]);
                            v.push_back(system->Vertices[widthVertices * (j + 1) + (i + 1)]);
                            auto tr = std::make_shared<Element2D<SimpleType, DEElementStore>>(v);
                            tr->index = index++;
                            system->Element.push_back(tr);

                            v.clear();
                            v.push_back(system->Vertices[widthVertices * (j + 0) + (i + 0)]);
                            v.push_back(system->Vertices[widthVertices * (j + 1) + (i + 1)]);
                            v.push_back(system->Vertices[widthVertices * (j + 1) + (i + 0)]);
                            auto bl = std::make_shared<Element2D<SimpleType, DEElementStore>>(v);
                            bl->index = index++;
                            system->Element.push_back(bl);
                        }
                    }

                    system->InitializeElements();
                    return system;
                }
            };


            extern template class SimpleDG2DBuilder<double>;
            extern template class SimpleDG2DBuilder<float>;

            extern template class SquareGridBuilder<double>;
            extern template class SquareGridBuilder<float>;
        }
    }
}

#endif //OPENPSTD_DG2DBUILDERS_H
