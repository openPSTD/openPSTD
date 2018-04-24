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

#include "ApplicationSettings.h"

using namespace OpenPSTD::GUI;

ApplicationSettings::ApplicationSettings(QWidget *parent): QDialog(parent), ui(new Ui_ApplicationSettings())
{
    ui->setupUi(this);
}

void ApplicationSettings::UpdateFromModel(std::shared_ptr<Model> const &model)
{
    if(model->settings->IsChanged())
    {
        ui->rbCPU->setChecked(!model->settings->CPUAcceleration&&!model->settings->GPUAcceleration);
        ui->rbMCPU->setChecked(model->settings->CPUAcceleration);
        ui->rbGPU->setChecked(model->settings->GPUAcceleration);
        ui->cbUseMockKernel->setChecked(model->settings->UseMockKernel);
    }
}

void ApplicationSettings::UpdateToModel(std::shared_ptr<Model> const &model)
{
    model->settings->UseMockKernel = ui->cbUseMockKernel->isChecked();
    model->settings->CPUAcceleration = ui->rbMCPU->isChecked();
    model->settings->GPUAcceleration = ui->rbGPU->isChecked();
}



