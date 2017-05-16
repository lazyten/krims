//
// Copyright (C) 2016-17 by the krims authors
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
#include "NumCompException.hh"
#include "numerical_error.hh"
#include <cmath>
#include <complex>

namespace krims {

/** \brief Functor to check that two values are numerically equal --- generic
 * case (which is an empty class)*/
template <typename T, typename U, typename Enable = void>
struct NumEqual {
  static_assert(!std::is_same<Enable, void>::value,
                "NumEqual has not been specialised for these types.");
  // no implementation of operator()
};

/** \brief Functor to check that two floating point values are numerically equal
 */
template <typename T, typename U>
struct NumEqual<T, U, typename std::enable_if<std::is_floating_point<T>::value &&
                                              std::is_floating_point<U>::value>::type> {
  typedef const T& first_argument_type;
  typedef const U& second_argument_type;
  typedef bool result_type;

  typedef typename std::common_type<T, U>::type common_type;

  NumEqual(const common_type tolerance, const NumCompActionType failure_action)
        : m_tolerance(tolerance), m_failure_action(failure_action) {}

  bool operator()(const T& lhs, const U& rhs) const;

 private:
  const common_type m_tolerance;
  const NumCompActionType m_failure_action;
};

/** \brief Functor to check that two complex numbers are numerically equal
 */
template <typename T, typename U>
struct NumEqual<std::complex<T>, std::complex<U>,
                typename std::enable_if<std::is_floating_point<T>::value &&
                                        std::is_floating_point<U>::value>::type> {
  typedef const std::complex<T>& first_argument_type;
  typedef const std::complex<U>& second_argument_type;
  typedef bool result_type;

  typedef typename std::common_type<T, U>::type common_real_type;

  NumEqual(const common_real_type tolerance, const NumCompActionType failure_action)
        : m_tolerance(tolerance), m_failure_action(failure_action) {}

  bool operator()(const std::complex<T>& lhs, const std::complex<U>& rhs) const;

 private:
  const common_real_type m_tolerance;
  const NumCompActionType m_failure_action;
};

//

//

template <typename T, typename U>
bool NumEqual<T, U, typename std::enable_if<std::is_floating_point<T>::value &&
                                            std::is_floating_point<U>::value>::type>::
operator()(const T& lhs, const U& rhs) const {
  const common_type error = numerical_error<common_type>(lhs, rhs);

  if (error <= m_tolerance) {
    return true;
  } else if (m_failure_action == NumCompActionType::ThrowNormal ||
             m_failure_action == NumCompActionType::ThrowVerbose) {
    NumCompException<common_type> e(lhs, rhs, error, m_tolerance, "==");
    e.add_exc_data(__FILE__, __LINE__, __PRETTY_FUNCTION__);
    throw e;
  } else {
    return false;
  }
}

template <typename T, typename U>
bool NumEqual<std::complex<T>, std::complex<U>,
              typename std::enable_if<std::is_floating_point<T>::value &&
                                      std::is_floating_point<U>::value>::type>::
operator()(const std::complex<T>& lhs, const std::complex<U>& rhs) const {
  NumEqual<T, U> is_equal{m_tolerance, m_failure_action};

  std::string part;
  try {
    // First compare real.
    // If failure action is throw and the comparison fails
    // we get to the catch part below and use the part string
    // to identify which part has failed.
    part = "Real part";
    const bool real_equal = is_equal(lhs.real(), rhs.real());

    // Now compare imaginary
    // If we get through both we return the combined result.
    part = "Imaginary part";
    return real_equal && is_equal(lhs.imag(), rhs.imag());
  } catch (NumCompExceptionBase& e) {
    // If we get here failure_action is some kind of Throw
    // So we rethrow what we caught.
    std::stringstream ss;

    ss << ' ' << part;
    if (m_failure_action == NumCompActionType::ThrowVerbose) {
      ss << " of complex numbers " << lhs << " and " << rhs;
    }
    ss << " is not equal.";
    e.append_extra(ss.str());
    throw;
  }
}

}  // namespace krims
