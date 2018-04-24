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
// Date: 11-4-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_APPLICATIONSETTINS_H
#define OPENPSTD_APPLICATIONSETTINS_H

#include <ui_ApplicationSettings.h>
#include <memory>
#include <QtCore/QArgument>
#include <QtWidgets/QDialog>
#include "Model.h"

namespace OpenPSTD
{
    namespace GUI
    {
        class ApplicationSettings : public QDialog
        {
            Q_OBJECT
        public:
            explicit ApplicationSettings(QWidget *parent = 0);
            void UpdateFromModel(std::shared_ptr<Model> const &model);
            void UpdateToModel(std::shared_ptr<Model> const &model);
        private:
            std::unique_ptr<Ui_ApplicationSettings> ui;
        };
    }
}



#endif //OPENPSTD_APPLICATIONSETTINS_H
