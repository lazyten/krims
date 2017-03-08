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
#include "NumCompConstants.hh"
#include "krims/TypeUtils.hh"
#include <limits>

namespace krims {

namespace detail {
//@{
/** Determine the type which gives the principle precision of the type T.
 * This is more or less equivalent to the real type of the object. The fallback is double.
 */
template <typename T, typename = void>
struct NumericEpsilonFor {
  typedef double type;
  static constexpr type value() { return std::numeric_limits<type>::epsilon(); }
};

template <typename T>
struct NumericEpsilonFor<T, krims::enable_if_t<std::is_floating_point<T>::value>> {
  typedef T type;
  static constexpr type value() { return std::numeric_limits<type>::epsilon(); }
};

template <typename T>
struct NumericEpsilonFor<T, krims::VoidType<typename T::scalar_type>> {
  typedef typename RealTypeOf<typename T::scalar_type>::type type;
  static constexpr type value() { return std::numeric_limits<type>::epsilon(); }
};
//@}
}  // namespace detail

/** Function which determines the actual tolerance value at the given accuracy level
 *  for types T */
template <typename T = double>
double numcomp_tolerance_value(const NumCompAccuracyLevel accuracy);

//
// ----------------------------------------------------------------------------
//

template <typename T>
double numcomp_tolerance_value(const NumCompAccuracyLevel accuracy) {
  double factor = NumCompConstants::default_tolerance_factor;

  switch (accuracy) {
    case NumCompAccuracyLevel::MachinePrecision:
      factor = 1.;
      break;
    case NumCompAccuracyLevel::TenMachinePrecision:
      factor = 10.;
      break;
    case NumCompAccuracyLevel::Extreme:
      factor /= 100.;
      break;
    case NumCompAccuracyLevel::Higher:
      factor /= 10.;
      break;
    case NumCompAccuracyLevel::Default:
      break;
    case NumCompAccuracyLevel::Lower:
      factor *= 10.;
      break;
    case NumCompAccuracyLevel::Sloppy:
      factor *= 100.;
      break;
    case NumCompAccuracyLevel::SuperSloppy:
      factor *= 1000.;
      break;
  }

  // Make sure factor is at least one:
  if (factor < 1.) factor = 1.;

  return factor * detail::NumericEpsilonFor<T>::value();
}

}  // namespace krims
