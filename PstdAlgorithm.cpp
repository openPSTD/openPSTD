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
// along with openPSTD.  If not, see <http://www.gnu.org/licenses/\>.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Date:
//
//
// Authors:
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////
//
// Created by michiel on 31-8-2015.
//

#include "PstdAlgorithm.h"

bool PointInSquare(QVector2D tl, QVector2D size, QVector2D point)
{
    QVector2D br = tl+size;
    if(((tl[0] < point[0] && point[0] < br[0]) || (br[0] < point[0] && point[0] < tl[0]))
                && ((tl[1] < point[1] && point[1] < br[1]) || (br[1] < point[1] && point[1] < tl[1])))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void AddSquareBuffer(std::unique_ptr<std::vector<float>> const &buffer, QVector2D tl, QVector2D size)
{
    float left = tl[0];
    float top = tl[1];

    float right = tl[0]+size[0];
    float bottom = tl[1]+size[1];

    buffer->push_back(left); buffer->push_back(top);
    buffer->push_back(right); buffer->push_back(top);

    buffer->push_back(left); buffer->push_back(top);
    buffer->push_back(left); buffer->push_back(bottom);

    buffer->push_back(left); buffer->push_back(bottom);
    buffer->push_back(right); buffer->push_back(bottom);

    buffer->push_back(right); buffer->push_back(top);
    buffer->push_back(right); buffer->push_back(bottom);
}

void AddSquareBuffer(std::unique_ptr<std::vector<QVector2D>> const &buffer, QVector2D tl, QVector2D size)
{
    float left = tl[0];
    float top = tl[1];

    float right = tl[0]+size[0];
    float bottom = tl[1]+size[1];

    buffer->push_back(QVector2D(left, top));
    buffer->push_back(QVector2D(right, top));

    buffer->push_back(QVector2D(left, top));
    buffer->push_back(QVector2D(left, bottom));

    buffer->push_back(QVector2D(left, bottom));
    buffer->push_back(QVector2D(right, bottom));

    buffer->push_back(QVector2D(right, top));
    buffer->push_back(QVector2D(right, bottom));
}