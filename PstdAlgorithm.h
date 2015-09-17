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

#ifndef OPENPSTD_ALGORITHM_H
#define OPENPSTD_ALGORITHM_H
#include <QVector2D>
#include <vector>
#include <memory>

bool PointInSquare(QVector2D tl, QVector2D size, QVector2D point);

void AddSquareBuffer(std::unique_ptr<std::vector<float>> const &buffer, QVector2D tl, QVector2D size);
void AddSquareBuffer(std::unique_ptr<std::vector<QVector2D>> const &buffer, QVector2D tl, QVector2D size);

#endif //OPENPSTD_ALGORITHM_H
