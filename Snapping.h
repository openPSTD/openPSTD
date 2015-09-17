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
// Created by michiel on 30-8-2015.
//

#ifndef OPENPSTD_SNAPPING_H
#define OPENPSTD_SNAPPING_H

#include <QVector2D>
#include <vector>
#include "Model.h"

class Snapping
{
private:
    static std::unique_ptr<std::vector<float>> GetEdges(std::shared_ptr<Model> const &model, int dimension);
public:
    static QVector2D Snap(std::shared_ptr<Model> const &model, QVector2D vector);
};


#endif //OPENPSTD_SNAPPING_H
