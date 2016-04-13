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
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_BASEOPERATION_H
#define OPENPSTD_BASEOPERATION_H

#include <memory>
#include "Reciever.h"

namespace OpenPSTD
{
    namespace GUI
    {
        class BaseOperation
        {
        public:
            virtual void Run(const Reciever &reciever) = 0;
        };

        class EmptyOperation: public BaseOperation
        {
        public:
            virtual void Run(const Reciever &reciever);
        };

        class OperationRunner
        {
        public:
            virtual void RunOperation(std::shared_ptr<BaseOperation> operation) = 0;
        };

        class ReceiverBuilder
        {
        public:
            virtual Reciever BuildReceiver() = 0;
        };
    }
}

#endif //OPENPSTD_BASEOPERATION_H
