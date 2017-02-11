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
#include "Backtrace.hh"
#include <exception>
#include <string>

namespace krims {

/** \brief Base class for all exceptions in krims
 *
 *  Greatly inspired by the deal.ii exception system.
 */
class ExceptionBase : public std::exception {
 public:
  /** Constructor */
  ExceptionBase()
        : m_name("?"),
          m_file("?"),
          m_line(0),
          m_function("?"),
          m_failed_condition("?"),
          m_what_str{"Failed to generate the exception message."} {}

  /** Default copy constructor */
  ExceptionBase(const ExceptionBase&) = default;

  /** Default destructor */
  virtual ~ExceptionBase() noexcept = default;

  /** \brief Add enhancing exception data
   *
   * \param file   The file where the exception occurred
   * \param function The function where the exception occurred
   * \param failed_condition The condition which failed and gave rise to the
   *                         exception
   * \param exception_name The name of the exception
   **/
  void add_exc_data(const char* file, int line, const char* function,
                    const char* failed_condition, const char* exception_name) noexcept;

  /** The c-string which describes briefly what happened */
  const char* what() const noexcept { return m_what_str.c_str(); }

  /** The name of the exception */
  const char* name() const { return m_name; }

  /** The result of print_extra's print call */
  std::string extra() const;

  /** Print exception-specific extra information to the outstream */
  virtual void print_extra(std::ostream& out) const noexcept { out << "(none)"; }

 protected:
  /** Print the internal exception data */
  virtual void print_exc_data(std::ostream& out) const noexcept;

  //! The name of the exception
  const char* m_name;

  //! The file where exception occurred
  const char* m_file;

  //! The line where exception occurred
  int m_line;

  //! The name of the function
  const char* m_function;

  //! The failed condition as a string.
  const char* m_failed_condition;

 private:
  //! The what of the exception
  std::string m_what_str;
};

}  // namespace krims
