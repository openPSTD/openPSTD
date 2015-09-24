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
// Purpose: Modelling boundaries between domains.
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef OPENPSTD_BOUNDARY_H
#define OPENPSTD_BOUNDARY_H
#include <string>

#include "Domain.h"
namespace Kernel {
    class Boundary {
        enum BoundaryType {
            HORIZONTAL, VERTICAL
        };
        std::shared_ptr<Domain> domain1, domain2;
        BoundaryType type;

        Boundary(std::shared_ptr<Domain> domain1, std::shared_ptr<Domain> domain2, BoundaryType type);

        std::string to_string();
    };
}

#endif //OPENPSTD_BOUNDARY_H
