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
//      Louis van Harten, Omar Richardson
//
//////////////////////////////////////////////////////////////////////////

#include "Solver.h"

namespace OpenPSTD {
    namespace Kernel {
        Solver::Solver(std::shared_ptr<Scene> scene, KernelCallback *callback) {
            this->scene = scene;
            this->settings = scene->settings;
            this->callback = callback;
            Kernel::debug("Number of render time: " + std::to_string(this->settings->GetRenderTime()));
            Kernel::debug("Number of time step: " + std::to_string(this->settings->GetTimeStep()));

            this->number_of_time_steps = (int) (this->settings->GetRenderTime() / this->settings->GetTimeStep());
        }

        SingleThreadSolver::SingleThreadSolver(std::shared_ptr<Scene> scene, KernelCallback *callback) : Solver::Solver(
                scene, callback) {
        }

        GPUSingleThreadSolver::GPUSingleThreadSolver(std::shared_ptr<Scene> scene, KernelCallback *callback)
                : Solver::Solver(scene, callback) {
        }

        MultiThreadSolver::MultiThreadSolver(std::shared_ptr<Scene> scene, KernelCallback *callback) : Solver::Solver(
                scene,
                callback) {
        }

        GPUMultiThreadSolver::GPUMultiThreadSolver(std::shared_ptr<Scene> scene, KernelCallback *callback)
                : Solver::Solver(
                scene, callback) {
        }


        // Todo: Overwrite solver for GPU/Multithreaded
        void Solver::compute_propagation() {
            this->callback->Callback(CALLBACKSTATUS::STARTING, "Starting simulation", -1);
            for (int frame = 0; frame < this->number_of_time_steps; frame++) {
                for (auto domain:this->scene->domain_list) {
                    domain->push_values();
                }
                for (unsigned long rk_step = 0; rk_step < 6; rk_step++) {
                    for (Kernel::CalcDirection calc_dir: Kernel::all_calc_directions) {
                        for (Kernel::CalculationType calc_type: Kernel::all_calculation_types) {
                            for (auto domain:this->scene->domain_list) {
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
                            this->update_field_values(domain, rk_step);
                        }
                    }
                    for (auto domain:this->scene->domain_list) {
                        domain->current_values.p0 = domain->current_values.px0 + domain->current_values.py0;
                        if (frame % this->settings->GetSaveNth() == 0 and not domain->is_pml) {
                            this->callback->WriteFrame(frame, domain->id, this->get_pressure_vector());
                        }
                    }
                }
                this->scene->apply_pml_matrices();
                for (auto receiver:this->scene->receiver_list) {
                    receiver->compute_local_pressure();
                    //Todo: Write this to a file or process in callback.
                }
                this->callback->Callback(CALLBACKSTATUS::RUNNING, "", frame);
            }
            this->callback->Callback(CALLBACKSTATUS::FINISHED, "Succesfully finished simulation",
                                     this->number_of_time_steps);
        }

        void Solver::update_field_values(std::shared_ptr<Domain> domain, unsigned long rk_step) {
            float dt = this->settings->GetTimeStep();
            float c1_square = this->settings->GetSoundSpeed() * this->settings->GetSoundSpeed();
            std::vector<float> coefs = this->settings->GetRKCoefficients();
            domain->current_values.vx0 =
                    domain->previous_values.vx0 - dt * coefs.at(rk_step) * (domain->l_values.Lpx / domain->rho).real();
            domain->current_values.vy0 =
                    domain->previous_values.vy0 - dt * coefs.at(rk_step) * (domain->l_values.Lpy / domain->rho).real();
            domain->current_values.px0 = domain->previous_values.px0 -
                                         dt * coefs.at(rk_step) *
                                         (domain->l_values.Lvx * domain->rho * c1_square).real();
            domain->current_values.py0 = domain->previous_values.py0 -
                                         dt * coefs.at(rk_step) *
                                         (domain->l_values.Lvy * domain->rho * c1_square).real();

        }

        PSTD_FRAME_PTR Solver::get_pressure_vector() {
            auto aligned_pressure = std::make_shared<PSTD_FRAME>();
            aligned_pressure->reserve((unsigned long) this->scene->size.x * this->scene->size.y);
            auto field = this->scene->get_pressure_field();
            unsigned long row_length = (unsigned long) this->scene->size.x;
            for (unsigned long row = 0; row < field.cols(); row++) {
                aligned_pressure->insert(aligned_pressure->end(),
                                         field.data() + row * row_length,
                                         field.data() + (row + 1) * row_length);
            }
            return aligned_pressure;
        }
    }
}