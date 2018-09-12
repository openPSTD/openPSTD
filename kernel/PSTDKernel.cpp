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
#include <boost/lexical_cast.hpp>

namespace OpenPSTD {
    namespace Kernel {

        using namespace std;
//-----------------------------------------------------------------------------
// interface of the kernel

        void PSTDKernel::initialize_kernel(std::shared_ptr<PSTDConfiguration> config) {
            using namespace Kernel;
            debug("Initializing kernel");
            this->config = config;
            this->settings = make_shared<PSTDSettings>(config->Settings);
            this->wnd = make_shared<WisdomCache>();
            this->scene = make_shared<Scene>(this->settings);

            this->initialize_scene();
            debug("Finished initializing kernel");
        }


        void PSTDKernel::initialize_scene() {
            using namespace Kernel;
            debug("Initializing scene");
            this->add_domains();
            this->add_speakers();
            this->add_receivers();
            //this->calcwind();
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
                int domain_id = scene->get_new_id();
                shared_ptr<Kernel::Domain> domain_ptr = std::make_shared<Kernel::Domain>(
                        this->settings, domain_id, default_alpha, grid_top_left,
                        grid_size, false, this->wnd, edge_param_map, nullptr);
                domains.push_back(domain_ptr);
                domain_id_int++;
            }


            for (auto domain: domains) {
                scene->add_domain(domain);
            }
            scene->add_pml_domains();
            for (auto domain:scene->domain_list) {
                domain->post_initialization();
                cout << *domain << endl;

            }
        }

       /*void PSTDKernel::calcwind() {
           double_t bottomy = 0;
           double_t topy = 0;
           double_t rightx = 0;
           double_t leftx = 0;
           for (auto domain: scene->domain_list) {

               if (bottomy < domain->bottom_right.y) {
                   bottomy = domain->bottom_right.y;
               }
               if (rightx < domain->bottom_right.x) {
                   rightx = domain->bottom_right.x;
               }
               if (topy > domain->top_left.y) {
                   topy = domain->top_left.y;
               }

               if (leftx > domain->top_left.x) {
                   leftx = domain->top_left.x;
               }
               //cout<<endl<< rightx<<" "<<leftx<<endl;

           }
           cout << "The grid spacing is " << scene->settings->GetGridSpacing() << endl;
           cout << "The size is " << (bottomy - topy) / scene->settings->GetGridSpacing() << endl;
           int gridy = (bottomy - topy) / scene->settings->GetGridSpacing() + 1;
           int gridx = (rightx - leftx) / scene->settings->GetGridSpacing() + 1;

           // cout<<"The size of x is "<< gridy<<endl;
           //  cout<<"The size of y is"<< gridx<<endl;
           double z[gridy], z_stag[gridy];

           double *u, *du, *u_stag, *du_stag;
           u = new double[gridy];
           du = new double[gridy];
           wind.umat = new double *[gridy];
           wind.dumat = new double *[gridy];
           u_stag = new double[gridy];
           du_stag = new double[gridy];
           wind.umat_stag= new double *[gridy];
           wind.dumat_stag = new double *[gridy];
           for (int i = 0; i < gridy; i++) {

               wind.umat[i] = new double[gridx];
               wind.dumat[i] = new double[gridx];
               wind.umat_stag[i] = new double[gridx];
               wind.dumat_stag[i] = new double[gridx];
           }
           double zo = 1e-5, k = 0.4, ustar = 0.3;
           double Bo = 0.015, L, x[gridy], siw[gridy], x_stag[gridy], siw_stag[gridy];
           for (int i = 0; i <= gridy; i++) {
               z[i] = topy + i * scene->settings->GetGridSpacing();
               z_stag[i] = topy + (scene->settings->GetGridSpacing() / 2) + i * scene->settings->GetGridSpacing();
               if (z[i] <= 0) {
                   z[i] = zo;
               }
               x[i] = pow(1 - (16 * z[i] / L), 1 / 4);
               x_stag[i] = pow(1 - (16 * z_stag[i] / L), 1 / 4);
               u[i] = abs((ustar / k) * (log(z[i] / zo)));
               u_stag[i] = abs((ustar / k) * (log(z_stag[i] / zo)));
               du[i] = z[i] * ustar / k;
               du_stag[i] = z[i] * ustar / k;

               //cout<<u[i]<< " ";
           }//cout<<endl;


           for (int i = 0; i < gridy; ++i) {
               for (int j = 0; j < gridx; j++) {
                   wind.umat[i][j] = u[i];
                   wind.umat_stag[i][j] = u_stag[i];
                   wind.dumat[i][j] = du[i];
                   wind.dumat_stag[i][j] = du_stag[i];

               }
           }

       }*/





