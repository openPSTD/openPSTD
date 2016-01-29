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
// Date: 29-1-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose: General types that can be used throughout the application.
//
//
//////////////////////////////////////////////////////////////////////////


#ifndef OPENPSTD_GENERALTYPES_H
#define OPENPSTD_GENERALTYPES_H

#include <memory>
#include <vector>

using PSTD_FRAME_UNIT = float;
using PSTD_FRAME = std::vector<PSTD_FRAME_UNIT>;
using PSTD_FRAME_PTR = std::shared_ptr<PSTD_FRAME>;

#endif //OPENPSTD_GENERALTYPES_H
