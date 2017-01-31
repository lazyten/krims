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

#include "ExceptionBase.hh"
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace krims {

ExceptionBase::ExceptionBase()
      : m_name("?"),
        m_file("?"),
        m_line(0),
        m_function("?"),
        m_failed_condition("?"),
        m_backtrace{},
        m_what_str("") {}

void ExceptionBase::add_exc_data(const char* file, int line, const char* function,
                                 const char* failed_condition, const char* exception_name,
                                 bool use_expensive) {
  m_name = exception_name;
  m_file = file;
  m_line = line;
  m_function = function;
  m_failed_condition = failed_condition;

  m_backtrace.obtain_backtrace(use_expensive);
}

const char* ExceptionBase::what() const noexcept {
  // if string is empty: build it.
  if (m_what_str == "") {
    m_what_str = generate_message();
  }

  // TODO: Is this noexcept???
  return m_what_str.c_str();
}

const char* ExceptionBase::name() const { return m_name; }

const std::string ExceptionBase::extra() const {
  std::stringstream ss;
  print_extra(ss);
  return ss.str();
}

void ExceptionBase::print_extra(std::ostream& out) const noexcept { out << "(none)"; }

void ExceptionBase::print_exc_data(std::ostream& out) const noexcept {
  out << "The assertion" << std::endl
      << "   " << m_failed_condition << std::endl
      << "failed in line " << m_line << R"( of file ")" << m_file
      << R"(" while executing the function)" << std::endl
      << "   " << m_function << std::endl
      << "This raised the exception" << std::endl
      << "   " << m_name << std::endl;
}

void ExceptionBase::print_stacktrace(std::ostream& out) const {
  // If we have no backtrace, print nothing.
  if (m_backtrace.frames().size() == 0) return;

  // Determine length of longest function name:
  size_t maxfunclen = 8;  // length of string "function"
  for (const auto& frame : m_backtrace.frames()) {
    maxfunclen = std::max(maxfunclen, frame.function_name.length());
  }

  // If the Function is longer than 80 columns than just print them as is
  // (otherwise we get too much empty space)
  if (maxfunclen > 80) maxfunclen = 8;

  // Print heading of the backtrace table:
  out << std::endl;
  out << "Backtrace:" << std::endl;
  out << "----------" << std::endl;
  out << "## " << std::setw(maxfunclen) << std::left << "function" << std::right << " @ ";
  if (m_backtrace.determine_file_line()) {
    out << "    file    :  linenr" << std::endl;
  } else {
    out << " executable :  address" << std::endl;
  }
  out << "--------------------------------------" << std::endl << std::endl;

  for (size_t i = 0; i < m_backtrace.frames().size(); ++i) {
    const Backtrace::Frame& frame = m_backtrace.frames()[i];

    out << std::setw(2) << i << " " << std::setw(maxfunclen) << std::left
        << frame.function_name << std::right << " @ ";

    if (m_backtrace.determine_file_line()) {
      out << frame.codefile << "  :  " << frame.line_number;
    } else {
      out << frame.executable_name << "  :  " << frame.address;
    }
    out << std::endl;
  }

  if (!m_backtrace.determine_file_line()) {
    out << std::endl
        << R"(Hint: Use "addr2line -e <executable> <address>" to get file and line number in backtrace.)"
        << std::endl;
  }
}

std::string ExceptionBase::generate_message() const noexcept {
  // Build c_string inside a try block since this function is noexcept
  try {
    std::ostringstream converter;

    converter << std::endl
              << "--------------------------------------------------------" << std::endl;

    // Print first the general data we hold:
    print_exc_data(converter);

    // Now print the extra information:
    converter << std::endl << "Extra information:" << std::endl;
    print_extra(converter);

    // Finally print a stacktrace:
    print_stacktrace(converter);

    converter << std::endl
              << "--------------------------------------------------------" << std::endl;

    return converter.str();
  } catch (...) {
    // Deal with error by printing some generic message
    std::string message(
          "Failed to generate the exception message in "
          "ExceptionBase::generate_message()");
    return message;
  }
}

}  // krims
