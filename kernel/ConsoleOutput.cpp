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
//      12-7-2015
//
// Authors:
//      Michiel Fortuijn
//
//////////////////////////////////////////////////////////////////////////

#include "ConsoleOutput.h"
#include <iostream>

void ConsoleOutput::Callback(CALLBACKSTATUS status, std::string message, int frame)
{
    if (status == CALLBACKSTATUS::ERROR)
    {
        std::cerr << "Error encountered while running OpenPSTD:" << std::endl;
        std::cerr << message << std::endl;
    }
    else if (status == CALLBACKSTATUS::STARTING)
    {
        std::cout << message << std::endl;
    }
    else if (status == CALLBACKSTATUS::RUNNING)
    {
        std::cout << message << std::endl;
    }
    else if (status == CALLBACKSTATUS::FINISHED)
    {
        std::cout << message << std::endl;
    }
}

void ConsoleOutput::WriteFrame(int frame, int domain, PSTD_FRAME_PTR data) {

}
