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
// Purpose:
//      The openPSTD solver. Subclasses should implement the various
//      solvers present in OpenPSTD v1.1
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_SOLVER_H
#define OPENPSTD_SOLVER_H

#include "KernelInterface.h"
#include "core/Scene.h"
#include "PSTDKernel.h"
#include <fftw3.h>

namespace OpenPSTD {
    namespace Kernel {

        /**
         * Component that computes the state variables of the scene for consecutive time steps.
         *
         * This is an abstract class, implemented in single/multi-threaded solvers.
         * Based on the settings and the scene, the solver repeatedly executes the
         * PSTD method to approximate the pressure and velocity.
         * The time integration is performed with a RK6 method described in <paper>.
         */
        class Solver {
        protected:
            /// Parameters and settings
            std::shared_ptr<PSTDSettings> settings;
            /// Scene (initialized before passed to the solver)
            std::shared_ptr<Scene> scene;

            std::shared_ptr<KernelCallback> callback;
            /**
             * The final number of computed frames
             */
            int number_of_time_steps;

            /**
             * Updates the pressure and velocity fields of the domains to the new values computed in the RK scheme
             * @param domain: Domain under consideration
             * @param rk_step: sub-step of RK6 method
             */
            void update_field_values(std::shared_ptr<Domain> domain, unsigned long rk_step, unsigned long frame);

            /**
             * The GUI format for pressure fields
             * @return PSTD_FRAME (shared pointer to float vector)
             */
            PSTD_FRAME_PTR get_pressure_vector(std::shared_ptr<Domain> domain);

            PSTD_FRAME_PTR get_receiver_pressure(std::shared_ptr<Receiver> receiver);

        public:
            /**
             * Solver constructor (abstract). Initialized parameters for running the openPSTD algorithm
             * @param scene: Pointer to scene object.
             * @param callback: Pointer to callback function
             * @return: New solver object.
             */
            Solver(std::shared_ptr<Scene> scene, std::shared_ptr<KernelCallback> callback);

            /**
             * Start the simulation solver.
             * Runs until the simulation is finished, but meanwhile makes calls to the callback.
             * A more badass name is welcome
             */
            virtual void compute_propagation() = 0;
        };

        /**
         * Default singlethreaded solver.
         */
        class SingleThreadSolver : public Solver {
        public:
            /**
             * Default constructor. Blocking call: will not return before the solver is done.
             * @see Solver
             */
            SingleThreadSolver(std::shared_ptr<Scene> scene, std::shared_ptr<KernelCallback> callback);

            /**
             * Single threaded implementation of the simulation solver
             */
            void compute_propagation();

            /**
             * compute a single timestep
             * @param frame
             */
            virtual void compute_timestep(int frame);
            /**
             * compute a single RK step
             * @param frame
             * @param rk_step
             */
            virtual void compute_rk_step(int frame, int rk_step);
        };

        /**
         * Solver that exploits the multiple CPU cores of a machine
         */
        class MultiThreadSolver : public SingleThreadSolver {
        public:
            /**
             * Multithreaded solver. This instance employs multiple CPU's
             * @see Solver
             */
            MultiThreadSolver(std::shared_ptr<Scene> scene, std::shared_ptr<KernelCallback> callback);

            void compute_rk_step(int frame, int rk_step) override;
        };

        /**
         * Solver that performs the computational intensive parts on a GPU
         */
        class GPUSingleThreadSolver : public Solver {
        public:
            /**
             * GPU solver. This instance runs the PSTD computations on the graphics card
             * @see Solver
             */
            GPUSingleThreadSolver(std::shared_ptr<Scene> scene, std::shared_ptr<KernelCallback> callback);

            /**
             * GPU-enabled implementation of the simulation solver
             */
            void compute_propagation();
        };

        /**
         * Solver that both utilized multiple cores and the GPU
         */
        class GPUMultiThreadSolver : public Solver {
        public:
            /**
             * Multithreaded GPU solver. This instance employs both multiple CPU's as well as the graphics card.
             * @see Solver
             */
            GPUMultiThreadSolver(std::shared_ptr<Scene> scene, std::shared_ptr<KernelCallback> callback);

            /**
             * GPU-enabled and multi-threaded implementation of the simulation solver
             */
            void compute_propagation();
        };
    }
}

#endif //OPENPSTD_SOLVER_H
