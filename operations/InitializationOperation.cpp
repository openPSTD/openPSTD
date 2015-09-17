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
//////////////////////////////////////////////////////////////////////////

//
// Created by michiel on 17-8-2015.
//

#include "InitializationOperation.h"
#include "MouseOperations.h"
#include "SelectDomainOperation.h"

void InitializationOperation::Run(const Reciever &reciever)
{
    //todo: make sure an no file is loaded
    reciever.model->d = std::unique_ptr<PSTDFile>(PSTDFile::New("test.jps"));
    reciever.model->d->Change();
    reciever.model->Register(reciever.model->d);

    reciever.model->view->aspectMatrix = QMatrix4x4();
    reciever.model->view->viewMatrix = QMatrix4x4();
    reciever.model->view->worldMatrix = QMatrix4x4();
    reciever.model->view->Change();

    reciever.model->settings->visual.colorScheme = std::unique_ptr<StandardColorScheme>(new StandardColorScheme());
    reciever.model->settings->Change();

    //select none of the domains
    std::shared_ptr<DeselectOperation> op1(new DeselectOperation());
    reciever.operationRunner->RunOperation(op1);

    //initialize Mouse handler
    std::shared_ptr<ChangeMouseHandlerOperations> op2(new ChangeMouseHandlerOperations(std::unique_ptr<MouseMoveSceneStrategy>(new MouseMoveSceneStrategy())));
    reciever.operationRunner->RunOperation(op2);
}