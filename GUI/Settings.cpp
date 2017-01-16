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
// Date: 11-4-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#include "Settings.h"
#include <boost/predef.h>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <cstdlib>

namespace fs = boost::filesystem;
using namespace OpenPSTD::GUI;

std::string GetConfigDir();
std::string GetFilename();

#if BOOST_OS_WINDOWS
    std::string configDir = "OpenPSTD";
    std::string filename = "config.xml";

    std::string GetConfigDir()
    {
        return std::string(getenv("APPDATA")) + "\\" + configDir;
    }

    std::string GetFilename()
    {
        return GetConfigDir() + "\\" + filename;
    }

#elif BOOST_OS_LINUX || BOOST_OS_CYGWIN
    std::string configDir = "~/.OpenPSTD";
    std::string filename = "~/.OpenPSTD/config.xml";

    std::string GetConfigDir()
    {
        return configDir;
    }

    std::string GetFilename()
    {
        return filename;
    }

#elif BOOST_OS_MACOS
    //todo check where to save config files on MAC OS x
    #error todo check where to save config files on MAC OS x
#else
    #error Compiling for a not supported platform or Windows, Linux or MAC OS x is not detected by boost predefs
#endif

std::shared_ptr<Settings> Settings::Load()
{
    std::string temp = GetConfigDir();
    if(!boost::filesystem::exists(temp) || !boost::filesystem::exists(GetFilename()))
    {
        return std::make_shared<Settings>();
    }

    //read all settings
    std::shared_ptr<Settings> settings = std::make_shared<Settings>();
    std::ifstream ifs(GetFilename());
    boost::archive::xml_iarchive ia(ifs);
    ia >> BOOST_SERIALIZATION_NVP(*settings);

    return settings;
}

void Settings::Save()
{
    if(!boost::filesystem::exists(GetConfigDir()))
    {
        fs::create_directories(GetConfigDir());
    }

    if(boost::filesystem::exists(GetFilename()))
    {
        fs::remove(GetFilename());
    }

    Settings* settings = this;

    std::ofstream ofs(GetFilename());
    boost::archive::xml_oarchive oa(ofs);
    oa << BOOST_SERIALIZATION_NVP(settings);
}









