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
// Date: 17-09-2015
//
//
// Authors: M. R. Fortuin
//
//
//////////////////////////////////////////////////////////////////////////

#include "MouseHandlers.h"
#include "operations/ViewOperations.h"
#include "operations/EditOperations.h"
#include "operations/SelectionOperations.h"
#include <iostream>
#include "Snapping.h"
#include "PstdAlgorithm.h"

namespace OpenPSTD
{
    namespace GUI
    {
        void MouseStrategy::SetOperationRunner(std::shared_ptr<OperationRunner> operationRunner)
        {
            this->operationRunner = operationRunner;
        }

        void MouseSelectStrategy::mouseReleaseEvent(std::shared_ptr<Model> const &model, QMouseEvent *mouseEvent,
                                                    QVector2D pos)
        {
            std::shared_ptr<SelectObjectOperation> op = std::make_shared<SelectObjectOperation>(pos);
            operationRunner->RunOperation(op);
        }

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
                operationRunner->RunOperation(op);
            }
        }

        void MouseMoveSceneStrategy::wheelEvent(std::shared_ptr<Model> const &model, QWheelEvent *mouseEvent,
                                                QVector2D pos)
        {
            pos = (model->view->aspectMatrix.inverted() * pos.toVector3D()).toVector2D();
            float delta = mouseEvent->delta();
            float delta2 = powf(2, delta / 120);

            std::shared_ptr<ResizeScene> op = std::make_shared<ResizeScene>(delta2, pos);
            operationRunner->RunOperation(op);
        }

        void MouseMoveSceneStrategy::mousePressEvent(std::shared_ptr<Model> const &model, QMouseEvent *event,
                                                     QVector2D pos)
        {
            pos = (model->view->aspectMatrix.inverted() * pos.toVector3D()).toVector2D();
            this->mousePos = pos;
        }


        void MouseCreateDomainStrategy::mousePressEvent(std::shared_ptr<Model> const &model, QMouseEvent *,
                                                        QVector2D pos)
        {
            QVector2D start = (model->view->viewMatrix.inverted() * pos.toVector3D()).toVector2D();

            start = Snapping::Snap(model, start);

            model->interactive->CreateDomain.start = start;
            model->interactive->CreateDomain.currentEnd = start;
            model->interactive->CreateDomain.visible = true;
            model->interactive->Change();
        }

        void MouseCreateDomainStrategy::mouseMoveEvent(std::shared_ptr<Model> const &model, QMouseEvent *mouseEvent,
                                                       QVector2D pos)
        {
            QVector2D end = (model->view->viewMatrix.inverted() * pos.toVector3D()).toVector2D();

            end = Snapping::Snap(model, end);

            model->interactive->CreateDomain.currentEnd = end;
            model->interactive->Change();
        }

        void MouseCreateDomainStrategy::mouseReleaseEvent(std::shared_ptr<Model> const &model, QMouseEvent *mouseEvent,
                                                          QVector2D pos)
        {
            QVector2D end = (model->view->viewMatrix.inverted() * pos.toVector3D()).toVector2D();

            end = Snapping::Snap(model, end);

            model->interactive->CreateDomain.currentEnd = end;

            model->interactive->CreateDomain.visible = false;
            model->interactive->Change();
            std::shared_ptr<CreateDomainOperation> op = std::make_shared<CreateDomainOperation>(
                    model->interactive->CreateDomain.start,
                    model->interactive->CreateDomain.currentEnd);
            this->operationRunner->RunOperation(op);
        }

        MouseCreateSpeakerReceiverStrategy::MouseCreateSpeakerReceiverStrategy(PstdObjectType type) : _type(type)
        {

        }

        void MouseCreateSpeakerReceiverStrategy::mouseMoveEvent(std::shared_ptr<Model> const &model,
                                                                QMouseEvent *mouseEvent,
                                                                QVector2D pos)
        {

        }

        void MouseCreateSpeakerReceiverStrategy::mouseReleaseEvent(std::shared_ptr<Model> const &model,
                                                                   QMouseEvent *mouseEvent,
                                                                   QVector2D pos)
        {
            std::shared_ptr<CreateReceiverSpeakerOperation> op = std::make_shared<CreateReceiverSpeakerOperation>(
                    this->_type, (model->view->viewMatrix.inverted() * pos.toVector3D()).toVector2D());
            this->operationRunner->RunOperation(op);
        }
    }
}