        void PSTDKernel::add_speakers() {
            using namespace Kernel;

            //Inconsistent: We created domains in this class, and speakers in the scene class
            for (auto speaker: this->config->Speakers) {
                vector<float> location = scale_to_grid(speaker);
                debug("Initializing Speaker (" + to_string(location.at(0)) + ", " + to_string(location.at(1)) + ")");


                this->scene->add_speaker(location.at(0), location.at(1), 0); // Z-coordinate is 0


            }
        }

        void PSTDKernel::add_receivers() {
            using namespace Kernel;
            //Inconsistent: We created domains in this class, and receivers in the scene class
            for (unsigned long i = 0; i < this->config->Receivers.size(); i++) {
                auto receiver = this->config->Receivers.at(i);
                vector<float> location = scale_to_grid(receiver);
               this->scene->add_receiver(location.at(0), location.at(1), 0, i);
            }
        }

        void PSTDKernel::run(KernelCallback *callback) {
            if (!config)
                throw PSTDKernelNotConfiguredException();

            using namespace Kernel;
            int solver_num = this->config->Settings.GetGPUAccel() + (this->config->Settings.GetMultiThread() << 1);
            std::shared_ptr<Kernel::Solver> solver;
            switch (solver_num) {
                case 0:
                    solver = std::make_shared<Kernel::SingleThreadSolver>(this->scene, callback);
                    break;
                case 1:
                    solver = std::make_shared<Kernel::GPUSingleThreadSolver>(this->scene, callback);
                    break;
                case 2:
                    solver = std::make_shared<Kernel::MultiThreadSolver>(this->scene, callback);
                    break;
                case 3:
                    solver = std::make_shared<Kernel::GPUMultiThreadSolver>(this->scene, callback);
                    break;
                default:
                    //TODO Raise Error
                    break;
            }
            solver->compute_propagation();
        }

        std::shared_ptr<Kernel::Scene> PSTDKernel::get_scene() {
            return this->scene;
        }

        SimulationMetadata PSTDKernel::get_metadata() {
            if (!config)
                throw PSTDKernelNotConfiguredException();

            SimulationMetadata result;
            int ndomains = (int) this->scene->domain_list.size();
            for (int i = 0; i < ndomains; i++) {
                Kernel::Point dsize = this->scene->domain_list[i]->size;
                std::vector<int> dimensions = {dsize.x, dsize.y, dsize.z};
                result.DomainMetadata.push_back(dimensions);
            }

            result.Framecount = (int) (this->settings->GetRenderTime() / this->settings->GetTimeStep());
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

        map<Kernel::Direction, Kernel::EdgeParameters> PSTDKernel::translate_edge_parameters(DomainConf domain) {
            using namespace Kernel;
            map<Direction, EdgeParameters> edge_parameters;
            edge_parameters[Direction::LEFT] = {domain.L.LR, domain.L.Absorption};
            edge_parameters[Direction::RIGHT] = {domain.R.LR, domain.R.Absorption};
            edge_parameters[Direction::TOP] = {domain.T.LR, domain.T.Absorption};
            edge_parameters[Direction::BOTTOM] = {domain.B.LR, domain.B.Absorption};
            return edge_parameters;
        }

        vector<int> PSTDKernel::round_off(vector<float> vector) {
            assert(vector.size() == 2); // We don't need to be very general here.
            return std::vector<int>{(int) vector.at(0), (int) vector.at(1)};
        }



    }
}
