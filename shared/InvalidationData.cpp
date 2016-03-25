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
//
//
// Authors:
//
//
//////////////////////////////////////////////////////////////////////////

//
// Created by michiel on 8-9-2015.
//

#include "InvalidationData.h"

namespace OpenPSTD
{
    namespace Shared
    {
        OPENPSTD_SHARED_EXPORT bool InvalidationData::IsChanged()
        {
            if (changed)
                return true;

            auto i = std::begin(items);

            while (i != std::end(items))
            {
                if (auto observe = i->lock())
                {
                    if (observe->changed)
                        return true;
                    i++;
                }
                else
                {
                    items.erase(i);
                }

            }
            return false;

        }

        OPENPSTD_SHARED_EXPORT void InvalidationData::Change()
        {
            changed = true;
        }

        OPENPSTD_SHARED_EXPORT void InvalidationData::Reset()
        {
            changed = false;

            auto i = std::begin(items);

            while (i != std::end(items))
            {
                if (auto observe = i->lock())
                {
                    observe->Reset();
                    i++;
                }
                else
                {
                    items.erase(i);
                }
            }
        }

        OPENPSTD_SHARED_EXPORT void InvalidationData::Register(std::weak_ptr<InvalidationData> item)
        {
            this->items.push_back(item);
        }
    }
}