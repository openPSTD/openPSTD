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
//      16-9-2015
//
// Authors:
//      Louis van Harten, 0mar Richardson
//
// Purpose:
//      Class containing a numerical representation of a domain as part of
//      the Scene. Stores computed values for velocity/pressure.
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_KERNELDOMAIN_H
#define OPENPSTD_KERNELDOMAIN_H

#include <Eigen/Dense>
#include <map>
#include <string>
#include <set>
#include <numeric>
#include <algorithm>
#include <iterator>
#include "kernel_functions.h"
#include "Geometry.h"
#include "../KernelInterface.h"
#include "Geometry.h"
#include "WisdomCache.h"

namespace OpenPSTD {
    namespace Kernel {

        /**
         * The observed state variables.
         * We simulate pressure and velocity, decomposed in x and y direction, as well as the combined pressure.
         * These values represent the state of the system for a fixed time.
         */


        struct windval {
            float **umat;
            float **dumat;
            float **umat_stag;
            float **dumat_stag;
        };


        struct FieldValues {
            Eigen::ArrayXXf vx0;
            Eigen::ArrayXXf vy0;
            Eigen::ArrayXXf p0;
            Eigen::ArrayXXf px0;
            Eigen::ArrayXXf py0;
            Eigen::ArrayXXf vx0_nonstag;
            Eigen::ArrayXXf vy0_nonstag;
            Eigen::ArrayXXf p0_nonstag;
            Eigen::ArrayXXf px0_nonstag;
            Eigen::ArrayXXf py0_nonstag;
            Eigen::ArrayXXf vy_ystag;
            Eigen::ArrayXXf vy_xstag;
        };

        /**
         * The spatial derivatives of the pressure and velocity in x and y direction
         */
        struct FieldLValues { // Todo (0mar): Rename, these are spatial derivatives
            Eigen::ArrayXXf Lpx;
            Eigen::ArrayXXf Lpy;
            Eigen::ArrayXXf Lvx;
            Eigen::ArrayXXf Lvy;
            Eigen::ArrayXXf Lpx_nonstag;
            Eigen::ArrayXXf Lpy_nonstag;
            Eigen::ArrayXXf Lvx_nonstag;
            Eigen::ArrayXXf Lvy_nonstag;
            Eigen::ArrayXXf Lvydx;
            Eigen::ArrayXXf Lvy_ystag;
            Eigen::ArrayXXf Lvy_xstag;
            Eigen::ArrayXXf umatarray;
            Eigen::ArrayXXf dumatarray;
            Eigen::ArrayXXf umatstagarray;
            Eigen::ArrayXXf dumatstagarray;
        };

        /**
         * The arrays used for attenuating the pressure and velocities at the boundaries of the domain.
         * These are only effective for PML domains.
         * A (2D) PML domain is able to attenuate sound in up to two directions.
         * @see apply_pml_matrices()
         */
        struct PMLArrays {
            Eigen::ArrayXXf px;
            Eigen::ArrayXXf py;
            Eigen::ArrayXXf vx;
            Eigen::ArrayXXf vy;
            Eigen::ArrayXXf px_nonstag;
            Eigen::ArrayXXf py_nonstag;
            Eigen::ArrayXXf vx_nonstag;
            Eigen::ArrayXXf vy_nonstag;
        };

        /**
         * The impedance parameter and locally reaction switch for a boundary
         */
        struct EdgeParameters {
            bool locally_reacting;
            float alpha;
        };

