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

#include "Edges.h"
#include <algorithm>

Edge::Edge(float PosInDim, int dim, float start, float end, float absorption, bool localReacting):
        _absorption(absorption), _localReacting(localReacting)
{
    _start[dim] = PosInDim;
    _end[dim] = PosInDim;
    if(dim == 0)
    {
        _start[1] = start;
        _end[1] = end;
    }
    else
    {
        _start[0] = start;
        _end[0] = end;
    }
}
Edge::Edge(QVector2D start, QVector2D end, float absorption, bool localReacting): _start(start), _end(end),
                                                                                  _absorption(absorption),
                                                                                  _localReacting(localReacting)
{

}

bool Edge::IsOnlyInDim(int dim) const
{
    return _start[dim] == _end[dim];
}

bool Edge::IsVertical() const
{
    return this->IsOnlyInDim(0);
}
bool Edge::IsHorizontal() const
{
    return this->IsOnlyInDim(1);
}

QVector2D Edge::GetStart() const
{
    return this->_start;
}
QVector2D Edge::GetEnd() const
{
    return this->_end;
}
float Edge::GetAbsorption() const
{
    return this->_absorption;
}
float Edge::GetLocalReacting() const
{
    return this->_localReacting;
}

float Edge::PosInDim(int dim) const
{
    if(IsOnlyInDim(dim))
    {
        return this->_start[dim];
    }
    else
    {
        return NAN;
    }
}

bool Edge::OnSameLine(const Edge& edge) const
{
    for(int i = 0; i < 2; i++)
    {
        if(IsOnlyInDim(i) && edge.IsOnlyInDim(i))
        {
            return this->PosInDim(i) == edge.PosInDim(i);
        }
    }
    return false;
}
bool Edge::OnSameLine(const Edge& edge1, const Edge& edge2)
{
    return edge1.OnSameLine(edge2);
}

std::vector<Edge> Edge::Substract(const Edge& other) const
{
    std::vector<Edge> result;
    if(OnSameLine(other))
    {
        if(other.EndInDim() < this->StartInDim())
        { // they do not overlap
            result.push_back(Edge(_start, _end, _absorption, _localReacting));
        }
        else if(this->EndInDim() < other.StartInDim())
        { // they do not overlap
            result.push_back(Edge(_start, _end, _absorption, _localReacting));
        }
        else if(this->StartInDim() < other.StartInDim() && other.EndInDim() < this->EndInDim())
        { // split the edge
            result.push_back(CreateInSameDim(this->StartInDim(), other.StartInDim()));
            result.push_back(CreateInSameDim(other.EndInDim(), this->EndInDim()));
        }
        else if(other.StartInDim() < this->StartInDim() && this->EndInDim() < other.EndInDim())
        { // everything is removed

        }
        else if(other.EndInDim() < this->EndInDim())
        { // start must be trimmed
            result.push_back(CreateInSameDim(other.EndInDim(), this->EndInDim()));
        }
        else if(this->StartInDim() < other.StartInDim())
        { // start must be trimmed
            result.push_back(CreateInSameDim(this->StartInDim(), other.StartInDim()));
        }
    }
    else
    {
        result.push_back(*this);
    }
    return result;
}

float Edge::StartInDim() const
{
    // the indexes are inverted, this is the start in the vector space of the line.

    if(IsOnlyInDim(0))
    {
        return std::min(_start[1], _end[1]);
    }
    else if(IsOnlyInDim(1))
    {
        return std::min(_start[0], _end[0]);
    }

    return NAN;
}

float Edge::EndInDim() const
{
    // the indexes are inverted, this is the end in the vector space of the line.

    if(IsOnlyInDim(0))
    {
        return std::max(_start[1], _end[1]);
    }
    else if(IsOnlyInDim(1))
    {
        return std::max(_start[0], _end[0]);
    }
    return NAN;
}

Edge Edge::CreateInSameDim(float start, float end) const
{
    for (int i = 0; i < 2; ++i)
    {
        if(IsOnlyInDim(i))
        {
            return Edge(this->PosInDim(i), i, start, end, _absorption, _localReacting);
        }
    }
    //todo throw exception
    return Edge(QVector2D(NAN, NAN), QVector2D(NAN, NAN), NAN, false);
}

std::vector<Edge> Edge::SubstractEdgeFromList(std::vector<Edge> edges, Edge rhs)
{
    std::vector<Edge> result;
    for (int i = 0; i < edges.size(); ++i)
    {
        std::vector<Edge> tmp = edges[i].Substract(rhs);
        result.insert(result.end(), tmp.begin(), tmp.end());
    }
    return result;
}

std::vector<Edge> Edge::SubstractListFromList(std::vector<Edge> lhs, std::vector<Edge> rhs)
{
    std::vector<Edge> result;
    for (int i = 0; i < lhs.size(); ++i)
    {
        std::vector<Edge> tmp = SubstractListFromEdge(lhs[i], rhs);
        result.insert(result.end(), tmp.begin(), tmp.end());
    }
    return result;
}

std::vector<Edge> Edge::SubstractListFromEdge(Edge lhs, std::vector<Edge> rhs)
{
    std::vector<Edge> result;
    result.push_back(lhs);
    for (int i = 0; i < rhs.size(); ++i)
    {
        result = SubstractEdgeFromList(result, rhs[i]);
    }
    return result;
}

std::vector<Edge> Edge::SubstractListFromEdge(Edge lhs, std::vector<Edge> rhs, std::vector<int> exceptIndices)
{
    std::vector<Edge> result;
    result.push_back(lhs);
    for (int i = 0; i < rhs.size(); ++i)
    {
        if(!std::any_of(exceptIndices.begin(), exceptIndices.end(), [i](int except){ return except == i; }))
        {
            result = SubstractEdgeFromList(result, rhs[i]);
        }
    }
    return result;
}