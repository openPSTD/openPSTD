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
// Date: 17-9-15
//
//
// Authors: omar
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_KERNELSCENE_H
#define OPENPSTD_KERNELSCENE_H

#include <eigen/Eigen/Dense>
#include <map>
#include <string>
#include "kernel_functions.h"
#include "KernelDomain.h"
#include "Geometry.h"
#include "Speaker.h"
#include "Receiver.h"
#include "Boundary.h"
#include "PSTDFile.h"

class KernelScene {
public:
    PSTDFile *cfg;
    std::vector <KernelDomain> domain_list;
    std::vector <Boundary> boundary_list;
    std::vector <Receiver> receiver_list;
    std::vector <Speaker> source_list;
    Point *top_left, *bottom_right, *size; //Todo (0mar): Still necessary?

    /**
     * Constructor of Scene object
     * @param cnf class containing configuration files
     */
    KernelScene(PSTDFile *cnf);

    /**
     * Adds a receiver to the scene.
     * A receiver logs the sound perceived in that location each time step.
     * The coordinates have to correspond to the (integer) grid points from the scene descriptor file,
     * but need not be integer.
     * @param x coordinate on grid in x dimension
     * @param y coordinate on grid in y dimension
     * @param x coordinate on grid in z dimension
     */
    void add_receiver(const double x,const double y,const double z);

    /**
     * Adds a speaker to the scene.
     * A speaker emits the sound that is modelled by the PSTD method.
     * The coordinates have to correspond to the (integer) grid points from the scene descriptor file,
     * but need not be integer.
     * @param x coordinate on grid in x dimension
     * @param y coordinate on grid in y dimension
     * @param x coordinate on grid in z dimension
     */
    void add_speaker(const double x,const double y,const double z);

    /**
     * Add domain to the scene. Checks for every other domain
     * whether they share a boundary or whether more PML domains have to be added(?).
     * @param domain: unprocessed domain object.
     */

    void add_domain(KernelDomain domain);
    void compute_rho_matrices();
    void compute_pml_matrices();
    void add_pml_domains();
    void apply_pml_matrices();
    //~KernelScene();

};


#endif //OPENPSTD_KERNELSCENE_H
