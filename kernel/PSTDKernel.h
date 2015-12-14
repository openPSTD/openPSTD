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
//      Louis van Harten
//      Omar  Richardson
//
// Purpose:
//      The PSTDKernel is the interface between the front- and backend
//      of the program.
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_KERNELFACADE_H
#define OPENPSTD_KERNELFACADE_H

#include <string>
#include "PSTDFile.h"
#include "Solver.h"
#include "core/Scene.h"

/**
 * The status of the kernel when the callback is called.
 */
enum CALLBACKSTATUS {
    CALLBACKSTATUS_ERROR,
    CALLBACKSTATUS_STARTING,
    CALLBACKSTATUS_RUNNING,
    CALLBACKSTATUS_SUCCESS,
};

class KernelCallback {
public:
    /**
     * This callback will be called with information how far the kernel is progressed.
     * Frame is only used with status == CALLBACKSTATUS_RUNNING, else it's -1.
     */
    virtual void Callback(CALLBACKSTATUS status, std::string message, int frame) = 0;

    virtual void WriteFrame(int frame, std::string domain, PSTD_FRAME data) = 0;
};

/**
 * The kernel API. Contains one method for initialization and one for running the simulation.
 */
class PSTDKernel {
private:
    std::shared_ptr<PSTDFileConfiguration> config;
    std::shared_ptr<PSTDFileSettings> settings;
    std::shared_ptr<Kernel::Scene> scene;
    const float default_alpha = 1.f;
    void initialize_scene();

    std::shared_ptr<Kernel::WaveNumberDiscretizer> wnd; // Todo: Implement

    /**
     * Read the scene description from application or file and converts the coordinates to domains.
     * Note that indexing in the file happens on grid points.
     * When interpreting domain coordinates to cells, note that this means the top_left is part of the domain,
     * and the bottom right is not.
     */
    void add_domains();

    /*
     * Computes the location of the speakers and creates new objects for them.
     * Expects real world coordinates from the scene descriptor file
     * Note that speakers are not bound to grid coordinates.
     * We find the corresponding grid by flooring the location divided by the grid size.
     */
    void add_speakers();

    /*
     * Computes the location of the receivers and creates new objects for them.
     * Expects real world coordinates from the scene descriptor file
     * @see add_speakers();
     */
    void add_receivers();


    std::vector<float> scale_to_grid(QVector2D world_vector);

    std::vector<float> scale_to_grid(QVector3D world_vector);

    std::vector<int> round_off(std::vector<float>);

    std::map<Kernel::Direction, Kernel::edge_parameters> translate_edge_parameters(Domain domain);

public:
    /**
     * Initializes the kernel and the scene, constructs the domains and sets the parameters.
     */
    PSTDKernel(std::shared_ptr<PSTDFileConfiguration> config);

    /**
     * Runs the kernel. The callback has a single function that informs the rest of the
     * application of the progress of the kernel.
     */
    void run(KernelCallback *callback);
};


#endif //OPENPSTD_KERNELFACADE_H
