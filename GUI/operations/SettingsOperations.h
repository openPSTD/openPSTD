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

#ifndef OPENPSTD_SETTINGSOPERATIONS_H
#define OPENPSTD_SETTINGSOPERATIONS_H

#include "BaseOperation.h"
#include "../ApplicationSettings.h"
namespace OpenPSTD
{
    namespace GUI
    {
        class UpdateSettingsOperation : public BaseOperation
        {
        private:
            std::shared_ptr<ApplicationSettings> ui;
        public:
            UpdateSettingsOperation(std::shared_ptr<ApplicationSettings> ui);

            void Run(const Reciever &reciever);
        };
    }
}

#endif //OPENPSTD_SETTINGSOPERATIONS_H
