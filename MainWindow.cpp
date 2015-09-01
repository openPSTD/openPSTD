//
// Created by michiel on 18-7-2015.
//

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include "operations/FileOperations.h"
#include <QVariant>
#include "MouseHandlers.h"
#include "operations/MouseOperations.h"
#include "operations/EditOperations.h"
#include "DomainProperties.h"

MainWindow::MainWindow(std::shared_ptr<OperationRunner> operationRunner, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui_MainWindow()),
    operationRunner(operationRunner)
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

    QObject::connect(ui->actionDelete_selected_Domain, &QAction::triggered, this,
                     [&](bool checked){this->operationRunner->RunOperation(std::shared_ptr<RemoveSelectedDomainOperation>(new RemoveSelectedDomainOperation()));});
    QObject::connect(ui->actionEdit_properties_of_domain, &QAction::triggered, this, &MainWindow::EditSelectedDomain);
}

void MainWindow::UpdateFromModel(std::shared_ptr<Model> const &model)
{
    if(model->view->IsChanged())
    {
        ui->mainView->UpdateViewMatrix(model->view->value);
    }
    ui->mainView->UpdateFromModel(model);
    ui->mainView->update();
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
    for(int i = 0; i < this->MouseHandlersActions.size(); i++)
    {
        this->MouseHandlersActions[i]->setChecked(false);
    }
    action->setChecked(true);

    std::shared_ptr<ChangeMouseHandlerOperations> op(new ChangeMouseHandlerOperations(std::move(mouseHandler)));
    this->operationRunner->RunOperation(op);
}

void MainWindow::EditSelectedDomain()
{
    std::unique_ptr<DomainProperties> w = std::unique_ptr<DomainProperties>(new DomainProperties());
    w->exec();
}