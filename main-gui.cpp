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
    std::shared_ptr<Controller> c(new Controller());

    c->SetArguments(argc, argv);
    c->SetOperationRunner(c);
    c->RunApplication();
}

Controller::Controller():
    operationRunner(nullptr),
    runningOp(false)
{

}

void Controller::SetOperationRunner(std::shared_ptr<OperationRunner> runner)
{
    this->operationRunner = runner;
}

void Controller::RunOperation(std::shared_ptr<BaseOperation> operation)
{
    Reciever r;
    r.model = this->model;
    r.operationRunner = this->operationRunner;
    if(runningOp)
    {
        operation->Run(r);
    }
    else
    {
        runningOp = true;
        operation->Run(r);
        runningOp = false;
        //todo: fix that also the document itself is registered
        //if(this->model->invalidation.IsChanged())
        {
            this->w->UpdateFromModel(this->model);
        }
        this->model->invalidation.Reset();
    }
}

void Controller::RunOperationWithoutUpdate(std::shared_ptr<BaseOperation> operation)
{
    Reciever r;
    r.model = this->model;
    r.operationRunner = this->operationRunner;
    runningOp = true;
    operation->Run(r);
    runningOp = false;
}
void Controller::UpdateWithoutOperation()
{
    this->w->UpdateFromModel(this->model);
    this->model->invalidation.Reset();
}

void Controller::SetArguments(int argc, char *argv[])
{
    this->argc = argc;
    this->argv = argv;
}

int Controller::RunApplication()
{
    this->model = std::shared_ptr<Model>(new Model());
    this->RunOperationWithoutUpdate(std::shared_ptr<BaseOperation>(new InitializationOperation()));

    this->a = std::unique_ptr<QApplication>(new QApplication(argc, argv));
    this->w = std::unique_ptr<MainWindow>(new MainWindow(this->operationRunner));

    this->w->show();

    this->UpdateWithoutOperation();

    return a->exec();
}


