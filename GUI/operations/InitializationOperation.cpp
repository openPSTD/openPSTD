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
// Date: 17-8-2015
//
//
// Authors: M. R. Fortuin
//
//
//////////////////////////////////////////////////////////////////////////

//
// Created by michiel on 17-8-2015.
//

#include "InitializationOperation.h"
#include "MouseOperations.h"
#include "SelectionOperations.h"
#include "FileOperations.h"
#include "../mouse/MouseMoveSceneStrategy.h"

namespace OpenPSTD
{
    namespace GUI
    {
        void InitializationOperation::Run(const Reciever &reciever)
        {
            reciever.model->view->aspectMatrix = QMatrix4x4();
            reciever.model->view->viewMatrix = QMatrix4x4();
            reciever.model->view->worldMatrix = QMatrix4x4();
            reciever.model->view->Change();

            reciever.model->colorScheme = std::unique_ptr<Shared::StandardColorScheme>(
                    new Shared::StandardColorScheme());
            reciever.model->Change();

            //select none of the domains
            std::shared_ptr<DeselectOperation> op2(new DeselectOperation());
            reciever.operationRunner->RunOperation(op2);

            //initialize Mouse handler
            std::shared_ptr<ChangeMouseHandlerOperations> op3(new ChangeMouseHandlerOperations(
                    std::unique_ptr<MouseMoveSceneStrategy>(new MouseMoveSceneStrategy())));
            reciever.operationRunner->RunOperation(op3);
        }
    }
}
