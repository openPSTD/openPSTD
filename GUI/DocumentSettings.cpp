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
//////////////////////////////////////////////////////////////////////////

//
// Created by michiel on 5-9-2015.
//

#include "DocumentSettings.h"
#include <iostream>

namespace OpenPSTD
{
    namespace GUI
    {

        DocumentSettings::DocumentSettings(QWidget *parent) : QDialog(parent), ui(new Ui_DocumentSettings())
        {
            ui->setupUi(this);
        }

        void DocumentSettings::UpdateFromModel(std::shared_ptr<Model> const &model)
        {
            Kernel::PSTDSettings settings = model->d->GetSceneConf()->Settings;

            ui->sbGridSpacing->setValue(settings.GetGridSpacing());
            ui->sbPatchError->setValue(settings.GetPatchError());
            ui->sbRenderTime->setValue(settings.GetRenderTime());
            ui->sbPMLCells->setValue(settings.GetPMLCells());
            ui->sbAttenuationOfPMLCells->setValue(settings.GetAttenuationOfPMLCells());
            ui->sbDensity->setValue(settings.GetDensityOfAir());
            ui->sbSoundSpeed->setValue(settings.GetSoundSpeed());
            ui->sbCFLNumerRKScheme->setValue(settings.GetFactRK());
            ui->sbSaveEveryNth->setValue(settings.GetSaveNth());
        }

        Kernel::PSTDSettings DocumentSettings::GetResult()
        {
            Kernel::PSTDSettings settings;

            settings.SetGridSpacing(ui->sbGridSpacing->value());
            settings.SetPatchError(ui->sbPatchError->value());
            settings.SetRenderTime(ui->sbRenderTime->value());
            settings.SetPMLCells(ui->sbPMLCells->value());
            settings.SetAttenuationOfPMLCells(ui->sbAttenuationOfPMLCells->value());
            settings.SetDensityOfAir(ui->sbDensity->value());
            settings.SetSoundSpeed(ui->sbSoundSpeed->value());
            settings.SetFactRK(ui->sbCFLNumerRKScheme->value());
            settings.SetSaveNth(ui->sbSaveEveryNth->value());

            return settings;
        }
    }
}
