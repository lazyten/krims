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
#include "krims/Algorithm/join.hh"
#include "krims/TypeUtils/EnableIfLibrary.hh"
#include "krims/TypeUtils/IsStreamInsertable.hh"
#include "krims/TypeUtils/RealTypeOf.hh"
#include "numerical_error.hh"
#include <cmath>
#include <complex>
#include <vector>

namespace krims {

namespace detail {
template <typename Container>
void showContainerValues(
      const Container& c,
      enable_if_t<!IsStreamInsertable<Container>::value, std::ostream>& o) {
  o << join(std::begin(c), std::end(c), " ");
}

template <typename Container>
void showContainerValues(
      const Container& c,
      enable_if_t<IsStreamInsertable<Container>::value, std::ostream>& o) {
  o << c;
}
}  // namespace detail

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

/** Base struct, which holds some common methods for comparing containers. */
template <typename Cont1, typename Cont2>
struct NumEqualContainerBase {
  static_assert(std::is_same<typename Cont1::size_type, typename Cont2::size_type>::value,
                "The size types have to match.");
  static_assert(
        std::is_same<typename Cont1::value_type, typename Cont2::value_type>::value,
        "The value types have to match.");

  typedef typename Cont1::value_type value_type;
  typedef typename Cont1::size_type size_type;
  typedef typename RealTypeOf<value_type>::type real_type;

  // Check whether the sizes match only
  bool sizes_match(size_type lhs_value, size_type rhs_value,
                   const std::string& what) const;

  // Compare the number of elements
  bool number_elem_match(const Cont1& lhs, const Cont2& rhs,
                         const std::string& object_name) const;

  bool element_values_match(const Cont1& lhs, const Cont2& rhs,
                            const std::string& object_name) const;

  const real_type tolerance;
  const NumCompActionType failure_action;

  NumEqualContainerBase(const real_type tolerance_,
                        const NumCompActionType failure_action_)
        : tolerance(tolerance_), failure_action(failure_action_) {}
};

/** \brief Functor to check that two vectors of floating point values are identical */
template <typename T, typename U>
struct NumEqual<std::vector<T>, std::vector<U>,
                typename std::enable_if<std::is_floating_point<T>::value &&
                                        std::is_floating_point<U>::value>::type>
      : private NumEqualContainerBase<std::vector<T>, std::vector<U>> {
 private:
  using base_type = NumEqualContainerBase<std::vector<T>, std::vector<U>>;

 public:
  typedef const std::vector<T>& first_argument_type;
  typedef const std::vector<U>& second_argument_type;
  typedef bool result_type;

  typedef typename std::common_type<T, U>::type common_type;

  NumEqual(const common_type tolerance, const NumCompActionType failure_action)
        : base_type{tolerance, failure_action} {};

  bool operator()(const std::vector<T>& lhs, const std::vector<U>& rhs) const {
    return base_type::number_elem_match(lhs, rhs, "vectors") &&
           base_type::element_values_match(lhs, rhs, "vectors");
  }
};

//
// --------------------------------------------------------
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

  std::string part = "Real part";
  try {
    // First compare real.
    // If failure action is throw and the comparison fails
    // we get to the catch part below and use the part string
    // to identify which part has failed.
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

template <typename Cont1, typename Cont2>
bool NumEqualContainerBase<Cont1, Cont2>::sizes_match(size_type lhs_value,
                                                      size_type rhs_value,
                                                      const std::string& what) const {
  if (lhs_value == rhs_value) {
    return true;
  } else if (failure_action == NumCompActionType::ThrowNormal ||
             failure_action == NumCompActionType::ThrowVerbose) {

    const size_type diff =
          lhs_value < rhs_value ? rhs_value - lhs_value : lhs_value - rhs_value;
    NumCompException<size_type> e(lhs_value, rhs_value, diff, 0, "==",
                                  "Size mismatch in number of " + what);
    e.add_exc_data(__FILE__, __LINE__, __PRETTY_FUNCTION__);
    throw e;
  } else {
    return false;
  }
}

template <typename Cont1, typename Cont2>
bool NumEqualContainerBase<Cont1, Cont2>::number_elem_match(
      const Cont1& lhs, const Cont2& rhs, const std::string& object_name) const {
  // Compare the sizes. If we get an exception just pass it upwards
  // appending the values in case the user wants detailed throw
  // information.
  try {
    return sizes_match(lhs.size(), rhs.size(), "elements");
  } catch (NumCompExceptionBase& e) {
    if (failure_action == NumCompActionType::ThrowVerbose) {
      std::stringstream ss;
      ss << " when comparing " << object_name << '\n';
      detail::showContainerValues(lhs, ss);
      ss << '\n' << "and" << '\n';
      detail::showContainerValues(rhs, ss);
      ss << '\n';
      e.append_extra(ss.str());
    }
    throw;
  }
}

template <typename Cont1, typename Cont2>
bool NumEqualContainerBase<Cont1, Cont2>::element_values_match(
      const Cont1& lhs, const Cont2& rhs, const std::string& object_name) const {
  // If one is not equal return false or catch the exception and amend
  // the data we are interested in before rethrowing.
  NumEqual<value_type, value_type> is_equal{tolerance, failure_action};
  for (size_type i = 0; i < lhs.size(); ++i) {
    try {
      if (!is_equal(lhs[i], rhs[i])) return false;
    } catch (NumCompExceptionBase& e) {
      std::stringstream ss;

      ss << " Entry (" << i << ") not equal";
      if (failure_action == NumCompActionType::ThrowVerbose) {
        ss << " when comparing " << object_name << '\n';
        ss << " when comparing " << object_name << '\n';
        detail::showContainerValues(lhs, ss);
        ss << '\n' << "and" << '\n';
        detail::showContainerValues(rhs, ss);
        ss << '\n';
      } else {
        ss << ".";
      }
      e.append_extra(ss.str());
      throw;
    }
  }
  return true;
}

}  // namespace krims
