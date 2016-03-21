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
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_MAIN_GUI_H
#define OPENPSTD_MAIN_GUI_H

#include <memory>
#include "operations/BaseOperation.h"

namespace OpenPSTD
{
    namespace GUI
    {
        class Controller: public OperationRunner, public std::enable_shared_from_this<Controller>
        {
        public:
            Controller();
            ~Controller() { std::cout << "destroy controller...." << std::endl; }

            void SetArguments(int argc, char *argv[]);

            virtual void RunOperation(std::shared_ptr<BaseOperation> operation);

            void RunOperationWithoutUpdate(std::shared_ptr<BaseOperation> operation);
            void UpdateWithoutOperation();

            std::shared_ptr<Model> model;
            std::unique_ptr<QApplication> a;
            std::unique_ptr<MainWindow> w;

            int argc;
            char **argv;

        private:

            bool runningOp;

        };
    }
}

#endif //OPENPSTD_MAIN_GUI_H
