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

#ifndef OPENPSTD_MAINWINDOW_H
#define OPENPSTD_MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include "operations/BaseOperation.h"
#include "MouseHandlers.h"
#include "DomainProperties.h"
#include "DocumentSettings.h"
#include <ui_MainWindow.h>

namespace OpenPSTD
{
    namespace GUI
    {

        class MainWindow : public QMainWindow
        {
        Q_OBJECT

        public:
            explicit MainWindow(std::weak_ptr<OperationRunner> operationRunner, QWidget *parent = 0);

            void UpdateFromModel(std::shared_ptr<Model> const &model);

        private:
            std::unique_ptr<Ui_MainWindow> ui;
            std::unique_ptr<DomainProperties> domainProperties;
            std::unique_ptr<DocumentSettings> documentSettings;
            std::weak_ptr<OperationRunner> operationRunner;
            std::vector<QAction *> MouseHandlersActions;

            void ChangeMouseHandler(QAction *action, std::unique_ptr<MouseStrategy> mouseHandler);

            void EditSelectedDomain();

            void EditDocumentSettings();

            void UpdateDisableWidgets(std::shared_ptr<Model> const &model);

            void ShowAbout();

            void ShowAboutIcons();

        public slots:

            void New();

            void Open();

            void Save();


        };

    }
}


#endif //OPENPSTD_MAINWINDOW_H
