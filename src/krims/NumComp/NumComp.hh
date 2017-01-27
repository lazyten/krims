//
// Copyright (C) 2016 by the krims authors
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
#include "NumEqual.hh"
#include <limits>
#include <type_traits>

namespace krims {

/** Struct to provide functions which check allow for numeric-aware comparison
 * between objects */
template <typename T>
class NumComp {
 public:
  // TODO hard-coded double type
  typedef typename std::conditional<std::is_floating_point<T>::value, T, double>::type
        error_type;

  /** Construct an NumComp object */
  explicit NumComp(const T& value)
        : m_tolerance(0),
          m_failure_action(NumCompConstants::default_failure_action),
          m_value(value) {
    tolerance(NumCompAccuracyLevel::Default);
  }

  /** Modify the tolerance by providing a different accuracy level */
  NumComp& tolerance(const NumCompAccuracyLevel accuracy);

  /** Modify the comparison tolerance */
  NumComp& tolerance(const error_type tolerance) {
    m_tolerance = tolerance;
    return *this;
  }

  /** Return the actual current tolerance level */
  error_type tolerance() const { return m_tolerance; }

  NumComp& failure_action(const NumCompActionType failure_action) {
    m_failure_action = failure_action;
    return *this;
  }

  //@{
  /** Compare with another object for equality */
  template <typename U>
  friend bool operator==(const U& lhs, const NumComp& rhs) {
    return NumEqual<U, T>{rhs.m_tolerance, rhs.m_failure_action}(lhs, rhs.m_value);
  }

  template <typename U>
  friend bool operator==(const NumComp& lhs, const U& rhs) {
    return NumEqual<T, U>{lhs.m_tolerance, lhs.m_failure_action}(lhs.m_value, rhs);
  }
  //@}

 private:
  /** \brief tolerance when comparing objects */
  error_type m_tolerance;

  //! The action to perform if a comparison fails
  NumCompActionType m_failure_action;

  /** The value  */
  const T& m_value;
};

/** Helper function to make an NumComp<T> object */
template <typename T>
NumComp<T> numcomp(const T& value) {
  return NumComp<T>(value);
}

/** Helper function to make a NumComp<T> object which is guaranteed to return */
template <typename T>
NumComp<T> numcomp_return(const T& value) {
  return NumComp<T>(value).failure_action(NumCompActionType::Return);
}

/** Helper function to make a NumComp<T> object which is guaranteed to throw
 *
 * If not ThrowVerbose is chosen as the default setting the comparison will
 * only throw normally (i.e. less verbose).
 * */
template <typename T>
NumComp<T> numcomp_throw(const T& value) {
  if (NumCompConstants::default_failure_action == NumCompActionType::ThrowVerbose) {
    return numcomp(value).failure_action(NumCompActionType::ThrowVerbose);
  } else {
    return NumComp<T>(value).failure_action(NumCompActionType::ThrowNormal);
  }
}

//
// ----------------------------------------------------------------------------
//

template <typename T>
NumComp<T>& NumComp<T>::tolerance(const NumCompAccuracyLevel accuracy) {
  error_type factor = NumCompConstants::default_tolerance_factor;

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

  m_tolerance = factor * std::numeric_limits<error_type>::epsilon();
  return *this;
}

}  // namespace krims
