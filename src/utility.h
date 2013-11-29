//  Copyright (C) 2013  kittikun
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#ifndef UTILITY_H
#define UTILITY_H

#include <cmath>
#include <string>
#include <boost/format.hpp>
#include <boost/type_traits.hpp>

namespace ramen {
    namespace utility
    {
        template <class T>
        const int calcNumDigits(T value, const bool includeMinus)
        {
            BOOST_STATIC_ASSERT(boost::is_arithmetic<T>::value);
            uint32_t digits = 0;

            // do we need to cound minus sign
            if (includeMinus && (value < 0)) {
                digits = 1;
            }

            while (value) {
                value /= 10;
                digits++;
            }

            return digits;
        }

        template <class T>
        const T calcNearestPowerofTwo(const T value)
        {
            BOOST_STATIC_ASSERT(boost::is_arithmetic<T>::value);
            const double power = ceil(::log(static_cast<double>(value)) / ::log(2));

            // cast in case T type is integral
            return T(pow(2, power));
        }

        template <class T>
        const std::string readableSizeByte(const T size)
        {
            BOOST_STATIC_ASSERT(boost::is_integral<T>::value);
            const char * symbols = "KMGTPE";

            if (size < 1024) {
                return boost::str(boost::format("%1%B") % size);
            }

            const double dSize = static_cast<double>(size);
            const int exp = static_cast<int>(::log(dSize) / ::log(1024));
            boost::format fmt;

            if (exp > 1)
                fmt = boost::format("%1$1.2f%2%B");
            else
                fmt = boost::format("%1$1.0f%2%B");

            return boost::str(fmt % (dSize / pow(1024, exp)) % symbols[static_cast<int>(exp) - 1]);
        }
    } // namespace utility
} // namespace ramen

#endif // UTILITY_H