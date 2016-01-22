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
// Purpose:
//      The openPSTD solver. Subclasses should implement the various
//      solvers present in OpenPSTD v1.1
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_SOLVER_H
#define OPENPSTD_SOLVER_H

#include "PSTDFile.h"
#include "core/Scene.h"
#include "PSTDKernel.h"


class KernelCallback; // Forward declaration
/**
 * Class for communicating with the frontend of openPSTD. Used for storing data frames and creating plots
 * as well as providing warnings and logs.
 */
namespace Kernel {

    class Solver {
    private:
        std::shared_ptr<PSTDFileSettings> settings;
        std::shared_ptr<Scene> scene;

    protected:
        KernelCallback *callback;
        /**
         * The final number of computed frames
         */
        int number_of_time_steps;

        /**
         * Updates the pressure and velocity fields of the domains to the new values computed in the RK scheme
         * @param domain: Domain under consideration
         * @param rk_step: sub-step of RK6 method
         */
        void update_field_values(std::shared_ptr<Domain> domain, unsigned long rk_step);

        /**
         * The GUI format for pressure fields
         * @return PSTD_FRAME (shared pointer to float vector)
         */
        PSTD_FRAME_PTR get_pressure_vector();

    public:
        /**
         * Solver constructor (abstract). Initialized parameters for running the openPSTD algorithm
         * @param scene: Pointer to scene object.
         * @param callback: Pointer to callback function
         * @return: New solver object.
         */
        Solver(std::shared_ptr<Scene> scene, KernelCallback *callback);

        /**
         * Start the simulation solver.
         * Runs until the simulation is finished, but meanwhile makes calls to the callback.
         * A more badass name is welcome
         */
        void compute_propagation();
    };

    class SingleThreadSolver : public Solver {
    public:
        /**
         * Default constructor. Blocking call: will not return before the solver is done.
         * @see Solver
         */
        SingleThreadSolver(std::shared_ptr<Scene> scene, KernelCallback *callback);
    };

    class MultiThreadSolver : public Solver {
    public:
        /**
         * Multithreaded solver. This instance employs multiple CPU's
         * @see Solver
         */
        MultiThreadSolver(std::shared_ptr<Scene> scene, KernelCallback *callback);
    };

    class GPUSingleThreadSolver : public Solver {
    public:
        /**
         * GPU solver. This instance runs the PSTD computations on the graphics card
         * @see Solver
         */
        GPUSingleThreadSolver(std::shared_ptr<Scene> scene, KernelCallback *callback);
    };

    class GPUMultiThreadSolver : public Solver {
    public:
        /**
         * Multithreaded GPU solver. This instance employs both multiple CPU's as well as the graphics card.
         * @see Solver
         */
        GPUMultiThreadSolver(std::shared_ptr<Scene> scene, KernelCallback *callback);
    };
}

#endif //OPENPSTD_SOLVER_H
