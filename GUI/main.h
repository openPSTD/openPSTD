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

#ifndef OPENPSTD_MAIN_GUI_H
#define OPENPSTD_MAIN_GUI_H

#include <QObject>
#include <memory>
#include "operations/BaseOperation.h"
#include "operations/long/LongOperationRunner.h"

namespace OpenPSTD
{
    namespace GUI
    {
        class Controller:
                public QObject,
                public OperationRunner,
                public ReceiverBuilder,
                public std::enable_shared_from_this<Controller>,
                public NotificationsHandler
        {
        Q_OBJECT
        public:
            Controller();

            void StartUpBackgroundWorker();
            void ShutdownBackgroundWorker();

            virtual void RunOperation(std::shared_ptr<BaseOperation> operation);

            void RunOperationWithoutUpdate(std::shared_ptr<BaseOperation> operation);
            void UpdateWithoutOperation();

            Reciever BuildReceiver();

            std::shared_ptr<Model> model;
            std::unique_ptr<QApplication> a;
            std::unique_ptr<MainWindow> w;



        private:
            bool runningOp;
            std::shared_ptr <BackgroundWorker> worker;

        private slots:
            virtual void Fatal(std::string message) override;

            virtual void Error(std::string message) override;

            virtual void Warning(std::string message) override;

            virtual void Info(std::string message) override;

            virtual void Debug(std::string message) override;

        };

        class SignalNotificationsHandler : public QObject, public NotificationsHandler
        {
        Q_OBJECT
        signals:
            void Fatal(std::string message) override;

            void Error(std::string message) override;

            void Warning(std::string message) override;

            void Info(std::string message) override;

            void Debug(std::string message) override;
        };
    }
}

#endif //OPENPSTD_MAIN_GUI_H
