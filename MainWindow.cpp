//
// Created by michiel on 18-7-2015.
//

#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(std::shared_ptr<OperationRunner> operationRunner, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui_MainWindow()),
    operationRunner(operationRunner)
{
    ui->setupUi(this);

    ui->mainView->SetOperationRunner(this->operationRunner);
}

void MainWindow::UpdateFromModel(std::shared_ptr<Model> const &model)
{
    ui->mainView->UpdateFromModel(model);
    ui->mainView->UpdateViewMatrix(model->view);
    ui->mainView->update();
}
