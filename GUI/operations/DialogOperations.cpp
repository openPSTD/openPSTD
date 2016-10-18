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
// Date: 18-10-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#include "DialogOperations.h"
#include <QMessageBox>

using namespace OpenPSTD::GUI;

DialogOperations::DialogOperations(DialogOperationsSeverity severity, std::string text): _severity(severity),
                                                                                         _text(text)
{

}

void DialogOperations::Run(const Reciever &reciever)
{
    QMessageBox msgBox;
    msgBox.setText(QString::fromStdString(_text));
    //msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    switch(_severity)
    {
        case DialogOperationsSeverity::Warning:
            msgBox.setIcon(QMessageBox::Warning);
            break;
        case DialogOperationsSeverity::Information:
            msgBox.setIcon(QMessageBox::Information);
            break;
        case DialogOperationsSeverity::Critical:
            msgBox.setIcon(QMessageBox::Critical);
            break;
    }
    msgBox.exec();
}