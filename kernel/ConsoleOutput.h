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
// Created by michiel on 12-7-2015.
//

#ifndef OPENPSTD_CONSOLEOUTPUT_H
#define OPENPSTD_CONSOLEOUTPUT_H

#include "PSTDKernel.h"

class ConsoleOutput : public KernelCallback {
    /**
     * Pass data to console application
     * @see KernelCallback.Callback()
     */
    virtual void Callback(CALLBACKSTATUS status, std::string message, int frame) override;

    /**
     * Write frame data or plots to console application
     * @see KernelCallback.WriteFrame()
     */
    virtual void WriteFrame(int frame, std::string domain, PSTD_FRAME_PTR data) override;

    virtual void WriteSample(int startSample, int receiver, std::vector<float> data) override;

};

#endif //OPENPSTD_CONSOLEOUTPUT_H
