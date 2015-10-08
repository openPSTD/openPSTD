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

namespace Kernel {


    struct field_values {
        Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> u0; //TODO (Louis): change float to T, derive a double and a float class
        Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> w0;
        Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> p0;
        Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> px0;
        Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> pz0;
    };

    /**
     * A representation of one domain, as seen by the kernel.
     */
    class Domain {
    public:
        std::shared_ptr<PSTDFile> pstd_file;
        std::shared_ptr<PSTDFileSettings> settings;
        std::shared_ptr<PSTDFileConfiguration> config;
        //Todo: Note that these three are not the same/don't overlap. Which one is needed for a domain?
        int id;
        double alpha;
        double impedance;
        double rho;
        std::shared_ptr<Point> top_left;
        std::shared_ptr<Point> bottom_right;
        std::shared_ptr<Point> size;
        bool is_pml;
        field_values current_values;
        field_values previous_values;
        bool is_sec_pml;

    private:
        std::vector<std::shared_ptr<Domain>> left;
        std::vector<std::shared_ptr<Domain>> right;
        std::vector<std::shared_ptr<Domain>> top;
        std::vector<std::shared_ptr<Domain>> bottom;
        std::vector<std::shared_ptr<Domain>> pml_for;
    public:
        /**
         * Default constructor
         * @param cnf class containing configuration files //TODO
         * @param id identifier for this domain
         * @param alpha alpha of the domain, used in calculating impedance
         * @param top_left coordinates of the top left corner (x,y,(z))
         * @param size lengths of the domain edges (x,y,(z))
         * @param is_pml true if domain is pml domain
         * @param pml_for array of adjacent domains for a PML domain. nullptr if not PML domain.
         */
        Domain(std::shared_ptr<PSTDFileSettings> settings, const int id, const double alpha,
               std::shared_ptr<Point> top_left, std::shared_ptr<Point> size, const bool is_pml,
               const std::shared_ptr<Domain> pml_for);

        /**
         * Calculates the rho matrices for all edges touching another domain
         */
        void calc_rho_matrices();

        /**
         * Checks if a certain point is contained in this domain
         */
        bool contains_point(int point[]);

        /**
         * Method that returns a list of all domains touching this domain along a direction
         * @param: Specified direction enum
         * @return vector of domain pointers.
         */
        std::vector<std::shared_ptr<Domain>> get_neighbour_at(Direction direction);

        /**
         * Method that checks if this domain is touching the input domain
         * @param d Domain to check against this domain
         */
        bool is_neighbour_of(Domain d);

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
         * Calculate one timestep of propagation in this domain
         * @param bt Boundary type (calculation direction)
         * @param ct Calculation type (pressure/velocity)
         * @param dest Values to be used as factor to compute derivative in wavenumber domain
         * @see spatderp3
         */
        void calc(BoundaryType bt, CalculationType ct, std::vector<float> dest);

        /**
         * Calculate one timestep of propagation in this domain
         * @param bt Boundary type (calculation direction)
         * @param ct Calculation type (pressure/velocity)
         * @see spatderp3
         */
        void calc(BoundaryType bt, CalculationType ct);

        /**
         * Method that gives the current domain initialized with zeroes, extended by the input
         * arguments in respective directions
         * @param x extension in x direction
         * @param y extension in y direction
         * @param z extension in z direction (default: 0)
         */
        Eigen::ArrayXXf extended_zeros(int x, int y, int z = 0);

    };
}
#endif //OPENPSTD_KERNELDOMAIN_H
