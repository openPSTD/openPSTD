//
// Created by michiel on 5-9-2015.
//

#include "DocumentSettings.h"
#include <iostream>


DocumentSettings::DocumentSettings(QWidget *parent): QDialog(parent), ui(new Ui_DocumentSettings())
{
    ui->setupUi(this);
}

void DocumentSettings::UpdateFromModel(std::shared_ptr<Model> const &model)
{
    PSTDFileSettings settings = model->d->GetSettings();

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

PSTDFileSettings DocumentSettings::GetResult()
{
    PSTDFileSettings settings;

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

