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
#include "ExceptionSystem.hh"
#include "TypeUtils.hh"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
#include <type_traits>

namespace krims {

// Forward-declare NumEqual functor.
template <typename T, typename Enable = void>
struct NumEqual;

/** What action to undertake in case a comparison fails:
 * Return false, throw an NumCompException or throw a verbose
 * NumCompException */
enum class NumCompActionType { Return, ThrowNormal, ThrowVerbose };

/** A flag to increase or decrease the tolerance based on the default
 *  value in order to change the accuracy for which the numerical
 *  comparison checks.
 *
 * MachinePrecision => std::numeric_limits<error_type>::epsilon()
 * Extreme     => tolerance_factor /  100 * MachinePrecision
 * Higher      => tolerance_factor /   10 * MachinePrecision
 * Default     => tolerance_factor        * MachinePrecision
 * Lower       => tolerance_factor *   10 * MachinePrecision
 * Sloppy      => tolerance_factor *  100 * MachinePrecision
 * SuperSloppy => tolerance_factor * 1000 * MachinePrecision
 *
 * where tolerance_factor is NumCompConstants::default_tolerance_factor
 **/
enum class NumCompAccuracyLevel {
  MachinePrecision,
  Extreme,
  Higher,
  Default,
  Lower,
  Sloppy,
  SuperSloppy,
};

/** A struct which holds static constants influencing the default behaviour of
 * NumComp */
struct NumCompConstants {
  /** The default tolerance_factor to use
   *
   * \see NumCompAccuracyLevel for more details.
   * */
  static double default_tolerance_factor;

  /** The default failure action */
  static NumCompActionType default_failure_action;
};

/** Struct to provide functions which check allow for numeric-aware comparison
 * between objects */
template <typename T>
class NumComp {
public:
  typedef typename std::conditional<std::is_floating_point<T>::value, T,
                                    double>::type error_type;

  /** Construct an NumComp object */
  explicit NumComp(const T& value)
        : m_tolerance(0),
          m_failure_action(NumCompConstants::default_failure_action),
          m_value(value) {
    tolerance(NumCompAccuracyLevel::Default);
  }

  /** Modify the tolerance by providing a different accuracy level */
  NumComp& tolerance(NumCompAccuracyLevel accuracy);

  /** Modify the comparison tolerance */
  NumComp& tolerance(error_type tolerance) {
    m_tolerance = tolerance;
    return *this;
  }

  NumComp& failure_action(NumCompActionType failure_action) {
    m_failure_action = failure_action;
    return *this;
  }

  //@{
  /** Compare with another object for equality */
  friend bool operator==(const T& lhs, const NumComp& rhs) {
    return NumEqual<T>{rhs.m_tolerance, rhs.m_failure_action}(lhs, rhs.m_value);
  }

  friend bool operator==(const NumComp& lhs, const T& rhs) {
    return rhs == lhs;
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

//

/** Exception raised by the NumComp operations if they fail on some objects. */
template <typename T>
class NumCompException : public ExceptionBase {
public:
  static_assert(std::is_floating_point<T>::value,
                "T needs to be a floating point value");

  NumCompException(const T lhs_, const T rhs_, const T error_,
                   const T tolerance_, const std::string operation_string_,
                   std::string description_ = "") noexcept
        : lhs(lhs_),
          rhs(rhs_),
          error(error_),
          tolerance(tolerance_),
          operation_string(operation_string_),
          description(description_) {}

  //! The value of the lhs
  const T lhs;

  //! The value of the rhs
  const T rhs;

  //! The error that was obtained
  const T error;

  //! The tolerance we applied
  const T tolerance;

  //! A string describing the operation (like "==" or "!=")
  const std::string operation_string;

  //! The description that was additionally supplied
  std::string description;

  //! Append some extra data to the description:
  void append(const std::string extra) {
    if (description != "") {
      description += " ";
    }
    description += extra;
  }

  /** Add enhancing exception data */
  void add_exc_data(const char* file, int line, const char* function);

  /** Print exception-specific extra information to the outstream */
  virtual void print_extra(std::ostream& out) const noexcept;

private:
  std::string failed_condition{""};
};

//

/** \brief Functor to check that two values are numerically equal --- generic
 * case */
template <typename T, typename Enable>
struct NumEqual {};  // no implementation of operator()

/** \brief Functor to check that two floating point values are numerically equal
 */
template <typename T>
struct NumEqual<
      T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
  typedef const T& first_argument_type;
  typedef const T& second_argument_type;
  typedef bool result_type;

  NumEqual(const T tolerance, const NumCompActionType failure_action)
        : m_tolerance(tolerance), m_failure_action(failure_action) {}

  bool operator()(const T& lhs, const T& rhs) const;

private:
  const T m_tolerance;
  const NumCompActionType m_failure_action;
};

/** \brief Functor to check that two complex numbers are numerically equal
 */
template <typename T>
struct NumEqual<T, typename std::enable_if<IsComplexNumber<T>::value>::type> {
  typedef const T& first_argument_type;
  typedef const T& second_argument_type;
  typedef bool result_type;
  typedef typename T::value_type real_type;

