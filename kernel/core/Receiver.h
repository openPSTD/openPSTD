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
// Authors: 0mar
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef OPENPSTD_RECEIVER_H
#define OPENPSTD_RECEIVER_H
#include <vector>
#include <iostream>
#include <memory>
#include "PSTDFile.h"
#include "Geometry.h"
#include <string>

namespace Kernel {
    class Receiver {
    public:
        const double x;
        const double y;
        const double z;
        std::string id;
        enum InterpolationType {
            NearestNeighbour, SpectralInterpolation
        };
        std::vector<double> location;
        std::shared_ptr<PSTDFile> config;
        Point grid_location; //Todo (0mar): Should this be rounded down or rather rounded off?
        std::vector<double> grid_offset;
        std::shared_ptr<Domain> container_domain;
        std::vector<double> received_values;

        Receiver(std::vector<double> location, std::shared_ptr<PSTDFile> config, std::string id,
                 std::shared_ptr<Domain> container);

        double compute_received_value();

        std::string to_string();
    };

}
#endif //OPENPSTD_RECEIVER_H
