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

#ifndef OPENPSTD_SIMULATELOPERATION_H
#define OPENPSTD_SIMULATELOPERATION_H

#include "LongOperationRunner.h"
#include <kernel/KernelInterface.h>
#include <shared/PSTDFileAccess.h>

namespace OpenPSTD
{
    namespace GUI
    {

        /**
         * Basic long operation for runnning the OpenPSTD kernel
         */
        class SimulateLOperation : public LongOperation, public OpenPSTD::Kernel::KernelCallback,
                                   public std::enable_shared_from_this<SimulateLOperation>
        {
        private:
            std::shared_ptr<OpenPSTD::Shared::PSTDFileAccess> pstdFileAccess;
            OpenPSTD::Kernel::SimulationMetadata metadata;
            int currentFrame;
            bool started;
            bool finished;

        public:
            SimulateLOperation();

            //interface of LongOperation and BaseOperation
            std::string GetName();
            float GetProgress();
            void Run(const Reciever &reciever) override;
            bool Started() override;
            bool Finished() override;


            //implementation of KernelCallback
            void Callback(OpenPSTD::Kernel::CALLBACKSTATUS status, std::string message, int frame) override;
            void WriteFrame(int frame, int domain, OpenPSTD::Kernel::PSTD_FRAME_PTR data) override;
            void WriteSample(int startSample, int receiver, std::vector<float> data) override;
            virtual void Fatal(std::string message) override;
            virtual void Error(std::string message) override;
            virtual void Warning(std::string message) override;
            virtual void Info(std::string message) override;
            virtual void Debug(std::string message) override;
        };
    }
}

#endif //OPENPSTD_SIMULATELOPERATION_H