  NumEqual(const real_type tolerance, const NumCompActionType failure_action)
        : m_tolerance(tolerance), m_failure_action(failure_action) {}

  bool operator()(const T& lhs, const T& rhs) const;

private:
  const real_type m_tolerance;
  const NumCompActionType m_failure_action;
};

//
// ----------------------------------------------------------------------------
//

double NumCompConstants::default_tolerance_factor = 100.;
NumCompActionType NumCompConstants::default_failure_action =
      NumCompActionType::Return;

//

template <typename T>
NumComp<T>& NumComp<T>::tolerance(NumCompAccuracyLevel accuracy) {
  error_type factor = NumCompConstants::default_tolerance_factor;

  switch (accuracy) {
    case NumCompAccuracyLevel::MachinePrecision:
      factor = 1.;
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
  m_tolerance = factor * std::numeric_limits<error_type>::epsilon();
  return *this;
}

//

template <typename T>
void NumCompException<T>::add_exc_data(const char* file, int line,
                                       const char* function) {
  std::stringstream ss;
  ss << std::scientific << std::setprecision(15) << lhs << operation_string
     << rhs << " (tol: " << tolerance << ")";
  failed_condition = ss.str();
  ExceptionBase::add_exc_data(file, line, function, failed_condition.c_str(),
                              "NumCompException");
}

template <typename T>
void NumCompException<T>::print_extra(std::ostream& out) const noexcept {
  out << std::scientific << std::setprecision(15) << "Error in comparison ("
      << error << ") larger than tolerance (" << tolerance << ").";
  if (description != "") {
    out << std::endl << description;
  }
}

//

template <typename T>
bool NumEqual<T,
              typename std::enable_if<std::is_floating_point<T>::value>::type>::
operator()(const T& lhs, const T& rhs) const {
  using std::abs;

  // This comparison algorithm is based on these resources:
  //    http://realtimecollisiondetection.net/blog/?p=89
  //    https://stackoverflow.com/questions/17333/most-effective-way-for-float-and-double-comparison

  if (lhs == rhs) {
    // shortcut, which handles infinities
    return true;
  } else {
    // This essentially does absolute comparision if lhs and rhs are small
    // compared to the tolerance else it does relative comparsion

    const T absdiff = abs(lhs - rhs);
    const T maxone = std::max({static_cast<T>(1), abs(lhs), abs(rhs)});
    const bool equal = absdiff <= m_tolerance * maxone;

    // Alternative to control tolerance for absolute comparison (absErr)
    // and relative comparison (relErr) separately:
    // bool equal = absdiff <= max(absErr, relErr * max(abs(lhs),abs(rhs)))

    if (equal) {
      return true;
    } else if (m_failure_action == NumCompActionType::ThrowNormal ||
               m_failure_action == NumCompActionType::ThrowVerbose) {
      const T error = absdiff / maxone;
      NumCompException<T> e(lhs, rhs, error, m_tolerance, "==");
      e.add_exc_data(__FILE__, __LINE__, __PRETTY_FUNCTION__);
      throw e;
    } else {
      return false;
    }
  }
}

template <typename T>
bool NumEqual<T, typename std::enable_if<IsComplexNumber<T>::value>::type>::
operator()(const T& lhs, const T& rhs) const {
  NumEqual<real_type> is_equal{m_tolerance, m_failure_action};

  std::string part;
  try {
    // First compare real.
    // If failure action is throw and the comparison fails
    // we get to the catch part below and use the part string
    // to identify which part has failed.
    part = "Real part";
    const bool real_equal = is_equal(lhs.real, rhs.real);

    // Now compare imaginary
    // If we get through both we return the combined result.
    part = "Imaginary part";
    return real_equal && is_equal(lhs.imag, rhs.imag);
  } catch (NumCompException<real_type>& e) {
    // If we get here failure_action is some kind of Throw
    // So we rethrow what we caught.
    std::stringstream ss;

    ss << part;
    if (m_failure_action == NumCompActionType::ThrowVerbose) {
      ss << " of complex numbers " << lhs << " and " << rhs;
    }
    ss << " is not equal.";
    e.append(ss.str());
    throw;
  }
}

}  // namespace krims
