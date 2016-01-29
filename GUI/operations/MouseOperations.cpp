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
// Created by michiel on 24-8-2015.
//

#include "MouseOperations.h"

namespace OpenPSTD
{
    namespace GUI
    {
        ChangeMouseHandlerOperations::ChangeMouseHandlerOperations(std::unique_ptr<MouseStrategy> mouseHandler) :
                mouseHandler(std::move(mouseHandler))
        {

        }

        void ChangeMouseHandlerOperations::Run(const Reciever &reciever)
        {
            reciever.model->mouseHandler = std::move(this->mouseHandler);
            reciever.model->mouseHandler->SetOperationRunner(reciever.operationRunner);
        }

    }
}