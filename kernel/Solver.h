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
//      Louis van Harten
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

namespace Kernel {

    class Solver {
    private:
        std::shared_ptr<PSTDFileConfiguration> config;

        void initDomains();
        void addSpeakers();
        void addReceivers();
        void initRhoMatrices();
    public:
        /**
         * Default constructor
         * @param scene The scene file for which the solver should be configured.
         */
        Solver(std::shared_ptr<Scene> scene);
        /**
         * Blocking call: will not return until the solver is done.
         */
        void runSingleThread();
        /**
         * Blocking call: will not return until the solver is done.
         */
        void runGPUSingleThread();
        /**
         * Blocking call: will not return until the solver is done.
         */
        void runMultiThread();
        /**
         * Blocking call: will not return until the solver is done.
         */
        void runGPUMultiThread();
    };

}

#endif //OPENPSTD_SOLVER_H
