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
// Date: 18-7-2015
//
//
// Authors: M. R. Fortuin
//
//
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <QtWidgets/qapplication.h>
#include "MainWindow.h"
#include "main.h"
#include "operations/InitializationOperation.h"

int main(int argc, char *argv[])
{
    using namespace OpenPSTD::GUI;

    std::shared_ptr <Controller> c = std::make_shared<Controller>();

    c->model = std::make_shared<Model>();
    c->RunOperationWithoutUpdate(std::make_shared<InitializationOperation>());

    c->a = std::unique_ptr<QApplication>(new QApplication(argc, argv));
    c->w = std::unique_ptr<MainWindow>(new MainWindow(c));

    c->w->show();

    c->UpdateWithoutOperation();

    int result = c->a->exec();

    return result;
}

namespace OpenPSTD
{
    namespace GUI
    {
        Controller::Controller() :
                runningOp(false)
        {

        }

        void Controller::RunOperation(std::shared_ptr<BaseOperation> operation)
        {
            Reciever r;
            r.model = this->model;
            r.operationRunner = this->shared_from_this();
            if (runningOp)
            {
                operation->Run(r);
            }
            else
            {
                runningOp = true;
                operation->Run(r);
                runningOp = false;
                //todo: fix that also the documentAccess itself is registered
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
            r.operationRunner = this->shared_from_this();
            runningOp = true;
            operation->Run(r);
            runningOp = false;
        }

        void Controller::UpdateWithoutOperation()
        {
            this->w->UpdateFromModel(this->model);
            this->model->Reset();
        }
    }
}
