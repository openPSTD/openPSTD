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
// Date: 11-4-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#include "SimulateLOperation.h"
#include <kernel/PSTDKernel.h>
#include <kernel/MockKernel.h>
#include "../../Model.h"

using namespace OpenPSTD::Kernel;
using namespace OpenPSTD::GUI;

SimulateLOperation::SimulateLOperation():
    started(false),
    finished(false),
    currentFrame(0)
{

}

std::string SimulateLOperation::GetName()
{
    return "Simulate";
}

float SimulateLOperation::GetProgress()
{
    return this->currentFrame/(float)this->metadata.Framecount;
}

void SimulateLOperation::Run(const Reciever &reciever)
{
    this->started = true;
    this->pstdFileAccess = reciever.model->documentAccess;
    std::shared_ptr<PSTDConfiguration> conf;
    {
        auto doc = this->pstdFileAccess->GetDocument();

        //make room in the document for results
        doc->DeleteResults();
        doc->InitializeResults();

        //get the configuration
        conf = doc->GetResultsSceneConf();
    }//make sure the doc ptr does not exist (release of the lock)
    //create and initialize kernel
    std::unique_ptr<KernelInterface> kernel;
    if(reciever.model->settings->UseMockKernel)
    {
        kernel = std::unique_ptr<MockKernel>(new MockKernel());
    }
    else
    {
        kernel = std::unique_ptr<PSTDKernel>(new PSTDKernel(reciever.model->settings->GPUAcceleration,
                                                            reciever.model->settings->CPUAcceleration));
    }

    kernel->initialize_kernel(conf);

    //get metadata
    metadata = kernel->get_metadata();
    //execute kernel
    kernel->run(this->shared_from_this());
    this->finished = true;
}

bool SimulateLOperation::Started()
{
    return started;
}

bool SimulateLOperation::Finished()
{
    return finished;
}

void SimulateLOperation::Callback(CALLBACKSTATUS status, std::string message,
                                                 int frame)
{
}

void SimulateLOperation::WriteFrame(int frame, int domain, PSTD_FRAME_PTR data)
{
    this->Update();
    if(frame >= 0)
    {
        this->currentFrame = frame;
    }
    this->pstdFileAccess->GetDocument()->SaveNextResultsFrame(domain, data);
}

void SimulateLOperation::WriteSample(int startSample, int receiver, std::vector<float> data)
{
    this->Update();
    Kernel::PSTD_RECEIVER_DATA_PTR data_ptr = std::make_shared<Kernel::PSTD_RECEIVER_DATA>(data);
    this->pstdFileAccess->GetDocument()->SaveReceiverData(receiver, data_ptr);
}

void SimulateLOperation::Fatal(std::string message)
{
    this->notifications->Fatal(message);
}

void SimulateLOperation::Error(std::string message)
{
    this->notifications->Error(message);
}

void SimulateLOperation::Warning(std::string message)
{
    this->notifications->Warning(message);
}

void SimulateLOperation::Info(std::string message)
{
    this->notifications->Info(message);
}

void SimulateLOperation::Debug(std::string message)
{
    this->notifications->Debug(message);
}




















