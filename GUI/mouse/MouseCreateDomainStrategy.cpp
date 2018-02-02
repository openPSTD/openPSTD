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

#include <GUI/Snapping.h>
#include "GUI/operations/EditOperations.h"
#include "MouseCreateDomainStrategy.h"


using namespace OpenPSTD::GUI;

void MouseCreateDomainStrategy::mousePressEvent(std::shared_ptr<OpenPSTD::GUI::Model> const &model, QMouseEvent *,
                                                QVector2D pos)
{
    QVector2D start = (model->view->viewMatrix.inverted() * pos.toVector3D()).toVector2D();

    start = Snapping::Snap(model, start);

    model->interactive->CreateDomain.start = start;
    model->interactive->CreateDomain.currentEnd = start;
    model->interactive->CreateDomain.visible = true;
    model->interactive->Change();
}

void MouseCreateDomainStrategy::mouseMoveEvent(std::shared_ptr<OpenPSTD::GUI::Model> const &model, QMouseEvent *mouseEvent,
                                               QVector2D pos)
{
    QVector2D end = (model->view->viewMatrix.inverted() * pos.toVector3D()).toVector2D();

    end = Snapping::Snap(model, end);

    model->interactive->CreateDomain.currentEnd = end;
    model->interactive->Change();
}

void MouseCreateDomainStrategy::mouseReleaseEvent(std::shared_ptr<OpenPSTD::GUI::Model> const &model, QMouseEvent *mouseEvent,
                                                  QVector2D pos)
{
    QVector2D end = (model->view->viewMatrix.inverted() * pos.toVector3D()).toVector2D();

    end = Snapping::Snap(model, end);

    model->interactive->CreateDomain.currentEnd = end;

    model->interactive->CreateDomain.visible = false;
    model->interactive->Change();
    std::shared_ptr<OpenPSTD::GUI::CreateDomainOperation> op = std::make_shared<OpenPSTD::GUI::CreateDomainOperation>(
            model->interactive->CreateDomain.start,
            model->interactive->CreateDomain.currentEnd);
    this->operationRunner.lock()->RunOperation(op);
}