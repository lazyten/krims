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
          m_extra("(none)"),
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

  /** The c-string which describes briefly what happened.
   *
   * This string includes information about what assertion has failed,
   * the name of the exception, where the excetpion has failed and
   * the exception-specific extra information */
  const char* what() const noexcept { return m_what_str.c_str(); }

  /** The name of the exception */
  const char* name() const { return m_name; }

  /** The name of the file where the exception occurred */
  const char* file() const { return m_file; }

  /** The line where the exception occurred */
  int line() const { return m_line; }

  /** The function which failed */
  const char* function() const { return m_function; }

  /** The condition which failed */
  const char* failed_condition() const { return m_failed_condition; }

  /** The exception-specific extra information */
  std::string extra() const { return m_extra; }

  /** Alter the exception-specific extra information */
  void set_extra(std::string extra) {
    m_extra = std::move(extra);
    rebuild_what_str();
  }

  /** Append another string to the extra string stored inside. */
  void append_extra(const std::string& s) {
    m_extra.append(s);
    rebuild_what_str();
  }

  void prepend_extra(std::string s) {
    m_extra = s.append(m_extra);
    rebuild_what_str();
  }

 protected:
  /** Invoke all functions to rebuild the string returned by what() */
  void rebuild_what_str() noexcept;

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

  //! The extra information, specific to the exception.
  std::string m_extra;

 private:
  //! The what of the exception
  std::string m_what_str;
};

}  // namespace krims
