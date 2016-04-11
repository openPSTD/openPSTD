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

#ifndef OPENPSTD_SETTINGS_H
#define OPENPSTD_SETTINGS_H

#include <string>
#include <memory>
#include <shared/InvalidationData.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace OpenPSTD
{
    namespace GUI
    {
        class SnappingSettings
        {
        private:
            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & BOOST_SERIALIZATION_NVP(SnapToGrid);
                ar & BOOST_SERIALIZATION_NVP(SnapToDomainEdges);
                ar & BOOST_SERIALIZATION_NVP(SnapToDomainEdgesDistance);
            }

        public:
            bool SnapToGrid = true;
            bool SnapToDomainEdges = true;
            float SnapToDomainEdgesDistance = 0.4f;
        };

        class VisualSettings
        {
        private:
            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & BOOST_SERIALIZATION_NVP(EdgeSize);
            }

        public:
            float EdgeSize = 5.0f;
        };


        class Settings : public OpenPSTD::Shared::InvalidationData
        {
        private:
            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive & ar, const unsigned int version)
            {
                ar & BOOST_SERIALIZATION_NVP(snapping);
                ar & BOOST_SERIALIZATION_NVP(visual);
                ar & BOOST_SERIALIZATION_NVP(GPUAcceleration);
                ar & BOOST_SERIALIZATION_NVP(CPUAcceleration);
                ar & BOOST_SERIALIZATION_NVP(UseMockKernel);
            }

        public:
            SnappingSettings snapping;
            VisualSettings visual;

            bool GPUAcceleration = false;
            bool CPUAcceleration = false;
            bool UseMockKernel = false;

            Settings();
            static std::shared_ptr<Settings> Load();
            void Save();
        };
    }
}

#endif //OPENPSTD_SETTINGS_H
