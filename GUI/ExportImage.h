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
// Date: 20-4-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_EXPORTIMAGE_H
#define OPENPSTD_EXPORTIMAGE_H

#include <ui_ExportImage.h>
#include <memory>
#include <string>
#include "operations/FileOperations.h"

namespace OpenPSTD
{
    namespace GUI
    {
        class ExportImage : public QDialog
        {
        Q_OBJECT

        public:
            explicit ExportImage(int maxFrame, QWidget *parent = 0);

            int GetStartFrame();
            int GetEndFrame();
            std::string GetDirectory();
            std::string GetName();
            ImageFormat GetFormat();


        private:
            std::unique_ptr<Ui_ExportImage> ui;

            void UpdateEnabling();
            void UpdateEnablingI(int i) {UpdateEnabling();};
            void UpdateEnablingS(QString &text) {UpdateEnabling();};

            void Browse();
        };
    }
}


#endif //OPENPSTD_EXPORTIMAGE_H