        /**
         * A representation of one rectangular scene unit
         *
         * This object stores the values for pressure and velocities, and references to its neighbours.
         * It supports boundaries with different impedance values as well as attenuating boundaries.
         */
        class Domain : public std::enable_shared_from_this<Domain> {
        public:
            /// Settings from the PSTDKernel
            std::shared_ptr<PSTDSettings> settings;
            /// Domain identifier. Does not necessarily correspond to the GUI and CLI ids; no need for that
            int id;
            /// Some coefficient...
            float alpha;
            // Todo: What is the alpha coefficient?
            /// Impedance of the boundary
            float impedance;
            /// Density of the domain interior. Defaults to air.
            float rho;
            /// Map with boundary coefficients
            std::map<Direction, EdgeParameters> edge_param_map;
            /// Map with update directions
            std::map<CalcDirection, bool> should_update;
            /// Start of the domain (top left)
            Point top_left;
            /// End of the domain (bottom right)
            Point bottom_right;
            /// Domain size
            Point size;
            /// Whether the domain is a perfectly matched layer
            bool is_pml;
            /// Another parameter (PML-related)
            //Todo: What is this local?
            bool local;
            /// Collection of state variables in this time step (not thread-safe)
            FieldValues current_values;
            /// Collection of state variables in previous time step (should be thread-safe)
            FieldValues previous_values;
            /// Derivative approximations of the state variables
            FieldLValues l_values;
            /// Pointer to WisdomCache object
            std::shared_ptr<WisdomCache> wnd;
            /// Whether the domain is a PML domain for other PML domains
            bool is_secondary_pml;
            /// List of domains that this domain functions for as a PML
            std::vector<std::shared_ptr<Domain>> pml_for_domain_list;

            windval wind;

        private:
            std::vector<std::shared_ptr<Domain>> left;
            std::vector<std::shared_ptr<Domain>> right;
            std::vector<std::shared_ptr<Domain>> top;
            std::vector<std::shared_ptr<Domain>> bottom;
            int num_neighbour_domains; // including pml_domains;
            int num_pml_neighbour_domains;
            bool has_horizontal_attenuation, is_corner_domain;
            std::vector<bool> needs_reversed_attenuation;
            PMLArrays pml_arrays;
        public:

            /**
             * Default constructor
             * @param settings: pointer to PSTDFile settings
             * @param id integer identifier for this domain
             * @param alpha alpha of the domain, used in calculating impedance
             * @param top_left coordinates of the top left corner (x,y,(z))
             * @param size lengths of the domain edges (x,y,(z))
             * @param is_pml true if domain is pml domain
             * @param pml_for array of adjacent domains for a PML domain. nullptr if not PML domain.
             * @return: Domain object
             */
            Domain(std::shared_ptr<PSTDSettings> settings, int id, const float alpha,
                   Point top_left, Point size, const bool is_pml,
                   std::shared_ptr<WisdomCache> wnd, std::map<Direction, EdgeParameters> edge_param_map,
                   const std::shared_ptr<Domain> pml_for_domain);

            /**
             * Constructor that accepts vectors of real word coordinates instead of points.
             * @param top_left_vector:  vector with top left corner dimensions
             * @param size: vector with size dimensions
             * @see #Domain
             */
            Domain(std::shared_ptr<PSTDSettings> settings, int id, const float alpha,
                   std::vector<float> top_left_vector, std::vector<float> size_vector, const bool is_pml,
                   std::shared_ptr<WisdomCache> wnd, std::map<Direction, EdgeParameters> edge_param_map,
                   const std::shared_ptr<Domain> pml_for_domain);

            void calcwind(std::shared_ptr<Domain> domain);

            /**
             * Overrides the old values with the new values.
             * Needs to be called before the CLI or GUI can access the data
             */
            void push_values();

            /**
             * Clears the matrices used in computing the field values
             */
            void clear_matrices();

            /**
             * Computes the matrices used in attenuating the field values in the PML domains
             */
            void compute_pml_matrices();

            /**
             * Applies the PML attenuation to the field values
             */
            void apply_pml_matrices();

            /**
             * Returns the number of neighbours
             * @param count_pml: Whether or not to also include PML domains in the count
             * @return number of neighbour domains.
             */
            int number_of_neighbours(bool count_pml);

            /**
             * Checks if a certain point is contained in this domain
             * @param point: Grid point object
             * @return: True if center of grid point is located in the domain, false otherwise
             */
            bool contains_point(Point point);

            /**
             * Checks if a certain (unrounded) location is contained in this domain.
             * @param location: Coordinates on grid.
             * @return: True if location falls within top_left and bottom_right, false otherwise.
             * @see contains_point(Point point)
             */
            bool contains_location(std::vector<float> location);

