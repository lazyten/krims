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
#include <ostream>
#include <string>

namespace krims {

/** \brief Base class for all exceptions in krims
 *
 *  Greatly inspired by the deal.ii exception system.
 */
class ExceptionBase : public std::exception {
 public:
  /** Constructor */
  ExceptionBase();

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
   * \param use_expensive Whether expensive methods to gather additional data
   * should be used (Only enable this in non-performance critical cases, e.g.
   * when the exception will actually be shown to the user in the end and the
   * program is then aborted.)
   * */
  void add_exc_data(const char* file, int line, const char* function,
                    const char* failed_condition, const char* exception_name,
                    bool use_expensive = false);

  /** The c-string which describes briefly what happened */
  const char* what() const noexcept;

  /** The name of the exception */
  const char* name() const;

  /** The result of print_extra's print call */
  const std::string extra() const;

  /** Print exception-specific extra information to the outstream */
  virtual void print_extra(std::ostream& out) const noexcept;

  /** Print a stacktrace to the outstream */
  void print_stacktrace(std::ostream& out) const;

 protected:
  /**  build the what string of this exception
   */
  virtual std::string generate_message() const noexcept;

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
  //! The class which will determine the backtrace.
  Backtrace m_backtrace;

  //! The what of the exception
  mutable std::string m_what_str;
};

}  // namespace krims
