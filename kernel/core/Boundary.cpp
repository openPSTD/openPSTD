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
// along with openPSTD.  If not, see <http://www.gnu.org/licenses/\>.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Date: 17-9-15
//
//
// Authors: 0mar Richardson
//
//
// Purpose: Models a horizontal or vertical boundary between two rectangular domains.
//
//
//////////////////////////////////////////////////////////////////////////
#include "Boundary.h"

namespace OpenPSTD
{
    namespace Kernel
    {

        Boundary::Boundary(std::shared_ptr<Domain> domain1, std::shared_ptr<Domain> domain2, CalcDirection type)
        {
            this->domain1 = domain1;
            this->domain2 = domain2;
            this->type = type;
        }
    }
}