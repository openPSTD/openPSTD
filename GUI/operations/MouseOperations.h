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
// Created by michiel on 24-8-2015.
//

#ifndef OPENPSTD_MOUSEOPERATIONS_H
#define OPENPSTD_MOUSEOPERATIONS_H

namespace OpenPSTD
{
    namespace GUI
    {
        class ChangeMouseHandlerOperations;
    }
}

#include "BaseOperation.h"
#include "GUI/mouse/MouseStrategy.h"
#include "../Model.h"

namespace OpenPSTD
{
    namespace GUI
    {
        class ChangeMouseHandlerOperations : public BaseOperation
        {
        private:
            std::unique_ptr<MouseStrategy> mouseHandler;

        public:
            ChangeMouseHandlerOperations(std::unique_ptr<MouseStrategy> mouseHandler);

            virtual void Run(const Reciever &reciever) override;
        };
    }
}

#endif //OPENPSTD_MOUSEOPERATIONS_H
