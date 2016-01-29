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
// Date:
//
//
// Authors:
//
//
//////////////////////////////////////////////////////////////////////////

//
// Created by michiel on 18-7-2015.
//

#include <iostream>
#include <QtWidgets/qapplication.h>
#include "MainWindow.h"
#include "main.h"
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
        this->model->Reset();
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
    this->model->Reset();
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


