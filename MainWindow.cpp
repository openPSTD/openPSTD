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
}

MainWindow::~MainWindow()
{
        delete ui;
}
