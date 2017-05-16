//
// Copyright (C) 2017 by the krims authors
//
// This file is part of krims.
//
// krims is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// krims is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with krims. If not, see <http://www.gnu.org/licenses/>.
//

#pragma once
#include "krims/TypeUtils/RealTypeOf.hh"
#include <limits>

namespace krims {
/** Determine a balanced mixture of absolute and relative error
 *  of the numbers, depending on their size.
 *
 * Based on these resources:
 *    http://realtimecollisiondetection.net/blog/?p=89
 *    https://stackoverflow.com/questions/17333/most-effective-way-for-float-and-double-comparison
 */
template <typename T, typename RealType = krims::RealTypeOfType<T>>
RealType abs_or_rel_error(const T& lhs, const T& rhs) {
  using std::abs;

  // shortcut, which handles infinities
  if (lhs == rhs) return 0;

  // Handle NaNs:
  if (lhs != lhs || rhs != rhs) return std::numeric_limits<T>::max();

  // This essentially does absolute comparision if lhs and rhs are small
  // compared to the tolerance else it does relative comparsion

  const RealType absdiff = abs(lhs - rhs);
  const RealType maxside = std::max<RealType>(abs(lhs), abs(rhs));
  const RealType maxone = std::max<RealType>(1, maxside);
  return absdiff / maxone;

  // Alternative to control tolerance for absolute comparison (absErr)
  // and relative comparison (relErr) separately:
  // return absdiff <= max(absErr, relErr * max(abs(lhs),abs(rhs)))
}

// TODO Add implementation based on
//      http://bitbashing.io/comparing-floats.html

/** Determine a suitable distance measure between the two numbers.
 *
 *  This value should represent roughly the numerical error between
 *  the two values at hand.
 */
template <typename T, typename RealType = krims::RealTypeOfType<T>>
RealType numerical_error(const T& lhs, const T& rhs) {
  return abs_or_rel_error(lhs, rhs);
}

}  // namespace krims
