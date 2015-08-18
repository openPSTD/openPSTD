//
// Created by michiel on 18-7-2015.
//

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include "operations/FileOperations.h"

MainWindow::MainWindow(std::shared_ptr<OperationRunner> operationRunner, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui_MainWindow()),
    operationRunner(operationRunner)
{
    ui->setupUi(this);

    ui->mainView->SetOperationRunner(this->operationRunner);

    QObject::connect(ui->actionNew, &QAction::triggered, this, &MainWindow::New);
    QObject::connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::Open);
    QObject::connect(ui->actionSave, &QAction::triggered, this, &MainWindow::Save);
}

void MainWindow::UpdateFromModel(std::shared_ptr<Model> const &model)
{
    ui->mainView->UpdateFromModel(model);
    ui->mainView->UpdateViewMatrix(model->view);
    ui->mainView->update();
}

void MainWindow::New()
{
    std::string fileName = QFileDialog::getSaveFileName(this, tr("Create PSTD file"), QString(), tr("PSTD file (*.pstd)")).toStdString();
    std::shared_ptr<NewFileOperation> op(new NewFileOperation(fileName));
    this->operationRunner->RunOperation(op);
}

void MainWindow::Open()
{
    std::string fileName = QFileDialog::getOpenFileName(this, tr("Open PSTD file"), QString(), tr("PSTD file (*.pstd)")).toStdString();
    std::shared_ptr<OpenFileOperation> op(new OpenFileOperation(fileName));
    this->operationRunner->RunOperation(op);
}

void MainWindow::Save()
{
    std::shared_ptr<SaveFileOperation> op(new SaveFileOperation());
    this->operationRunner->RunOperation(op);
}
