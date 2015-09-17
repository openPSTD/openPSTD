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
#include "ui_MainWindow.h"
#include <QFileDialog>
#include "operations/FileOperations.h"
#include <QVariant>
#include "MouseHandlers.h"
#include "operations/MouseOperations.h"
#include "operations/EditOperations.h"

MainWindow::MainWindow(std::shared_ptr<OperationRunner> operationRunner, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui_MainWindow()),
    operationRunner(operationRunner),
    domainProperties(new DomainProperties()),
    documentSettings(new DocumentSettings())
{
    ui->setupUi(this);

    ui->mainView->SetOperationRunner(this->operationRunner);

    MouseHandlersActions.push_back(ui->actionMove_scene);
    MouseHandlersActions.push_back(ui->actionAdd_Domain);
    MouseHandlersActions.push_back(ui->actionSelect);

    QObject::connect(ui->actionNew, &QAction::triggered, this, &MainWindow::New);
    QObject::connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::Open);
    QObject::connect(ui->actionSave, &QAction::triggered, this, &MainWindow::Save);
    QObject::connect(ui->actionMove_scene, &QAction::triggered, this,
                     [&](bool checked){ChangeMouseHandler(ui->actionMove_scene, std::unique_ptr<MouseMoveSceneStrategy>(new MouseMoveSceneStrategy()));});
    QObject::connect(ui->actionAdd_Domain, &QAction::triggered, this,
                     [&](bool checked){ChangeMouseHandler(ui->actionAdd_Domain, std::unique_ptr<MouseCreateDomainStrategy>(new MouseCreateDomainStrategy()));});
    QObject::connect(ui->actionSelect, &QAction::triggered, this,
                     [&](bool checked){ChangeMouseHandler(ui->actionSelect, std::unique_ptr<MouseSelectStrategy>(new MouseSelectStrategy()));});
    QObject::connect(ui->actionAdd_Receiver, &QAction::triggered, this,
                     [&](bool checked){ChangeMouseHandler(ui->actionAdd_Receiver, std::unique_ptr<MouseCreateSpeakerReceiverStrategy>(new MouseCreateSpeakerReceiverStrategy(OBJECT_RECEIVER)));});
    QObject::connect(ui->actionAdd_speaker, &QAction::triggered, this,
                     [&](bool checked){ChangeMouseHandler(ui->actionAdd_speaker, std::unique_ptr<MouseCreateSpeakerReceiverStrategy>(new MouseCreateSpeakerReceiverStrategy(OBJECT_SPEAKER)));});

    QObject::connect(ui->actionDelete_selected, &QAction::triggered, this,
                     [&](bool checked){this->operationRunner->RunOperation(std::shared_ptr<RemoveSelectedObjectOperation>(new RemoveSelectedObjectOperation()));});
    QObject::connect(ui->actionEdit_properties_of_domain, &QAction::triggered, this, &MainWindow::EditSelectedDomain);
    QObject::connect(ui->actionDocument_Settings, &QAction::triggered, this, &MainWindow::EditDocumentSettings);
}

void MainWindow::UpdateFromModel(std::shared_ptr<Model> const &model)
{
    ui->mainView->UpdateFromModel(model);
    ui->mainView->update();

    this->UpdateDisableWidgets(model);

    documentSettings->UpdateFromModel(model);

    if(model->interactive->IsChanged() && model->interactive->Selection.Type == SELECTION_DOMAIN)
    {
        std::shared_ptr<rapidjson::Document> conf = model->d->GetSceneConf();

        rapidjson::Value& domains = (*conf)["domains"];
        int i = model->interactive->Selection.SelectedIndex;

        float aTop = domains[i]["edges"]["t"]["a"].GetDouble();
        float aBottom = domains[i]["edges"]["b"]["a"].GetDouble();
        float aLeft = domains[i]["edges"]["l"]["a"].GetDouble();
        float aRight = domains[i]["edges"]["r"]["a"].GetDouble();

        bool lrTop = domains[i]["edges"]["t"]["lr"].GetBool();
        bool lrBottom = domains[i]["edges"]["b"]["lr"].GetBool();
        bool lrLeft = domains[i]["edges"]["l"]["lr"].GetBool();
        bool lrRight = domains[i]["edges"]["r"]["lr"].GetBool();

        domainProperties->SetAbsorptionT(aTop);
        domainProperties->SetAbsorptionB(aBottom);
        domainProperties->SetAbsorptionL(aLeft);
        domainProperties->SetAbsorptionR(aRight);

        domainProperties->SetLRT(lrTop);
        domainProperties->SetLRB(lrBottom);
        domainProperties->SetLRL(lrLeft);
        domainProperties->SetLRR(lrRight);
    }
}

void MainWindow::New()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Create PSTD file"), QString(), tr("PSTD file (*.pstd)"));
    if(!fileName.isNull())
    {
        std::shared_ptr<NewFileOperation> op(new NewFileOperation(fileName.toStdString()));
        this->operationRunner->RunOperation(op);
    }
}

void MainWindow::Open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open PSTD file"), QString(), tr("PSTD file (*.pstd)"));
    if(!fileName.isNull())
    {
        std::shared_ptr<OpenFileOperation> op(new OpenFileOperation(fileName.toStdString()));
        this->operationRunner->RunOperation(op);
    }
}

void MainWindow::Save()
{
    std::shared_ptr<SaveFileOperation> op(new SaveFileOperation());
    this->operationRunner->RunOperation(op);
}

void MainWindow::ChangeMouseHandler(QAction *action, std::unique_ptr<MouseStrategy> mouseHandler)
{
    for (int i = 0; i < this->MouseHandlersActions.size(); i++)
    {
        this->MouseHandlersActions[i]->setChecked(false);
    }
    action->setChecked(true);

    std::shared_ptr<ChangeMouseHandlerOperations> op(new ChangeMouseHandlerOperations(std::move(mouseHandler)));
    this->operationRunner->RunOperation(op);
}

void MainWindow::EditSelectedDomain()
{
    if(this->domainProperties->exec() == QDialog::Accepted)
    {
        std::shared_ptr<EditSelectedDomainEdgesOperation> op(new EditSelectedDomainEdgesOperation());
        op->AbsorptionT = this->domainProperties->AbsorptionT();
        op->AbsorptionB = this->domainProperties->AbsorptionB();
        op->AbsorptionL = this->domainProperties->AbsorptionL();
        op->AbsorptionR = this->domainProperties->AbsorptionR();
        op->LRT = this->domainProperties->LRT();
        op->LRB = this->domainProperties->LRB();
        op->LRL = this->domainProperties->LRL();
        op->LRR = this->domainProperties->LRR();
        this->operationRunner->RunOperation(op);
    }
}

void MainWindow::UpdateDisableWidgets(std::shared_ptr<Model> const &model)
{
    ui->actionDelete_selected->setEnabled(model->interactive->Selection.Type != SELECTION_NONE);
    ui->actionEdit_properties_of_domain->setEnabled(model->interactive->Selection.Type == SELECTION_DOMAIN);
    ui->actionResize_domain->setEnabled(model->interactive->Selection.Type == SELECTION_DOMAIN);
}

void MainWindow::EditDocumentSettings()
{
    if (this->documentSettings->exec() == QDialog::Accepted)
    {
        this->operationRunner->RunOperation(std::shared_ptr<EditDocumentSettingsOperation>(
                new EditDocumentSettingsOperation(this->documentSettings->GetResult())));
    }
}