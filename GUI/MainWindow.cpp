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
// Date: 18-07-2015
//
//
// Authors: M. R. Fortuin
//
//
//////////////////////////////////////////////////////////////////////////


#include "MainWindow.h"
#include <QFileDialog>
#include "operations/FileOperations.h"
#include "operations/MouseOperations.h"
#include "operations/EditOperations.h"
#include "operations/SettingsOperations.h"
#include "operations/long/LOperationOperation.h"
#include "operations/long/SimulateLOperation.h"
#include "AboutBoxesText.h"
#include "mouse/MouseSelectStrategy.h"
#include "mouse/MouseMoveSceneStrategy.h"
#include "mouse/MouseCreateDomainStrategy.h"
#include "mouse/MouseCreateSpeakerReceiverStrategy.h"
#include <QMessageBox>

namespace OpenPSTD
{
    namespace GUI
    {
        MainWindow::MainWindow(std::weak_ptr<OperationRunner> operationRunner, QWidget *parent) :
                QMainWindow(parent),
                ui(new Ui_MainWindow()),
                operationRunner(operationRunner),
                domainProperties(new DomainProperties()),
                documentSettings(new DocumentSettings()),
                applicationSettings(std::make_shared<ApplicationSettings>())
        {
            ui->setupUi(this);

            progressBar = new QProgressBar(ui->statusbar);
            progressBar->setAlignment(Qt::AlignRight);
            progressBar->setMaximumSize(180, 19);
            progressBar->setValue(0);
            progressBar->setVisible(false);

            statusbarLabel = new QLabel(ui->statusbar);
            statusbarLabel->setAlignment(Qt::AlignRight);
            statusbarLabel->setMaximumHeight(19);
            statusbarLabel->setVisible(false);

            ui->statusbar->addWidget(progressBar);
            ui->statusbar->addWidget(statusbarLabel);

            ui->mainView->SetOperationRunner(this->operationRunner);

            MouseHandlersActions.push_back(ui->actionMove_scene);
            MouseHandlersActions.push_back(ui->actionAdd_Domain);
            MouseHandlersActions.push_back(ui->actionSelect);
            MouseHandlersActions.push_back(ui->actionAdd_Receiver);
            MouseHandlersActions.push_back(ui->actionAdd_speaker);

            QObject::connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::ShowAbout);
            QObject::connect(ui->actionAbout_icons, &QAction::triggered, this, &MainWindow::ShowAboutIcons);

            QObject::connect(ui->actionNew, &QAction::triggered, this, &MainWindow::New);
            QObject::connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::Open);
            QObject::connect(ui->actionSave, &QAction::triggered, this, &MainWindow::Save);
            QObject::connect(ui->actionMove_scene, &QAction::triggered, this,
                             [&](bool checked) {
                                 ChangeMouseHandler(ui->actionMove_scene,
                                                    std::unique_ptr<MouseMoveSceneStrategy>(
                                                            new MouseMoveSceneStrategy()));
                             });
            QObject::connect(ui->actionAdd_Domain, &QAction::triggered, this,
                             [&](bool checked) {
                                 ChangeMouseHandler(ui->actionAdd_Domain,
                                                    std::unique_ptr<MouseCreateDomainStrategy>(
                                                            new MouseCreateDomainStrategy()));
                             });
            QObject::connect(ui->actionSelect, &QAction::triggered, this,
                             [&](bool checked) {
                                 ChangeMouseHandler(ui->actionSelect,
                                                    std::unique_ptr<MouseSelectStrategy>(
                                                            new MouseSelectStrategy()));
                             });
            QObject::connect(ui->actionAdd_Receiver, &QAction::triggered, this,
                             [&](bool checked) {
                                 ChangeMouseHandler(ui->actionAdd_Receiver,
                                                    std::unique_ptr<MouseCreateSpeakerReceiverStrategy>(
                                                            new MouseCreateSpeakerReceiverStrategy(
                                                                    OBJECT_RECEIVER)));
                             });
            QObject::connect(ui->actionAdd_speaker, &QAction::triggered, this,
                             [&](bool checked) {
                                 ChangeMouseHandler(ui->actionAdd_speaker,
                                                    std::unique_ptr<MouseCreateSpeakerReceiverStrategy>(
                                                            new MouseCreateSpeakerReceiverStrategy(
                                                                    OBJECT_SPEAKER)));
                             });

            QObject::connect(ui->actionDelete_selected, &QAction::triggered, this,
                             [&](bool checked) {
                                 this->operationRunner.lock()->RunOperation(
                                         std::make_shared<RemoveSelectedObjectOperation>());
                             });
            QObject::connect(ui->actionStart_simulation, &QAction::triggered, this,
                             [&](bool checked) {
                                 this->operationRunner.lock()->RunOperation(
                                         std::make_shared<StartLOperation>(std::make_shared<SimulateLOperation>()));
                             });

