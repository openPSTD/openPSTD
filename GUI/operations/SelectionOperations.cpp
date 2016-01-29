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
// Date: 31-8-2015
//
//
// Authors: M. R. Fortuin
//
//
//////////////////////////////////////////////////////////////////////////


#include "SelectionOperations.h"
#include <iostream>
#include "../PstdAlgorithm.h"

namespace OpenPSTD
{
    namespace GUI
    {

        SelectIndexedObjectOperation::SelectIndexedObjectOperation(SelectionType type, int index) :
                type(type), index(index)
        {

        }

        void SelectIndexedObjectOperation::Run(const Reciever &reciever)
        {
            reciever.model->interactive->Selection.Type = type;
            reciever.model->interactive->Selection.SelectedIndex = index;
            reciever.model->interactive->Change();
        }

        DeselectOperation::DeselectOperation() : SelectIndexedObjectOperation(SELECTION_NONE, -1)
        { }

        SelectDomainOperation::SelectDomainOperation(int selectDomainIndex) : SelectIndexedObjectOperation(
                SELECTION_DOMAIN,
                selectDomainIndex)
        { }

        SelectObjectOperation::SelectObjectOperation(QVector2D ScreenPosition) : ScreenPosition(ScreenPosition)
        { }

        void SelectObjectOperation::Run(const Reciever &reciever)
        {
            QVector2D mousePos = (reciever.model->view->viewMatrix.inverted() *
                                  this->ScreenPosition.toVector3D()).toVector2D();
            auto conf = reciever.model->d->GetSceneConf();

            for (int i = 0; i < conf->Receivers.size(); i++)
            {
                QVector2D receiverPos(conf->Receivers[i][0], conf->Receivers[i][1]);
                if (receiverPos.distanceToPoint(mousePos) < 0.4f)
                {
                    reciever.operationRunner->RunOperation(std::shared_ptr<SelectIndexedObjectOperation>(
                            new SelectIndexedObjectOperation(SELECTION_RECEIVER, i)));
                    return;
                }
            }


            for (int i = 0; i < conf->Speakers.size(); i++)
            {
                QVector2D speakersPos(conf->Speakers[i][0], conf->Speakers[i][1]);
                if (speakersPos.distanceToPoint(mousePos) < 0.4f)
                {
                    reciever.operationRunner->RunOperation(std::shared_ptr<SelectIndexedObjectOperation>(
                            new SelectIndexedObjectOperation(SELECTION_SPEAKER, i)));
                    return;
                }
            }

            for (int i = 0; i < conf->Domains.size(); i++)
            {
                QVector2D tl = conf->Domains[i].TopLeft;
                QVector2D size = conf->Domains[i].Size;

                if (PointInSquare(tl, size, mousePos))
                {
                    //execute operation that selects the correct index
                    reciever.operationRunner->RunOperation(std::shared_ptr<SelectIndexedObjectOperation>(
                            new SelectIndexedObjectOperation(SELECTION_DOMAIN, i)));
                    return;
                }
            }
            //deselects the domain
            std::shared_ptr<DeselectOperation> op1(new DeselectOperation());
            reciever.operationRunner->RunOperation(op1);
        }
    }
}