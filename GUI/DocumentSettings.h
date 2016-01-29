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
// Created by michiel on 5-9-2015.
//

#ifndef OPENPSTD_DOCUMENTSETTINGS_H
#define OPENPSTD_DOCUMENTSETTINGS_H

#include <ui_DocumentSettings.h>
#include <memory>
#include "Model.h"
namespace OpenPSTD
{
    namespace GUI
    {
        class DocumentSettings : public QDialog
        {
        Q_OBJECT

        public:
            explicit DocumentSettings(QWidget *parent = 0);

            void UpdateFromModel(std::shared_ptr<Model> const &model);

            Kernel::PSTDSettings GetResult();

        private:
            std::unique_ptr<Ui_DocumentSettings> ui;
        };

    }
}

#endif //OPENPSTD_DOCUMENTSETTINGS_H
