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

BackgroundWorker::BackgroundWorker(std::weak_ptr<ReceiverBuilder> builder)
{
    this->builder = builder;
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
            {
                boost::unique_lock<boost::mutex> lock(this->queueMutex);
                while(this->queue.size() == 0)
                {
                    queueCondition.wait(lock);
                }
                {
                    boost::unique_lock<boost::recursive_mutex> lockCurrentOp(this->currentOperationMutex);
                    this->currentOperation = this->queue.front();
                }
                this->queue.pop();
            }
            Reciever r;
            {
                std::shared_ptr<ReceiverBuilder> builder = this->builder.lock();
                r = builder->BuildReceiver();
            }//make sure the shared_ptr to builder is removed
            this->currentOperation->Run(r);
            {
                boost::unique_lock<boost::recursive_mutex> lockCurrentOp(this->currentOperationMutex);
                this->currentOperation = nullptr;
            }
            this_thread::interruption_point();
        }
    }
    catch(thread_interrupted ex)
    {
        std::cout << "interupted worker thread" << std::endl;
        //todo do something with this exception
    }
}

void LongOperation::Update()
{
    this_thread::interruption_point();
}

