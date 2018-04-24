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

#include "ExportImage.h"
#include <QFileDialog>
#include <QSpinBox>

namespace OpenPSTD
{
    namespace GUI
    {
        ExportImage::ExportImage(int maxFrame, QWidget *parent): QDialog(parent), ui(new Ui_ExportImage())
        {
            ui->setupUi(this);

            ui->sbStart->setMaximum(maxFrame);
            ui->sbEnd->setMaximum(maxFrame);
            ui->sbEnd->setValue(maxFrame);

            ui->cbFormat->addItem("PNG", QVariant());
            ui->cbFormat->addItem("JPG", QVariant());
            ui->cbFormat->addItem("BMP", QVariant());
            ui->cbFormat->setCurrentIndex(0);

            QObject::connect(ui->btnBrowse, &QPushButton::clicked, this, &ExportImage::Browse);

            QObject::connect(ui->sbStart, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int) {this->UpdateEnabling();});
            QObject::connect(ui->sbEnd, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int) {this->UpdateEnabling();});
            QObject::connect(ui->leDirectory, &QLineEdit::textChanged, [&](const QString &) {this->UpdateEnabling();});
            QObject::connect(ui->leName, &QLineEdit::textChanged, [&](const QString &) {this->UpdateEnabling();});

            UpdateEnabling();
        }

        int ExportImage::GetStartFrame()
        {
            return ui->sbStart->value();
        }

        int ExportImage::GetEndFrame()
        {
            return ui->sbEnd->value();
        }

        std::string ExportImage::GetDirectory()
        {
            return ui->leDirectory->text().toStdString();
        }

        std::string ExportImage::GetName()
        {
            return ui->leName->text().toStdString();
        }

        void ExportImage::Browse()
        {
            QString directory = QFileDialog::getExistingDirectory(this, tr("Open Output directory"), QString());
            if (!directory.isNull())
            {
                ui->leDirectory->setText(directory);
            }
        }

        void ExportImage::UpdateEnabling()
        {
            bool dirExists = QDir(ui->leDirectory->text()).exists() && !ui->leDirectory->text().isEmpty();
            bool nameNoneEmpty = !ui->leName->text().isEmpty();
            bool rangeCorrect = ui->sbStart->value() < ui->sbEnd->value();

            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(dirExists && nameNoneEmpty && rangeCorrect);
        }

        ImageFormat ExportImage::GetFormat()
        {
            switch (ui->cbFormat->currentIndex())
            {
                case 0:
                    return ImageFormat::PNG;
                case 1:
                    return ImageFormat::JPG;
                case 2:
                    return ImageFormat::BMP;
                default:
                    return ImageFormat::PNG;
            }
        }


    }

}