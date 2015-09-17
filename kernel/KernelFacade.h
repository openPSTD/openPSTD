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

#ifndef OPENPSTD_KERNELFACADE_H
#define OPENPSTD_KERNELFACADE_H

#include <string>

/**
 * The configuration of the kernel.
 */
struct KernelConfiguration {
    bool multiThreaded;
    bool gpuAccelerated;
    bool writePlot;
    bool writeArray;
};

/**
 * The status of the kernel when the callback is called.
 */
enum CALLBACKSTATUS
{
    CALLBACKSTATUS_ERROR,
    CALLBACKSTATUS_STARTING,
    CALLBACKSTATUS_RUNNING,
    CALLBACKSTATUS_SUCCES,
};

class KernelFacadeCallback
{
public:
    /**
     * This callback will be called with information how far the kernel is progressed.
     * Frame is only used with status == CALLBACKSTATUS_RUNNING, else it's -1.
     */
    virtual void Callback(CALLBACKSTATUS status, std::string message, int frame) = 0;
};

/**
 * A facade before the kernel. This is a simlpe C++ api that can be used for calling the kernel.
 */
class KernelFacade
{
private:
    KernelConfiguration configuration;

    void OutputPythonHomeAndPath();

public:
    /**
     * Configures the kernel
     */
    void Configure(KernelConfiguration configuration);

    /**
     * Runs the kernel with a certain scene file. The callback has a single function that informs the rest of the
     * application how the progress is of the kernel.
     */
    void Run(std::string sceneFile, KernelFacadeCallback* callback);
};


#endif //OPENPSTD_KERNELFACADE_H
