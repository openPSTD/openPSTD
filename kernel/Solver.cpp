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
// along with openPSTD.  If not, see <http://www.gnu.org/licenses/\>.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Date:
//      2-11-15
//
// Authors:
//      Louis van Harten, Omar Richardson, Michiel Fortuin
//
//////////////////////////////////////////////////////////////////////////

#include "Solver.h"
#include <boost/lexical_cast.hpp>

namespace OpenPSTD {
    namespace Kernel {
        Solver::Solver(std::shared_ptr<Scene> scene, std::shared_ptr<KernelCallback> callback) {
            this->scene = scene;
            this->settings = scene->settings;
            this->callback = callback;
            this->callback->Debug("Number of render time: " + boost::lexical_cast<std::string>(this->settings->GetRenderTime()));
            this->callback->Debug("Size of time step: " + boost::lexical_cast<std::string>(this->settings->GetTimeStep()));

            this->number_of_time_steps = (int) (this->settings->GetRenderTime() / this->settings->GetTimeStep());
        }

        SingleThreadSolver::SingleThreadSolver(std::shared_ptr<Scene> scene, std::shared_ptr<KernelCallback> callback) : Solver::Solver(
                scene, callback) {
        }

        GPUSingleThreadSolver::GPUSingleThreadSolver(std::shared_ptr<Scene> scene, std::shared_ptr<KernelCallback> callback)
                : Solver::Solver(scene, callback) {
        }

        MultiThreadSolver::MultiThreadSolver(std::shared_ptr<Scene> scene, std::shared_ptr<KernelCallback> callback) : SingleThreadSolver::SingleThreadSolver(
                scene,
                callback) {
        }

        GPUMultiThreadSolver::GPUMultiThreadSolver(std::shared_ptr<Scene> scene, std::shared_ptr<KernelCallback> callback)
                : Solver::Solver(
                scene, callback) {
        }

        void SingleThreadSolver::compute_propagation() {
            this->callback->Info("Starting simulation");

            for (int frame = 0; frame < this->number_of_time_steps; frame++) {
                compute_timestep(frame);
            }
            this->callback->Info("Succesfully finished simulation");
        }

        void SingleThreadSolver::compute_timestep(int frame)
        {
            for (auto domain:this->scene->domain_list) {
                domain->push_values();
                //std::cout << *domain << std::endl;
            }
            for (unsigned long rk_step = 0; rk_step < 6; rk_step++) {
                compute_rk_step(frame, rk_step);
            }
            for (auto domain:this->scene->domain_list) {
                if (frame % this->settings->GetSaveNth() == 0 and not domain->is_pml) {
                    this->callback->WriteFrame(frame, domain->id, this->get_pressure_vector(domain));
                }
            }
            this->scene->apply_pml_matrices();
            for (auto receiver:this->scene->receiver_list) {
                receiver->compute_local_pressure();
                if (frame % this->settings->GetSaveNth() == 0) {
                    this->callback->WriteSample(frame, (int) receiver->id, *this->get_receiver_pressure(receiver));
                }
            }
            this->callback->Info("Finished frame: " + boost::lexical_cast<std::string>(frame));
        }

        void SingleThreadSolver::compute_rk_step(int frame, int rk_step)
        {
            for (Kernel::CalcDirection calc_dir: Kernel::all_calc_directions) {
                for (Kernel::CalculationType calc_type: Kernel::all_calculation_types) {
                    for (auto domain:this->scene->domain_list) {
                        //std::cout << *domain << std::endl;
                        if (not domain->is_rigid()) {
                            if (domain->should_update[calc_dir]) {
                                domain->calc(calc_dir, calc_type);
                            }
                        }
                    }
                }
            }
            for (auto domain:this->scene->domain_list) {
                if (not domain->is_rigid()) {
                    this->update_field_values(domain, rk_step, frame);
                }
            }
            for (auto domain:this->scene->domain_list) {
                domain->current_values.p0 = domain->current_values.px0 + domain->current_values.py0;
            }
        }