            /**
             * Returns a vector of all domains touching this domain along the specified direction
             * @param: Direction enum
             * @return vector of domain pointers.
             */
            std::vector<std::shared_ptr<Domain>> get_neighbours_at(Direction direction);

            /**
             * Returns a single neighbour domain (if existing) in the specified location.
             * If no neighbour domain is present in that location, returns `nullptr`
             * @param: Direction enum
             * @return vector of domain pointers.
             */
            std::shared_ptr<Domain> get_neighbour_at(Direction direction, std::vector<float> location);


            /**
             * Adds a neighbour to the set of neighbour domains.
             * @param domain: Pointer to fully initialized neighbour domain
             * @param direction: Direction in which the domain is neighboured.
             */
            void add_neighbour_at(std::shared_ptr<Domain> domain, Direction direction);

            /**
             * Method that checks if this domain is touching `domain`
             * @param domain: Domain to check against this domain
             */
            bool is_neighbour_of(std::shared_ptr<Domain> domain);

            /**
             * @return: true if domain functions as a PML domain for the provided domain, false otherwise.
             */
            bool is_pml_for(std::shared_ptr<Domain> domain);

            /**
             * @return: true if the domain is rigid, false otherwise
             */
            bool is_rigid();

            /**
             * Returns a vector of 1d locations of all nodes spanned by the domain in world grid
             * coordinates in direction cd. This exists to facilitate porting the legacy code.
             * @param cd Direction in which the range is requested
             */
            std::vector<int> get_range(CalcDirection cd);

            /**
             * Computes the grid points in a given direction the domain has in common with another domain.
             * @param other_domain: Domain this domain is compared to
             * @param direction: Direction along which the grid points are checked.
             * @return: vector of integers with shared grid point coordinates (in x/y/z direction)
             */
            std::vector<int> get_intersection_with(std::shared_ptr<Domain> other_domain, Direction direction);

            /**
             * Calculate one timestep of propagation in this domain
             * @param cd Boundary type (calculation direction)
             * @param ct Calculation type (pressure/velocity)
             * @param dest Values to be used as factor to compute derivative in wavenumber domain
             * @see kernel_functions.cpp
             */
            Eigen::ArrayXXf calc(CalcDirection cd, CalculationType ct, Eigen::ArrayXcf dest);

            /**
             * Calculate one time step of propagation in this domain
             * @param cd Boundary type (calculation direction)
             * @param ct Calculation type (pressure/velocity)
             * @see Kernel#spatderp3()
             */
            //void calcwind(std::shared_ptr<Domain> domain,std::shared_ptr<PSTDSettings> settings);

            void calc(CalcDirection cd, CalculationType ct);

            /**
             * Process data after all methods have been initialized.
             * Finds neighbouring domains and update information.
             */
            void post_initialization();

            /**
             * Get ranges of boundary grid points not connected to a neighbour domain along a specified direction.
             * @param direction: Domain side under consideration
             * @return: 2D array each row a range start and end variable
             */
            Eigen::ArrayXXi get_vacant_range(Direction direction);

            /**
             * Method that gives the current domain initialized with zeroes, extended by the input
             * arguments in respective directions
             * @param x extension in x direction
             * @param y extension in y direction
             * @param z extension in z direction (default: 0)
             */
            Eigen::ArrayXXf extended_zeros(int x, int y, int z = 0);

        private:
            void initialize_domain(std::shared_ptr<PSTDSettings> settings, int id, const float alpha,
                                   Point top_left, Point size, const bool is_pml,
                                   std::shared_ptr<WisdomCache> wnd,
                                   std::map<Direction, EdgeParameters> edge_param_map,
                                   const std::shared_ptr<Domain> pml_for_domain);

            void clear_fields();

            void clear_pml_arrays();

            void find_update_directions();

            void compute_number_of_neighbours();

            int get_num_pmls_in_direction(Direction direction);

            void create_attenuation_array(CalcDirection calc_dir, bool ascending, Eigen::ArrayXXf &pml_pressure,
                                          Eigen::ArrayXXf &pml_velocity);
        };

        std::ostream &operator<<(std::ostream &str, Domain const &v);
    }
}

#endif //OPENPSTD_KERNELDOMAIN_H
