//
// Created by michiel on 18-7-2015.
//

#include <iostream>
#include <QtWidgets/qapplication.h>
#include "MainWindow.h"
#include "main-gui.h"
#include "operations/InitializationOperation.h"

int main(int argc, char *argv[])
{
    Controller c;
    c.SetArguments(argc, argv);
    c.RunApplication();
}

Controller::Controller():
    operationRunner(this)
{

}

void Controller::RunOperation(std::shared_ptr<BaseOperation> operation)
{
    Reciever r;
    r.model = this->model;
    r.operationRunner = this->operationRunner;
    operation->Run(r);

    this->w->UpdateFromModel(this->model);
}

void Controller::SetArguments(int argc, char *argv[])
{
    this->argc = argc;
    this->argv = argv;
}

int Controller::RunApplication()
{
    this->model = std::shared_ptr<Model>(new Model());

    this->a = std::unique_ptr<QApplication>(new QApplication(argc, argv));
    this->w = std::unique_ptr<MainWindow>(new MainWindow(this->operationRunner));
    this->w->show();

    this->operationRunner->RunOperation(std::shared_ptr<BaseOperation>(new InitializationOperation()));

    return a->exec();
}


