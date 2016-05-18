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
//      18-7-2015
//
// Authors:
//      michiel
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_MAINWINDOW_H
#define OPENPSTD_MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QTimer>
#include <memory>
#include "operations/BaseOperation.h"
#include "mouse/MouseStrategy.h"
#include "DomainProperties.h"
#include "DocumentSettings.h"
#include "ApplicationSettings.h"
#include <ui_MainWindow.h>
#include "operations/long/LongOperationRunner.h"

namespace OpenPSTD
{
    namespace GUI
    {

        class MainWindow : public QMainWindow
        {
        Q_OBJECT

        public:
            explicit MainWindow(std::weak_ptr<OperationRunner> operationRunner, QWidget *parent = 0);

            void UpdateFromModel(std::shared_ptr<Model> const &model, std::shared_ptr<BackgroundWorker> worker);

        private:
            std::unique_ptr<Ui_MainWindow> ui;
            std::unique_ptr<DomainProperties> domainProperties;
            std::shared_ptr<ApplicationSettings> applicationSettings;
            std::weak_ptr<OperationRunner> operationRunner;
            QProgressBar* progressBar;
            QLabel* statusbarLabel;
            QLabel* statusbarLabelFrameNumber;
            QTimer* updateTimer;
            std::vector<QAction *> MouseHandlersActions;
            int maxFrame;
            Kernel::PSTDSettings fileSettings;

            void ChangeMouseHandler(QAction *action, std::unique_ptr<MouseStrategy> mouseHandler);

            void EditSelectedDomain();

            void EditDocumentSettings();
            void EditApplicationSettings();

            void UpdateDisableWidgets(std::shared_ptr<Model> const &model, std::shared_ptr<BackgroundWorker> worker);
            void UpdateHsbFrame(std::shared_ptr<Model> const &model, std::shared_ptr<BackgroundWorker> worker);

            void ShowAbout();

            void ShowAboutIcons();

            void hsbFrameChanged(int value);

        public slots:

            void New();

            void Open();

            void Save();
            void ExportHDF5();
            void ExportImage();


        };

    }
}


#endif //OPENPSTD_MAINWINDOW_H
