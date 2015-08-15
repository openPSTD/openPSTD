//
// Created by michiel on 18-7-2015.
//

#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui_MainWindow())
{
    ui->setupUi(this);

    std::unique_ptr<Model> model(new Model());
    model->d = std::unique_ptr<PSTDFile>(PSTDFile::New("test.jps"));

    QMatrix4x4 view = QMatrix4x4();
    view.scale(0.5f, 0.5f);

    ui->mainView->UpdateFromModel(model);
    ui->mainView->UpdateViewMatrix(view);
}

MainWindow::~MainWindow()
{
    delete ui;
}
