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
    this->add_domains();
    scene->add_pml_domains();
    this->add_speakers();
    this->add_receivers();
    scene->compute_rho_arrays();
    scene->compute_pml_matrices();
}


void PSTDKernel::add_domains() {
    for (auto domain: this->config->Domains) {
        // This is not a reference to the data. Where is the screen description?
        //Todo: Implement
        //std::shared_ptr<Kernel::Domain> domain_ptr = std::make_shared<Kernel::Domain>(domain);
        //scene.add_domain(domain_ptr); Inaccessible base?
    }
}


void PSTDKernel::add_speakers() {
    for (auto speaker: this->config->Speakers) {
        //see domain.
        std::vector<float> location; //= speaker.location ofzo
        //Kernel::Speaker speaker = Speaker(location)
        //std::shared_ptr<Kernel::Speaker> speaker_ptr = std::make_shared<Kernel::Speaker>(speaker)
    }
}

void PSTDKernel::add_receivers() {
    for (auto receiver: this->config->Receivers) {
        //see domain.
        std::vector<float> location; //= receiver.location ofzo
        //Kernel::Receivers receiver = Receivers(location)
        //std::shared_ptr<Kernel::Receivers> receiver_ptr = std::make_shared<Kernel::Receivers>(speaker)
    }
}

void PSTDKernel::run(KernelCallback *callback)
{
    // TODO: discuss how to handle the callback. It probably should be passed to the solver.


    int solver_num = this->config->Settings.GetGPUAccel() + this->config->Settings.GetMultiThread() << 1;
    Kernel::Solver *solver;
    switch(solver_num) {
        case 0:
            solver = new Kernel::SingleThreadSolver(this->scene);
            break;
        case 1:
            solver = new Kernel::GPUSingleThreadSolver(this->scene);
            break;
        case 2:
            solver = new Kernel::MultiThreadSolver(this->scene);
            break;
        case 3:
            solver = new Kernel::GPUMultiThreadSolver(this->scene);
            break;
        default:
            break;
    }

}
