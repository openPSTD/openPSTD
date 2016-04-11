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
// Date: 10-4-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#include <GUI/operations/ViewOperations.h>
#include "MouseMoveSceneStrategy.h"

using namespace OpenPSTD::GUI;

void MouseMoveSceneStrategy::mouseMoveEvent(std::shared_ptr<Model> const &model, QMouseEvent *mouseEvent,
                                            QVector2D pos)
{
    pos = (model->view->aspectMatrix.inverted() * pos.toVector3D()).toVector2D();
    QVector2D offset = pos - this->mousePos;
    this->mousePos = pos;

    auto buttons = mouseEvent->buttons();
    if (buttons & Qt::LeftButton)
    {
        std::shared_ptr<TranslateScene> op = std::make_shared<TranslateScene>(offset);
        MouseStrategy::operationRunner.lock()->RunOperation(op);
    }
}

void MouseMoveSceneStrategy::wheelEvent(std::shared_ptr<Model> const &model, QWheelEvent *mouseEvent,
                                        QVector2D pos)
{
    pos = (model->view->aspectMatrix.inverted() * pos.toVector3D()).toVector2D();
    float delta = mouseEvent->delta();
    float delta2 = powf(2, delta / 120);

    std::shared_ptr<ResizeScene> op = std::make_shared<ResizeScene>(delta2, pos);
    operationRunner.lock()->RunOperation(op);
}

void MouseMoveSceneStrategy::mousePressEvent(std::shared_ptr<Model> const &model, QMouseEvent *event,
                                             QVector2D pos)
{
    pos = (model->view->aspectMatrix.inverted() * pos.toVector3D()).toVector2D();
    this->mousePos = pos;
}