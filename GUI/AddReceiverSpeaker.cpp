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
// Date: 19-5-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#include "AddReceiverSpeaker.h"
namespace OpenPSTD
{
    namespace GUI
    {
        AddReceiverSpeaker::AddReceiverSpeaker(QWidget *parent): QDialog(parent), ui(new Ui_AddReceiverSpeaker())
        {
            ui->setupUi(this);
        }

        CreateReceiverSpeakerOperation::Options AddReceiverSpeaker::GetOptions()
        {
            return CreateReceiverSpeakerOperation::Options(
                    ui->cbType->currentIndex() == 0?OBJECT_RECEIVER:OBJECT_SPEAKER,
                    QVector3D((float)ui->dsbX->value(), (float)ui->dsbY->value(), 0),
                    ui->sbCount->value(),
                    (float)ui->dsbDirection->value(),
                    (float)ui->dsbDistanceBetween->value());
        }
    }
}

