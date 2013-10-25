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

namespace ramen {
	namespace utility
	{

		template <class T>
		const int calcNumDigits(T value, const bool includeMinus)
		{
			int digits = 0;

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
			double power = ceil(log(value)/log(2));
			double res = pow(2, power);

			// cast in case T type is integral
			return T(res);
		}

	} // namespace utility
} // namespace ramen

#endif // UTILITY_H