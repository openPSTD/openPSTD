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

#include "GUI/operations/EditOperations.h"
#include "MouseCreateSpeakerReceiverStrategy.h"

using namespace OpenPSTD::GUI;

MouseCreateSpeakerReceiverStrategy::MouseCreateSpeakerReceiverStrategy(OpenPSTD::GUI::PstdObjectType type) : _type(type)
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
    this->operationRunner.lock()->RunOperation(op);
}