        void MultiThreadSolver::compute_rk_step(int frame, int rk_step)
        {
            #pragma omp parallel
            {
                #pragma omp single nowait
                {
                    for (Kernel::CalcDirection calc_dir: Kernel::all_calc_directions) {
                        for (Kernel::CalculationType calc_type: Kernel::all_calculation_types) {
                            for (auto domain:this->scene->domain_list) {
                                //std::cout << *domain << std::endl;
                                #pragma omp task
                                {
                                    if (not domain->is_rigid()) {
                                        if (domain->should_update[calc_dir]) {
                                            domain->calc(calc_dir, calc_type);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            for (auto domain:this->scene->domain_list) {
                if (not domain->is_rigid()) {
                    this->update_field_values(domain, rk_step, frame);
                }
            }
            for (auto domain:this->scene->domain_list) {
                domain->current_values.p0 = domain->current_values.px0 + domain->current_values.py0;
            }
        }

        void GPUSingleThreadSolver::compute_propagation() {
            this->callback->Error("!! GPU SOLVER NOT YET IMPLEMENTED !! ");
            //TODO
        }
        void GPUMultiThreadSolver::compute_propagation() {
            this->callback->Error("!! GPU+MCPU SOLVER NOT YET IMPLEMENTED !! ");
            //TODO
        }

        void Solver::update_field_values(std::shared_ptr<Domain> domain, unsigned long rk_step,
                                         unsigned long frame) { // frame is temp
            float dt = this->settings->GetTimeStep();
            float c1_square = this->settings->GetSoundSpeed() * this->settings->GetSoundSpeed();
            std::vector<float> coefs = this->settings->GetRKCoefficients();
            if (!domain->is_pml) {
                //write_array_to_file(domain->current_values.px0+domain->current_values.py0, "pressure_tot", frame * 6 + rk_step);
                //write_array_to_file(domain->current_values.px0, "pressure_x", frame * 6 + rk_step);
                //write_array_to_file(domain->current_values.py0, "pressure_y", frame * 6 + rk_step);
                //write_array_to_file(domain->current_values.vx0, "velocity_x", frame * 6 + rk_step);
                //write_array_to_file(domain->current_values.vy0, "velocity_y", frame * 6 + rk_step);
            }
            domain->current_values.vx0 =
                    domain->previous_values.vx0 - dt * coefs.at(rk_step) * (domain->l_values.Lpx / domain->rho);
            domain->current_values.vy0 =
                    domain->previous_values.vy0 - dt * coefs.at(rk_step) * (domain->l_values.Lpy / domain->rho);
            domain->current_values.px0 = domain->previous_values.px0 - dt * coefs.at(rk_step) *
                                                                       (domain->l_values.Lvx * domain->rho * c1_square);
            domain->current_values.py0 = domain->previous_values.py0 - dt * coefs.at(rk_step) *
                                                                       (domain->l_values.Lvy * domain->rho * c1_square);

            /*if (!domain->is_pml) {
                this->callback->Callback(CALLBACKSTATUS::RUNNING, "Subframe " + boost::lexical_cast<std::string>(rk_step), 0);
                //write_array_to_file(domain->l_values.Lpx, "pressure_deriv_x", frame * 6 + rk_step);
                //write_array_to_file(domain->l_values.Lpy, "pressure_deriv_y", frame * 6 + rk_step);
                //write_array_to_file(domain->l_values.Lvx, "velocity_deriv_x", frame * 6 + rk_step);
                //write_array_to_file(domain->l_values.Lvy, "velocity_deriv_y", frame * 6 + rk_step);
                this->callback->Callback(CALLBACKSTATUS::RUNNING,
                                         "vx0 max: " + boost::lexical_cast<std::string>(domain->current_values.vx0.maxCoeff()), 0);
                this->callback->Callback(CALLBACKSTATUS::RUNNING,
                                         "vy0 max: " + boost::lexical_cast<std::string>(domain->current_values.vy0.maxCoeff()), 0);
                this->callback->Callback(CALLBACKSTATUS::RUNNING,
                                         "px0 max: " + boost::lexical_cast<std::string>(domain->current_values.px0.maxCoeff()), 0);
                this->callback->Callback(CALLBACKSTATUS::RUNNING,
                                         "py0 max: " + boost::lexical_cast<std::string>(domain->current_values.py0.maxCoeff()), 0);
                this->callback->Callback(CALLBACKSTATUS::RUNNING,
                                         "Lpx max: " + boost::lexical_cast<std::string>(domain->l_values.Lpx.maxCoeff()), 0);
                this->callback->Callback(CALLBACKSTATUS::RUNNING,
                                         "Lpy max: " + boost::lexical_cast<std::string>(domain->l_values.Lpy.maxCoeff()), 0);
                this->callback->Callback(CALLBACKSTATUS::RUNNING,
                                         "Lvx max: " + boost::lexical_cast<std::string>(domain->l_values.Lvx.maxCoeff()), 0);
                this->callback->Callback(CALLBACKSTATUS::RUNNING,
                                         "Lvy max: " + boost::lexical_cast<std::string>(domain->l_values.Lvy.maxCoeff()), 0);
            }*/
        }

        PSTD_FRAME_PTR Solver::get_pressure_vector(std::shared_ptr<Domain> domain) {
            auto aligned_pressure = std::make_shared<PSTD_FRAME>();
            aligned_pressure->reserve((unsigned long) domain->size.x * domain->size.y);
            for (unsigned long i=0;i<domain->size.y;i++) {
                for (unsigned long j=0;j<domain->size.x;j++) {
                    aligned_pressure->push_back(domain->current_values.p0(i,j));
                }
            }
            return aligned_pressure;
        }

        PSTD_FRAME_PTR Solver::get_receiver_pressure(std::shared_ptr<Receiver> receiver) {
            auto pressure_vector = std::make_shared<PSTD_FRAME>();
            pressure_vector->push_back(receiver->received_values.back());
            return pressure_vector;
        }
    }
}
