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

#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <GUI/operations/long/LongOperationRunner.h>

using namespace OpenPSTD::GUI;

class MockReceiverBuilder: public ReceiverBuilder
{
public:
    Reciever BuildReceiver(){ return Reciever(); }

};

class MockLongRunningOperation: public LongOperation
{
private:
    float progress = 0;

public:
    boost::condition_variable started;
    boost::condition_variable end;
    boost::condition_variable ended;

    std::string GetName(){ return "mock"; }
    float GetProgress(){ return progress; }
    bool Cancel(){};
    void Run(const Reciever &reciever)
    {
        boost::mutex mutex;
        boost::unique_lock<boost::mutex> lock(mutex);

        progress = 0.5f;
        started.notify_all();
        end.wait(lock);
        progress = 1;
        ended.notify_all();
    }
};

BOOST_AUTO_TEST_SUITE(GUI_Long_Operations_Runner)

    BOOST_AUTO_TEST_CASE(Constructor)
    {
        std::shared_ptr<MockReceiverBuilder> builder = std::make_shared<MockReceiverBuilder>();
        BackgroundWorker worker(builder);
    }

    BOOST_AUTO_TEST_CASE(Start)
    {
        std::shared_ptr<MockReceiverBuilder> builder = std::make_shared<MockReceiverBuilder>();
        BackgroundWorker worker(builder);
        worker.Start();
        BOOST_CHECK(worker.IsIdle());
        worker.JoinASAP();
    }

    BOOST_AUTO_TEST_CASE(StartWithSleep)
    {
        std::shared_ptr<MockReceiverBuilder> builder = std::make_shared<MockReceiverBuilder>();
        BackgroundWorker worker(builder);
        worker.Start();
        BOOST_CHECK(worker.IsIdle());
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
        BOOST_CHECK(worker.IsIdle());
        worker.JoinASAP();
    }

    BOOST_AUTO_TEST_CASE(RunSingleOperation)
    {
        boost::mutex mutex;
        boost::unique_lock<boost::mutex> lock(mutex);

        std::shared_ptr<MockReceiverBuilder> builder = std::make_shared<MockReceiverBuilder>();

        //create background worker
        BackgroundWorker worker(builder);

        //create operation
        std::shared_ptr<MockLongRunningOperation> op = std::make_shared<MockLongRunningOperation>();

        //enqueue operation
        worker.EnqueueOperation(op);

        //start
        worker.Start();

        //wait that it started
        op->started.wait_for(lock, boost::chrono::milliseconds(1000));

        //test items
        BOOST_CHECK(op->GetProgress() == 0.5f);
        BOOST_CHECK(worker.GetCurrentName() == "mock");
        BOOST_CHECK(worker.GetCurrentProgress() == 0.5f);

        //end operation
        op->end.notify_all();
        op->ended.wait_for(lock, boost::chrono::milliseconds(1000));

        //wait so the worker can recover
        boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
        BOOST_CHECK(worker.IsIdle());

        worker.JoinASAP();
    }

    BOOST_AUTO_TEST_CASE(RunSingleOperationWithSleep)
    {
        boost::mutex mutex;
        boost::unique_lock<boost::mutex> lock(mutex);

        std::shared_ptr<MockReceiverBuilder> builder = std::make_shared<MockReceiverBuilder>();

        //create background worker
        BackgroundWorker worker(builder);
        //start
        worker.Start();

        //wait a while
        boost::this_thread::sleep_for(boost::chrono::seconds(1));

        //create operation
        std::shared_ptr<MockLongRunningOperation> op = std::make_shared<MockLongRunningOperation>();

        //enqueue operation
        worker.EnqueueOperation(op);

        //wait that it started
        op->started.wait_for(lock, boost::chrono::milliseconds(1000));

        //test items
        BOOST_CHECK(op->GetProgress() == 0.5f);
        BOOST_CHECK(worker.GetCurrentName() == "mock");
        BOOST_CHECK(worker.GetCurrentProgress() == 0.5f);

        //end operation
        op->end.notify_all();
        op->ended.wait_for(lock, boost::chrono::milliseconds(1000));

        //wait so the worker can recover
        boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
        BOOST_CHECK(worker.IsIdle());

        worker.JoinASAP();
    }

BOOST_AUTO_TEST_SUITE_END()