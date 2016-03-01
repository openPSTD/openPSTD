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
// Authors: Omar Richardson
//
//
// Purpose: Model a scene (a collections of rectangular domains).
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_KERNELSCENE_H
#define OPENPSTD_KERNELSCENE_H

#include <Eigen/Dense>
#include <map>
#include <string>
#include <sstream>
#include "kernel_functions.h"
#include "Domain.h"
#include "Geometry.h"
#include "Speaker.h"
#include "Receiver.h"
#include "Boundary.h"
#include "../KernelInterface.h"

namespace OpenPSTD {
    namespace Kernel {
        class Scene {
        public:
            std::vector<std::shared_ptr<Domain>> domain_list;
            std::shared_ptr<PSTDSettings> settings;
            Point top_left;
            Point bottom_right;
            Point size;

            std::vector<std::shared_ptr<Boundary>> boundary_list;
            std::vector<std::shared_ptr<Receiver>> receiver_list;
            std::vector<std::shared_ptr<Speaker>> speaker_list;
        private:
            std::map<Direction, EdgeParameters> default_edge_parameters; // Uninitialized
            int number_of_domains;
        public:

            /**
             * Constructor of Scene object
             * @param config: pointer to configuration settings
             */
            Scene(std::shared_ptr<PSTDSettings> settings);

            /**
             * Adds a receiver to the scene.
             * A receiver logs the sound perceived in that location each time step.
             * The coordinates have to correspond to the (integer) grid points from the scene descriptor file,
             * but need not be integer.
             * @param x coordinate on grid in x dimension
             * @param y coordinate on grid in y dimension
             * @param x coordinate on grid in z dimension
             */
            void add_receiver(const float x, const float y, const float z);

            /**
             * Adds a speaker to the scene.
             * A speaker emits the sound that is modelled by the PSTD method.
             * The coordinates have to correspond to the (integer) grid points from the scene descriptor file,
             * but need not be integer.
             * @param x coordinate on grid in x dimension
             * @param y coordinate on grid in y dimension
             * @param x coordinate on grid in z dimension
             */
            void add_speaker(const float x, const float y, const float z);

            /**
             * Add domain to the scene. Checks for every other domain
             * whether they share a boundary and processes pml domains correctly
             * @param domain: pointer to domain object to be added.
             */
            void add_domain(std::shared_ptr<Domain> domain);

            /**
             * Computes the perfectly matched layer matrix coefficients for each domain in the scene.
             */
            void compute_pml_matrices();

            /**
             * Add the necessary perfectly matched layer domain to the current scene,
             * checks which layers belong to which domains, and checks which layers can be merged.
             */
            void add_pml_domains();

            /**
             * Applies the layer coefficients for each domain in the scene.
             */
            void apply_pml_matrices();

            /**
             * Obtains the global pressure field by summing the pressure in each domain.
             * @return: Array with pressure values
             */
            Eigen::ArrayXXf get_pressure_field();

            /**
             * Fetch a domain with specified ID, if existing.
             * @param id: integer ID of domain.
             * @return: domain pointer if domain with id exists, else nullptr.
             */
            std::shared_ptr<Domain> get_domain(int id);

            /**
            * Returns a new domain ID integer
            */
            int get_new_id();

        private:

            /**
             * General field obtainer function, extensible if necessary
             */
            Eigen::ArrayXXf get_field(char field_type);

            /**
             * Helper function for add_pml_domains.
             * Collects the topleft and bottom right points of a domain.
             */
            std::vector<int> get_corner_points(std::shared_ptr<Domain> domain);

            /**
             * Helper function for add_pml_domains
             * Checks if two pml domains can (and should) be merged.
             */

            bool should_merge_domains(std::shared_ptr<Domain> domain1, std::shared_ptr<Domain> domain2);

            /**
             * Helper function for add_pml_domains
             * Finds the common parent of two pml domains.
             */
            std::shared_ptr<Domain> get_singular_parent_domain(std::shared_ptr<Domain> domain);
        };

        std::ostream &operator<<(std::ostream &str, Scene const &v);
    }
}

#endif //OPENPSTD_KERNELSCENE_H
