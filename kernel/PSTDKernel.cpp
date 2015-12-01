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
//      Omar  Richardson
//
//////////////////////////////////////////////////////////////////////////

#include "PSTDKernel.h"


//-----------------------------------------------------------------------------
// interface of the kernel

PSTDKernel::PSTDKernel(std::shared_ptr<PSTDFileConfiguration> config) {
    this->config = config;
    this->settings = std::make_shared<PSTDFileSettings>(config->Settings);
    Kernel::Scene scene(this->config);
    this->scene = std::make_shared<Kernel::Scene>(scene);
    this->initialize_scene();
}


void PSTDKernel::initialize_scene() {
    float dx = this->settings->GetGridSpacing();
    float dy = dx;
    for (auto domain: this->config->Domains) {
        // This is not a reference to the data. Where is the screen description?
        //Todo: Implement
        //std::shared_ptr<Kernel::Domain> domain_ptr = std::make_shared<Kernel::Domain>(domain);
        //scene.add_domain(domain_ptr); Inaccessible base?
    }
    scene->add_pml_domains();
    this->add_speakers();
    this->add_receivers();
    scene->compute_rho_arrays();
    scene->compute_pml_matrices();
}

void PSTDKernel::add_speakers() {
    //Implement. Focus on (1) grid coordinates vs world coordinates and (2) roundoff
}

void PSTDKernel::add_receivers() {
    //Implement. Focus on (1) grid coordinates vs world coordinates and (2) roundoff
}

void PSTDKernel::run(KernelCallback *callback)
{
    // TODO: discuss how to handle the callback

    std::shared_ptr<Kernel::Scene> cur_scene;
    if (this->config == nullptr) {
        assert(false); //Kernel not initialized yet. Maybe config in constructor?
    }

    int solver_num = this->config->Settings.GetGPUAccel() + this->config->Settings.GetMultiThread() << 1;
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
