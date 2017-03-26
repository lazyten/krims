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
#include "krims/ExceptionSystem.hh"
#include <iomanip>

namespace krims {

/** Base exception of all NumCompExceptions. Catch these to get all NumComp
 *  Exceptions type-indepentantly */
class NumCompExceptionBase : public ExceptionBase {
 public:
  NumCompExceptionBase(std::string description_) noexcept : description(description_) {}

  //! The description that was additionally supplied
  std::string description;

  //! Append some extra data to the description:
  void append(const std::string& extra);
};

/** Exception raised by the NumComp operations if they fail on some objects. */
template <typename T>
class NumCompException : public NumCompExceptionBase {
 public:
  static_assert(std::is_arithmetic<T>::value, "T needs to be an arithmetic value");
  // otherwise the declaration of T as error and tolerance makes no sense.

  NumCompException(const T lhs_, const T rhs_, const T error_, const T tolerance_,
                   const std::string operation_string_,
                   const std::string description_ = "") noexcept
        : NumCompExceptionBase{description_},
          lhs(lhs_),
          rhs(rhs_),
          error(error_),
          tolerance(tolerance_),
          operation_string(operation_string_) {}

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

  /** Add enhancing exception data */
  void add_exc_data(const char* file, int line, const char* function);

  /** Print exception-specific extra information to the outstream */
  virtual void print_extra(std::ostream& out) const noexcept;

 private:
  std::string failed_condition{""};
};

//
// --------------------------------------------------------------
//

template <typename T>
void NumCompException<T>::add_exc_data(const char* file, int line, const char* function) {
  std::stringstream ss;
  ss << std::scientific << std::setprecision(15) << lhs << operation_string << rhs
     << " (tol: " << tolerance << ")";
  failed_condition = ss.str();
  ExceptionBase::add_exc_data(file, line, function, failed_condition.c_str(),
                              "NumCompException");
}

template <typename T>
void NumCompException<T>::print_extra(std::ostream& out) const noexcept {
  out << std::scientific << std::setprecision(15) << "Error in comparison (" << error
      << ") larger than tolerance (" << tolerance << ").";
  if (NumCompExceptionBase::description != "") {
    out << '\n' << '\n' << NumCompExceptionBase::description;
  }
}

}  // namespace krims
