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

#include <eigen/Eigen/Dense>
#include <map>
#include <string>
#include <numeric>
#include "kernel_functions.h"
#include "Geometry.h"
#include "PSTDFile.h"
#include "Geometry.h"
#include "wave_numbers.h"

namespace Kernel {


    struct field_values {
        Eigen::ArrayXXf u0; //TODO (Louis): change float to T, derive a float and a float class
        Eigen::ArrayXXf w0;
        Eigen::ArrayXXf p0;
        Eigen::ArrayXXf px0;
        Eigen::ArrayXXf pz0;
    };

    /**
     * A representation of one domain, as seen by the kernel.
     */
    class Domain : std::enable_shared_from_this<Domain>{
    public:
        std::shared_ptr<PSTDFileSettings> settings;
        int id;
        float alpha;
        float impedance;
        float rho;
        std::map<std::string, RhoArray> rho_arrays;
        std::shared_ptr<Point> top_left;
        std::shared_ptr<Point> bottom_right;
        std::shared_ptr<Point> size;
        bool is_pml;
        field_values current_values;
        field_values previous_values;
        std::shared_ptr<WaveNumberDiscretizer> wnd;
        bool is_sec_pml;
        std::vector<std::shared_ptr<Domain>> pml_for_domain_list;

    private:
        std::vector<std::shared_ptr<Domain>> left;
        std::vector<std::shared_ptr<Domain>> right;
        std::vector<std::shared_ptr<Domain>> top;
        std::vector<std::shared_ptr<Domain>> bottom;
    public:
        /**
         * Default constructor
         * @param settings: PSTD settings pointer
         * @param id identifier for this domain
         * @param alpha alpha of the domain, used in calculating impedance
         * @param top_left coordinates of the top left corner (x,y,(z))
         * @param size lengths of the domain edges (x,y,(z))
         * @param is_pml true if domain is pml domain
         * @param pml_for array of adjacent domains for a PML domain. nullptr if not PML domain.
         */
        Domain(std::shared_ptr<PSTDFileSettings> settings, const int id, const float alpha,
               std::shared_ptr<Point> top_left, std::shared_ptr<Point> size, const bool is_pml,
               std::shared_ptr<WaveNumberDiscretizer> wnd,
               const std::shared_ptr<Domain> pml_for_domain);

        /**
         * Calculates the rho matrices for all edges touching another domain
         * "rho matrices" is the term used in the python code. In some parts of the C++ code,
         * they are referred to as "rho arrays". TODO (optional) rename this for internal consistency
         */
        void calc_rho_matrices();

        void push_values();

        void clear_matrices();

        void calc_pml_matrices();

        bool should_update(std::shared_ptr<Domain> domain);

        bool is_rigid();

        int number_of_neighbours(bool count_pml);

        /**
         * Checks if a certain point is contained in this domain
         */
        bool contains_point(Point point);

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


        void add_neighbour_at(std::shared_ptr<Domain> domain, Direction direction);
        /**
         * Method that checks if this domain is touching the input domain
         * @param d Domain to check against this domain
         */
        bool is_neighbour_of(std::shared_ptr<Domain> domain);

        bool is_pml_for(std::shared_ptr<Domain> domain);
        /**
         * Returns true if the domain is rigid
         */
        bool is_rigid();

        /**
         * Returns a vector of 1d locations of all nodes spanned by the domain in world grid
         * coordinates in direction bt. This exists to facilitate porting the legacy code.
         * @param bt Direction in which the range is requested
         */
        std::vector<int> get_range(BoundaryType bt);

        /**
         * Computes the grid points in a given direction the domain has in common with another domain.
         * @param other_domain: Domain this domain is compared to
         * @param direction: Direction along which the grid points are checked.
         * @return: vector of integers with shared grid point coordinates (in x/y/z direction)
         */
        std::vector<int> get_intersection_with(std::shared_ptr<Domain> other_domain, Direction direction);

        /**
         * Calculate one timestep of propagation in this domain
         * @param bt Boundary type (calculation direction)
         * @param ct Calculation type (pressure/velocity)
         * @param dest Values to be used as factor to compute derivative in wavenumber domain
         * @see spatderp3
         */
        Eigen::ArrayXXf calc(BoundaryType bt, CalculationType ct, std::shared_ptr<Eigen::ArrayXcf> dest);

        /**
         * Calculate one timestep of propagation in this domain
         * @param bt Boundary type (calculation direction)
         * @param ct Calculation type (pressure/velocity)
         * @see spatderp3
         */
        void calc(BoundaryType bt, CalculationType ct);


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
        std::shared_ptr<Eigen::ArrayXXf> extended_zeros(int x, int y, int z = 0);

    };
}
#endif //OPENPSTD_KERNELDOMAIN_H
