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
// Date: 19-4-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_LOCKING_PTR_H
#define OPENPSTD_LOCKING_PTR_H

#include <memory>
#include <boost/thread.hpp>
namespace OpenPSTD
{
    namespace Shared
    {
        template<class T>
        class locking_ptr
        {
        public:
            locking_ptr(std::shared_ptr<T> ptr, std::shared_ptr<boost::recursive_mutex> mutex)
                    : m_ptr(ptr),
                      m_mutex(mutex),
                      m_lock(*m_mutex)
            {
            }

            std::shared_ptr<T> operator ->()
            {
                return m_ptr;
            }
            std::shared_ptr<T> const operator ->() const
            {
                return m_ptr;
            }
        private:
            std::shared_ptr<T> m_ptr;
            std::shared_ptr<boost::recursive_mutex> m_mutex; // whatever implementation you use
            boost::unique_lock<boost::recursive_mutex> m_lock;
        };
    }
}


#endif //OPENPSTD_LOCKING_PTR_H
