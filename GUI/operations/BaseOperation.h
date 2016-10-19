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

#ifndef OPENPSTD_BASEOPERATION_H
#define OPENPSTD_BASEOPERATION_H

#include <memory>
#include "Reciever.h"

namespace OpenPSTD
{
    namespace GUI
    {
        /**
         * A notification handler. All the errors can be handled through this class. All methods are called from the
         * runnable inside BackgroundWorker. So if there are some changes in the GUI, it should be remotly invoked.
         * A possible implementation is to write the information to the command line or is to create a pop-up screen
         * with the message. So the methods (except for debug) should be used sparsely.
         */
        class NotificationsHandler
        {
        public:
            /**
             * An fatel error where the long operation can not continue.
             */
            virtual void Fatal(std::string message) = 0;
            /**
             * An simple error, the long operation may not or may continue after this error.
             */
            virtual void Error(std::string message) = 0;
            /**
             * A warning, the long operation will continue after this, but the user should be aware of something that
             * probably is wrong.
             */
            virtual void Warning(std::string message) = 0;
            /**
             * Information that the user should be aware of.
             */
            virtual void Info(std::string message) = 0;
            /**
             * Debug information, this can be used extensivly, and is only shown to the user if the user has chosen it.
             */
            virtual void Debug(std::string message) = 0;
        };

        /**
         * A null notification handler. This handler ignores every notification.
         */
        class NullNotificationsHandler: public NotificationsHandler
        {
        public:
            virtual void Fatal(std::string message) override {};

            virtual void Error(std::string message) override {};

            virtual void Warning(std::string message) override {};

            virtual void Info(std::string message) override {};

            virtual void Debug(std::string message) override {};
        };

        class BaseOperation
        {
        protected:
            /**
             * Here can all the notification go.
             */
            std::shared_ptr<NotificationsHandler> notifications;
        public:

            BaseOperation(): notifications(nullptr)
            {

            }

            virtual void Run(const Reciever &reciever) = 0;

            /**
             * Sets the Notification handler. This should be set before the operation has started or else the behavior
             * is undefined. The current implementation of Background worker will always set this handler to an instance
             * before starting the operation.
             * @param handler
             */
            void SetNotificationHandler(std::shared_ptr<NotificationsHandler> handler) { this->notifications = handler; };
            /**
             * Gets the Notification handler.
             * @return
             */
            std::shared_ptr<NotificationsHandler> GetNotificationHandler(){ return this->notifications; };

        };

        class EmptyOperation: public BaseOperation
        {
        public:
            virtual void Run(const Reciever &reciever);
        };

        class OperationRunner
        {
        public:
            virtual void RunOperation(std::shared_ptr<BaseOperation> operation) = 0;
        };

        class ReceiverBuilder
        {
        public:
            virtual Reciever BuildReceiver() = 0;
        };
    }
}

#endif //OPENPSTD_BASEOPERATION_H
