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

namespace Kernel {
    Solver::Solver(std::shared_ptr<Scene> scene) {
        this->config = scene->config;
        this->settings = scene->settings;
    }

    SingleThreadSolver::SingleThreadSolver(std::shared_ptr<Scene> scene) : Solver::Solver(scene) {
        //TODO implement solver
    }

    GPUSingleThreadSolver::GPUSingleThreadSolver(std::shared_ptr<Scene> scene) : Solver::Solver(scene) {
        //TODO implement solver
    }

    MultiThreadSolver::MultiThreadSolver(std::shared_ptr<Scene> scene) : Solver::Solver(scene) {
        //TODO implement solver
    }

    GPUMultiThreadSolver::GPUMultiThreadSolver(std::shared_ptr<Scene> scene) : Solver::Solver(scene) {
        //TODO implement solver
    }
}