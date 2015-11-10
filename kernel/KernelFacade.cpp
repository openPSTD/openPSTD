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
//      Michiel Fortuijn
//
//////////////////////////////////////////////////////////////////////////

#include "KernelFacade.h"
#include "kernel/core/Scene.h"

//-----------------------------------------------------------------------------
// interface of the kernel

void KernelFacade::Configure(KernelConfiguration configuration)
{
    this->configuration = configuration;
}

void KernelFacade::Run(std::shared_ptr<PSTDFileConfiguration> config, KernelFacadeCallback* callback)
{
    // TODO: discuss how to handle the callback

    std::shared_ptr<Kernel::Scene> cur_scene;
    cur_scene = std::make_shared<Kernel::Scene>(Kernel::Scene(config));

    int solver_num = config->Settings.GetGPUAccel() + config->Settings.GetMultiThread() << 1;
    Kernel::Solver *solver;
    switch(solver_num) {
        case 0:
            solver = new Kernel::SingleThreadSolver(cur_scene);
            break;
        case 1:
            solver = new Kernel::GPUSingleThreadSolver(cur_scene);
            break;
        case 2:
            solver = new Kernel::MultiThreadSolver(cur_scene);
            break;
        case 3:
            solver = new Kernel::GPUMultiThreadSolver(cur_scene);
            break;
        default:
            break;
    }
}
