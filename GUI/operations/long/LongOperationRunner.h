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
// Date: 10-4-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_LONGOPERATIONRUNNER_H
#define OPENPSTD_LONGOPERATIONRUNNER_H

#include "../BaseOperation.h"
#include <queue>
#include <boost/thread.hpp>

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
            virtual void Fatel(std::string message) = 0;
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
            virtual void Fatel(std::string message) override {};

            virtual void Error(std::string message) override {};

            virtual void Warning(std::string message) override {};

            virtual void Info(std::string message) override {};

            virtual void Debug(std::string message) override {};
        };

        /**
         * A base class for long running operations that need to run on the background.
         * Mainly used for the OpenPSTD algorithm, but can be used for other algorithms.
         */
        class LongOperation : public BaseOperation
        {
        private:
            /**
             * true if the next update should throw cancel operation exception
             */
            bool cancels;

        protected:
            /**
             * A simple method that should be called as many times as possible
             * whenever the state of the current operation can be aborted.
             * This method can create a interruption exception(abort thread) or a cancalation exception.
             */
            void Update();

            /**
             * Here can all the notification go.
             */
            std::shared_ptr<NotificationsHandler> notifications;

        public:
            LongOperation();

            /**
             * Should be a thread safe method.
             * Retrieves the name of the operation.
             */
            virtual std::string GetName() = 0;
            /**
             * Should be a thread safe method.
             * Retrieves the progress of the operation between 0 and 1. (inclusive, 0 = nothing done, 1 = finished)
             */
            virtual float GetProgress() = 0;
            /**
             * Should be a thread safe method.
             * Checks if the Operation is started. If the operation is finished, then this should still be true.
             */
            virtual bool Started(){ return this->GetProgress() == 0; };
            /**
             * Should be a thread safe method.
             * Checks if the operation is already finished.
             */
            virtual bool Finished(){ return this->GetProgress() == 1; };

            /**
             * Should be a thread safe method.
             * This makes sure this operation is canceled when the next Update is called.
             */
            virtual bool Cancel();

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



        class CancelLongOperationException : public std::exception
        {
        public:
            const char *what() const noexcept override;
        };

        /**
         * A simple background worker.
         * All the methods are thread safe.
         */
        class BackgroundWorker
        {
        private:
            std::queue<std::shared_ptr<LongOperation>> queue;
            boost::mutex queueMutex; //For a simple mutex is chosen because this queue is not used in a heavy duty system
            boost::condition_variable queueCondition;
            std::unique_ptr<boost::thread> backgroundWorker;
            std::shared_ptr<LongOperation> currentOperation;
            boost::recursive_mutex currentOperationMutex;
            std::weak_ptr<ReceiverBuilder> builder;
            boost::mutex defaultNotificationHandlerMutex;
            std::shared_ptr<NotificationsHandler> defaultNotificationHandler;
            bool running;

            void WorkerMethod();

        public:
            BackgroundWorker(std::weak_ptr<ReceiverBuilder> builder);

            /**
             * Start the background work.
             */
            void Start();
            /**
             * End after the queue is finished, no new work items can be generated.
             */
            void EndAfterWork();
            /**
             * stoppes the background worker as soon as possible at a interrupt point.
             */
            void EndASAP();
            /**
             * Waits for the worker to finish.
             */
            void JoinAfterWork();
            /**
             * Waits for the worker to finish.
             */
            void JoinASAP();

            /**
             * Enqueue an operation, if the queue is empty, this will immediately start.
             */
            void EnqueueOperation(std::shared_ptr<LongOperation> Op);
            /**
             * Stops the current operation.
             */
            void StopCurrentOperation();

            /**
             * True is the background worker does nothing.
             */
            bool IsIdle();
            /**
             * Get the progress of the current long operation.
             */
            float GetCurrentProgress();
            /**
             * Get the name of the current long operation.
             */
            std::string GetCurrentName();

            void SetDefaultNotificationHandler(std::shared_ptr<NotificationsHandler> notificationHandler);

        };
    }
}

#endif //OPENPSTD_LONGOPERATIONRUNNER_H