            QObject::connect(ui->actionEdit_properties_of_domain, &QAction::triggered, this,
                             &MainWindow::EditSelectedDomain);
            QObject::connect(ui->actionDocument_Settings, &QAction::triggered, this, &MainWindow::EditDocumentSettings);
            QObject::connect(ui->actionApplication_Settings, &QAction::triggered, this, &MainWindow::EditApplicationSettings);
        }

        void MainWindow::UpdateFromModel(std::shared_ptr<Model> const &model, std::shared_ptr<BackgroundWorker> worker)
        {
            ui->mainView->UpdateFromModel(model);
            ui->mainView->update();

            this->UpdateDisableWidgets(model, worker);

            documentSettings->UpdateFromModel(model);
            applicationSettings->UpdateFromModel(model);

            if (model->interactive->IsChanged() && model->interactive->Selection.Type == SELECTION_DOMAIN)
            {
                auto conf = model->documentAccess->GetDocument()->GetSceneConf();
                int i = model->interactive->Selection.SelectedIndex;
                domainProperties->SetDomain(conf->Domains[i]);
            }

            std::cout << "update ";
            if(!worker->IsIdle())
            {
                this->progressBar->show();
                this->statusbarLabel->show();
                this->progressBar->setValue((int)(worker->GetCurrentProgress()*100));
                std::cout << (int)(worker->GetCurrentProgress()*100) << std::endl;
                this->statusbarLabel->setText(QString::fromStdString(worker->GetCurrentName()));
            }
            else
            {
                std::cout << -1 << std::endl;
                this->progressBar->hide();
                this->statusbarLabel->hide();
            }
        }

        void MainWindow::New()
        {
            QString fileName = QFileDialog::getSaveFileName(this, tr("Create PSTD file"), QString(),
                                                            tr("PSTD file (*.pstd)"));
            if (!fileName.isNull())
            {
                auto op = std::make_shared<NewFileOperation>(fileName.toStdString());
                this->operationRunner.lock()->RunOperation(op);
            }
        }

        void MainWindow::Open()
        {
            QString fileName = QFileDialog::getOpenFileName(this, tr("Open PSTD file"), QString(),
                                                            tr("PSTD file (*.pstd)"));
            if (!fileName.isNull())
            {
                auto op = std::make_shared<OpenFileOperation>(fileName.toStdString());
                this->operationRunner.lock()->RunOperation(op);
            }
        }

        void MainWindow::Save()
        {
            auto op = std::make_shared<SaveFileOperation>();
            this->operationRunner.lock()->RunOperation(op);
        }

        void MainWindow::ChangeMouseHandler(QAction *action, std::unique_ptr<MouseStrategy> mouseHandler)
        {
            for (int i = 0; i < this->MouseHandlersActions.size(); i++)
            {
                this->MouseHandlersActions[i]->setChecked(false);
            }
            action->setChecked(true);

            auto op = std::make_shared<ChangeMouseHandlerOperations>(std::move(mouseHandler));
            this->operationRunner.lock()->RunOperation(op);
        }

        void MainWindow::EditSelectedDomain()
        {
            if (this->domainProperties->exec() == QDialog::Accepted)
            {
                auto op = std::make_shared<EditSelectedDomainEdgesOperation>();
                op->AbsorptionT = this->domainProperties->AbsorptionT();
                op->AbsorptionB = this->domainProperties->AbsorptionB();
                op->AbsorptionL = this->domainProperties->AbsorptionL();
                op->AbsorptionR = this->domainProperties->AbsorptionR();
                op->LRT = this->domainProperties->LRT();
                op->LRB = this->domainProperties->LRB();
                op->LRL = this->domainProperties->LRL();
                op->LRR = this->domainProperties->LRR();
                this->operationRunner.lock()->RunOperation(op);
            }
        }

        void MainWindow::UpdateDisableWidgets(std::shared_ptr<Model> const &model, std::shared_ptr<BackgroundWorker> worker)
        {
            ui->actionDelete_selected->setEnabled(model->interactive->Selection.Type != SELECTION_NONE);
            ui->actionEdit_properties_of_domain->setEnabled(model->interactive->Selection.Type == SELECTION_DOMAIN);
            ui->actionResize_domain->setEnabled(model->interactive->Selection.Type == SELECTION_DOMAIN);
        }

        void MainWindow::EditDocumentSettings()
        {
            if (this->documentSettings->exec() == QDialog::Accepted)
            {
                this->operationRunner.lock()->RunOperation(
                        std::make_shared<EditDocumentSettingsOperation>(this->documentSettings->GetResult()));
            }
        }

        void MainWindow::EditApplicationSettings()
        {
            if (this->applicationSettings->exec() == QDialog::Accepted)
            {
                this->operationRunner.lock()->RunOperation(
                        std::make_shared<UpdateSettingsOperation>(this->applicationSettings));
            }
        }

        void MainWindow::ShowAbout()
        {
            QMessageBox::about(this, "About OpenPSTD", QString::fromStdString(ABOUT_OPENPSTD));
        }

        void MainWindow::ShowAboutIcons()
        {
            QMessageBox::about(this, "About icons", QString::fromStdString(ABOUT_NOUN_ICONS));
        }
    }
}