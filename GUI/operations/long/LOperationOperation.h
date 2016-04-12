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
// Date: 12-4-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_LOPERATIONOPERATION_H
#define OPENPSTD_LOPERATIONOPERATION_H

#include "../BaseOperation.h"
#include "LongOperationRunner.h"
namespace OpenPSTD
{
    namespace GUI
    {
        class StartLOperation : public BaseOperation
        {
        private:
            std::shared_ptr<LongOperation> LOp;

        public:
            StartLOperation(std::shared_ptr<LongOperation> LOp);

            void Run(const Reciever &r);
        };
    }
}



#endif //OPENPSTD_LOPERATIONOPERATION_H
