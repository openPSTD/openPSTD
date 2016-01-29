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
#include "wave_numbers.h"

namespace OpenPSTD
{
    namespace Kernel
    {


        struct FieldValues
        {
            Eigen::ArrayXXf u0; //TODO (do we want to do this?): change float to T, derive a float and a double class
            Eigen::ArrayXXf w0;
            Eigen::ArrayXXf p0;
            Eigen::ArrayXXf px0;
            Eigen::ArrayXXf py0;
        };

        struct FieldLValues
        { // Todo (0mar): Rename, these are spatial derivatives
            Eigen::ArrayXXf Lpx;
            Eigen::ArrayXXf Lpy;
            Eigen::ArrayXXf Lvx;
            Eigen::ArrayXXf Lvy;

        };

        struct PMLArrays
        { // Todo: Change to unique pointers.
            Eigen::ArrayXXf px;
            Eigen::ArrayXXf py;
            Eigen::ArrayXXf u;
            Eigen::ArrayXXf w;
        };

        struct EdgeParameters
        {
            bool locally_reacting;
            float alpha; //Better name
        };

        /**
         * A representation of one domain, as seen by the kernel.
         */
        class Domain : public std::enable_shared_from_this<Domain>
        {
        public:
            std::shared_ptr<PSTDSettings> settings;
            int id;
            float alpha;
            float impedance;
            float rho;
            std::map<Direction, EdgeParameters> edge_param_map;
            std::map<int, RhoArray> rho_arrays;
            std::map<CalcDirection, bool> should_update;
            Point top_left;
            Point bottom_right;
            Point size;
            bool is_pml;
            bool local;
            FieldValues current_values, previous_values;
            FieldLValues l_values;
            std::shared_ptr<WisdomCache> wnd;
            bool is_secondary_pml;
            std::vector<std::shared_ptr<Domain>> pml_for_domain_list;

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
             *
             *
             */

            //Domain() { };
            /**
             * Default constructor
             * @param settings: PSTD settings pointer
             * @param id identifier for this domain
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
             * @see Domain(***)
             */
            Domain(std::shared_ptr<PSTDSettings> settings, int id, const float alpha,
                   std::vector<float> top_left_vector, std::vector<float> size_vector, const bool is_pml,
                   std::shared_ptr<WisdomCache> wnd, std::map<Direction, EdgeParameters> edge_param_map,
                   const std::shared_ptr<Domain> pml_for_domain);

            /**
             * Calculates the rho matrices for all edges touching another domain
             * "rho matrices" is the term used in the python code. In this implementation
             * they are referred to as "rho arrays", consistent with their use.
             */
            void compute_rho_arrays();

            /**
             * Overrides the old values with the new values.
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
             * @return number of neighbours.
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
             */
            bool contains_location(std::vector<float> location);

            /**
             * Method that returns a list of all domains touching this domain along a direction
             * @param: Specified direction enum
             * @return vector of domain pointers.
             */
            std::vector<std::shared_ptr<Domain>> get_neighbours_at(Direction direction);

            /**
             * Method that returns a single domain of all domains touching this domain along a direction
             * @param: Specified direction enum
             * @return vector of domain pointers.
             */
            std::shared_ptr<Domain> get_neighbour_at(Direction direction, std::vector<float> location);


            /**
             * Add a a neighbour to the set of neighbour domains.
             * @param domain: Pointer to fully initialized neighbour domain
             * @param direction: Direction in which the domain is neighboured.
             */
            void add_neighbour_at(std::shared_ptr<Domain> domain, Direction direction);

            /**
             * Method that checks if this domain is touching the input domain
             * @param d Domain to check against this domain
             */
            bool is_neighbour_of(std::shared_ptr<Domain> domain);

            /**
             * True if domain functions as a PML domain for the provided domain, false otherwise.
             */
            bool is_pml_for(std::shared_ptr<Domain> domain);

            /**
             * Returns true if the domain is rigid
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
             * @see spatderp3
             */
            Eigen::ArrayXXf calc(CalcDirection cd, CalculationType ct, Eigen::ArrayXcf dest);

            /**
             * Calculate one timestep of propagation in this domain
             * @param cd Boundary type (calculation direction)
             * @param ct Calculation type (pressure/velocity)
             * @see spatderp3
             */
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

            int get_rho_array_key(std::shared_ptr<Domain> domain1, std::shared_ptr<Domain> domain2);

            int get_num_pmls_in_direction(Direction direction);

            void create_attenuation_array(CalcDirection calc_dir, bool ascending, Eigen::ArrayXXf &pml_pressure,
                                          Eigen::ArrayXXf &pml_velocity);
        };

        std::ostream &operator<<(std::ostream &str, Domain const &v);
    }
}

#endif //OPENPSTD_KERNELDOMAIN_H
