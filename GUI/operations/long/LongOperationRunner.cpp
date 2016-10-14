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
//////////////////////////////////////////////////////////////////////////

#include "LongOperationRunner.h"

using namespace OpenPSTD::GUI;
using namespace boost;
using namespace std;

const char *CancelLongOperationException::what() const noexcept
{
    return "Cancels this operation";
}

BackgroundWorker::BackgroundWorker(std::weak_ptr<ReceiverBuilder> builder)
{
    this->builder = builder;
    this->defaultNotificationHandler = std::make_shared<NullNotificationsHandler>();
}

void BackgroundWorker::Start()
{
    this->backgroundWorker = std::unique_ptr<thread>(new thread(&BackgroundWorker::WorkerMethod, this));
}

void BackgroundWorker::EndAfterWork()
{
    this->running = false;
}

void BackgroundWorker::EndASAP()
{
    this->backgroundWorker->interrupt();
}

void BackgroundWorker::JoinAfterWork()
{
    this->EndAfterWork();
    this->backgroundWorker->join();
}

void BackgroundWorker::JoinASAP()
{
    this->EndASAP();
    this->backgroundWorker->join();
}

void BackgroundWorker::EnqueueOperation(std::shared_ptr<LongOperation> Op)
{
    boost::lock_guard<boost::mutex> _(this->queueMutex);
    this->queue.push(Op);
    this->queueCondition.notify_all();
}

bool BackgroundWorker::IsIdle()
{
    boost::unique_lock<boost::recursive_mutex> lock(this->currentOperationMutex);
    return currentOperation?false:true;
}

float BackgroundWorker::GetCurrentProgress()
{
    boost::unique_lock<boost::recursive_mutex> lock(this->currentOperationMutex);
    if(this->IsIdle())
    {
        return -1;
    }
    else
    {
        return currentOperation->GetProgress();
    }
}

std::string BackgroundWorker::GetCurrentName()
{
    boost::unique_lock<boost::recursive_mutex> lock(this->currentOperationMutex);
    if(this->IsIdle())
    {
        return "";
    }
    else
    {
        return currentOperation->GetName();
    }
}

void BackgroundWorker::WorkerMethod()
{
    this->running = true;
    try
    {
        while (running)
        {
            //queue handler
            {
                boost::unique_lock<boost::mutex> lock(this->queueMutex);
                // wait while the queue is empty
                while(this->queue.size() == 0)
                {
                    queueCondition.wait(lock); //will temporary unlock queue
                }
                //dequeue the first element in the queue
                {
                    boost::unique_lock<boost::recursive_mutex> lockCurrentOp(this->currentOperationMutex);
                    this->currentOperation = this->queue.front();
                }
                this->queue.pop();
            }
            //check for the notification handler
            if(!this->currentOperation->GetNotificationHandler())
            {
                boost::unique_lock<boost::mutex> lock(this->defaultNotificationHandlerMutex);
                this->currentOperation->SetNotificationHandler(this->defaultNotificationHandler);
            }
            //create a receiver
            Reciever r;
            {
                std::shared_ptr<ReceiverBuilder> builder = this->builder.lock();
                r = builder->BuildReceiver();
            }//make sure the shared_ptr to builder is removed
            try
            {
                //run the operation
                this->currentOperation->Run(r);
            }
            catch(CancelLongOperationException) //catches cancelation
            {
                this->currentOperation->GetNotificationHandler()->Warning("The operation is canceled");
            }
            catch(thread_interrupted ex)//catches en rethrows interrupted
            {
                throw ex;
            }
            catch(const std::exception& e) //catches the rest of the exceptions
            {
                this->currentOperation->GetNotificationHandler()->Fatal(e.what());
            }
            catch(...)
            {
                this->currentOperation->GetNotificationHandler()->Fatal("Unkown error happened (developer: all "
                                                                                "exceptions thrown should inherit from "
                                                                                "std::exception).");
            }

            {//remove the current operation
                boost::unique_lock<boost::recursive_mutex> lockCurrentOp(this->currentOperationMutex);
                this->currentOperation = nullptr;
            }
            this_thread::interruption_point();
        }
    }
    catch(thread_interrupted ex)
    {
        std::cout << "interupted worker thread" << std::endl;
    }
}

void BackgroundWorker::StopCurrentOperation()
{
    boost::unique_lock<boost::recursive_mutex> lock(this->currentOperationMutex);
    this->currentOperation->Cancel();
}

void BackgroundWorker::SetDefaultNotificationHandler(std::shared_ptr<NotificationsHandler> notificationHandler)
{
    boost::unique_lock<boost::mutex> lock(this->defaultNotificationHandlerMutex);
    this->defaultNotificationHandler = notificationHandler;
}


LongOperation::LongOperation(): cancels(false), notifications(nullptr)
{

}

void LongOperation::Update()
{
    this_thread::interruption_point();
    if(this->cancels)
        throw CancelLongOperationException();
}

bool LongOperation::Cancel()
{
    this->cancels = true;
    return true;
}
