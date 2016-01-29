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
// Date: 24-9-2015
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_EDGES_H
#define OPENPSTD_EDGES_H

#include <vector>
#include <QVector2D>

namespace OpenPSTD
{
    namespace GUI
    {
/**
 * This class is specificaly for horizontal edges and vertical edges.
 * No fields are added to this class. It is possible to send an object of this type directly to the GPU and draw lines.
 */
        class Edge
        {
        private:
            QVector2D _start, _end;
            float _absorption;
            bool _localReacting;

            float StartInDim() const;

            float EndInDim() const;

            Edge CreateInSameDim(float start, float end) const;

            float PosInDim(int dim) const;

        public:
            Edge(float PosInDim, int dim, float start, float end, float absorption, bool localReacting);

            Edge(QVector2D start, QVector2D end, float absorption, bool localReacting);

            bool IsOnlyInDim(int dim) const;

            bool IsVertical() const;

            bool IsHorizontal() const;

            QVector2D GetStart() const;

            QVector2D GetEnd() const;

            float GetAbsorption() const;

            float GetLocalReacting() const;

            /**
             * Tests if the 2 edges are on the same line. This only works if they are on horizontal of vertical.
             */
            bool OnSameLine(const Edge &edge) const;

            static bool OnSameLine(const Edge &edge1, const Edge &edge2);

            std::vector<Edge> Substract(const Edge &edge) const;

            static std::vector<Edge> SubstractEdgeFromList(std::vector<Edge> edges, Edge rhs);

            static std::vector<Edge> SubstractListFromList(std::vector<Edge> lhs, std::vector<Edge> rhs);

            static std::vector<Edge> SubstractListFromEdge(Edge lhs, std::vector<Edge> rhs);

            static std::vector<Edge> SubstractListFromEdge(Edge lhs, std::vector<Edge> rhs,
                                                           std::vector<int> exceptIndices);
        };

    }
}

#endif //OPENPSTD_EDGES_H
