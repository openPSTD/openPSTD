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
//      Louis van Harten
//
//////////////////////////////////////////////////////////////////////////

#include <signal.h>
#include "PSTDKernel.h"


using namespace std;
//-----------------------------------------------------------------------------
// interface of the kernel

PSTDKernel::PSTDKernel(std::shared_ptr<PSTDFileConfiguration> config) {
    using namespace Kernel;
    debug("Initializing kernel");
    this->config = config;
    this->settings = make_shared<PSTDFileSettings>(config->Settings);
    this->scene = make_shared<Scene>(this->settings);
    this->initialize_scene();
    this->wnd = make_shared<WaveNumberDiscretizer>();
    debug("Finished initializing kernel");
}


void PSTDKernel::initialize_scene() {
    using namespace Kernel;
    debug("Initializing scene");
    this->add_domains();
    scene->add_pml_domains();
    this->add_speakers();
    this->add_receivers();
    scene->compute_rho_arrays();
    scene->compute_pml_matrices();
    debug("Finished initializing");
}


void PSTDKernel::add_domains() {
    int domain_id_int = 0;
    vector<shared_ptr<Kernel::Domain>> domains;
    for (auto domain: this->config->Domains) {
        Kernel::debug("Initializing domain " + to_string(domain_id_int));
        vector<float> tl = scale_to_grid(domain.TopLeft);
        vector<float> s = scale_to_grid(domain.Size);
        Kernel::Point grid_top_left((int) tl.at(0), (int) tl.at(1));
        Kernel::Point grid_size((int) s.at(0), (int) s.at(1));
        map<Kernel::Direction, Kernel::EdgeParameters> edge_param_map = translate_edge_parameters(domain);
        string domain_id = "Domain" + to_string(domain_id_int);
        shared_ptr<Kernel::Domain> domain_ptr(
                new Kernel::Domain(this->settings, domain_id, default_alpha, grid_top_left,
                                   grid_size, false, this->wnd, edge_param_map, nullptr));
        domains.push_back(domain_ptr);
        domain_id_int++;
    }
    for (auto domain: domains) {
        domain->post_initialization();
        scene->add_domain(domain);
    }
}


void PSTDKernel::add_speakers() {
    using namespace Kernel;
    //Inconsistent: We created domains in this class, and speakers in the scene class
    for (auto speaker: this->config->Speakers) {
        vector<float> location = scale_to_grid(speaker);
        debug(
                "Initializing Speaker (" + to_string(location.at(0)) + ", " + to_string(location.at(1)) + ")");
        this->scene->add_speaker(location.at(0), location.at(1), 0); // Z-coordinate is 0
    }
}

void PSTDKernel::add_receivers() {
    using namespace Kernel;
    //Inconsistent: We created domains in this class, and receivers in the scene class
    for (auto receiver: this->config->Receivers) {
        vector<float> location = scale_to_grid(receiver);
        this->scene->add_receiver(location.at(0), location.at(1), 0);
    }
}

void PSTDKernel::run(KernelCallback *callback) {
    using namespace Kernel;
    int solver_num = this->config->Settings.GetGPUAccel() + (this->config->Settings.GetMultiThread() << 1);
    std::shared_ptr<Kernel::Solver> solver;
    switch (solver_num) {
        case 0:
            solver = std::shared_ptr<Kernel::Solver>(new Kernel::SingleThreadSolver(this->scene, callback));
            break;
        case 1:
            solver = std::shared_ptr<Kernel::Solver>(new Kernel::GPUSingleThreadSolver(this->scene, callback));
            break;
        case 2:
            solver = std::shared_ptr<Kernel::Solver>(new Kernel::MultiThreadSolver(this->scene, callback));
            break;
        case 3:
            solver = std::shared_ptr<Kernel::Solver>(new Kernel::GPUMultiThreadSolver(this->scene, callback));
            break;
        default:
            //TODO Raise Error
            break;
    }
    solver->compute_propagation();
}

std::vector<std::vector<int>> PSTDKernel::GetDomainMetadata() {
    int ndomains = this->scene->domain_list.size();
    std::vector<std::vector<int>> result;
    for(int i=0; i<ndomains; i++) {
        std::vector<int> dimensions;
        dimensions.push_back(this->scene->domain_list[i]->size.x);
        dimensions.push_back(this->scene->domain_list[i]->size.y);
        dimensions.push_back(this->scene->domain_list[i]->size.z);
        result.push_back(dimensions);
    }
    return result;
}


vector<float> PSTDKernel::scale_to_grid(QVector2D world_vector) {
    QVector2D scaled_vector = world_vector / this->settings->GetGridSpacing();
    return vector<float>{scaled_vector[0], scaled_vector[1]};
}

vector<float> PSTDKernel::scale_to_grid(QVector3D world_vector) {
    //Not yet adapted to 3D.
    QVector3D scaled_vector = world_vector / this->settings->GetGridSpacing();
    return vector<float>{scaled_vector[0], scaled_vector[1]};
}

map<Kernel::Direction, Kernel::EdgeParameters> PSTDKernel::translate_edge_parameters(Domain domain) {
    using namespace Kernel;
    map<Direction, EdgeParameters> edge_parameters;
    edge_parameters[Direction::LEFT] = {domain.L.LR, domain.L.Absorption};
    edge_parameters[Direction::RIGHT] = {domain.R.LR, domain.R.Absorption};
    edge_parameters[Direction::BOTTOM] = {domain.B.LR, domain.B.Absorption};
    edge_parameters[Direction::TOP] = {domain.T.LR, domain.T.Absorption};
    return edge_parameters;
}

vector<int> PSTDKernel::round_off(vector<float> vector) {
    assert(vector.size() == 2); // We don't need to be very general here.
    return std::vector<int>{(int) vector.at(0), (int) vector.at(1)};
}
