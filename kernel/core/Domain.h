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
#include "kernel_functions.h"
#include "PSTDFile.h"
#include "Geometry.h"

namespace Kernel {

    struct domain_values {
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> u0; //TODO (Louis): change float to T, derive a double and a float class
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> w0;
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> p0;
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> px0;
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> pz0;
    };

    /**
     * A representation of one domain, as seen by the kernel.
     */
    class Domain {
    public:
        std::shared_ptr<PSTDFile> cfg;
        int id;
        double alpha;
        double impedance;
        double rho;
        std::shared_ptr<Point> top_left;
        std::shared_ptr<Point> bottom_right;
        std::shared_ptr<Point> size;
        bool is_pml;
        domain_values current_dvals;
        domain_values prev_dvals;

        std::vector<std::shared_ptr<Domain>> left;
        std::vector<std::shared_ptr<Domain>> right;
        std::vector<std::shared_ptr<Domain>> top;
        std::vector<std::shared_ptr<Domain>> bottom;
        std::vector<std::shared_ptr<Domain>> pml_for;
        bool is_sec_pml;

    private:
    public:
        /**
         * Default constructor
         * @param cnf class containing configuration files //TODO
         * @param id identifier for this domain
         * @param alpha alpha of the domain, used in calculating impedance
         * @param topleft coordinates of the top left corner (x,y,(z))
         * @param size lengths of the domain edges (x,y,(z))
         * @param is_pml true if domain is pml domain
         * @param pml_for array of adjacent domains for a PML domain. nullptr if not PML domain.
         */
        Domain(const Config cnf, const int id, const double alpha,
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
         * Method that returns a list of all domains touching this domain
         */
        std::vector<Domain> neighbour_list();

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

    };
}
#endif //OPENPSTD_KERNELDOMAIN_H
