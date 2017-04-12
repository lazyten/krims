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

#include "ExceptionBase.hh"
#include <iostream>
#include <sstream>

namespace krims {

void ExceptionBase::add_exc_data(const char* file, int line, const char* function,
                                 const char* failed_condition,
                                 const char* exception_name) noexcept {
  m_name = exception_name;
  m_file = file;
  m_line = line;
  m_function = function;
  m_failed_condition = failed_condition;

  // Rebuild the string stored in m_what_str.
  rebuild_what_str();
}

/** Invoke all functions to rebuild the string returned by what() */
void ExceptionBase::rebuild_what_str() noexcept {
  try {
    std::ostringstream converter;

    // Print basic exception data:
    converter << "The assertion" << '\n'
              << "   " << m_failed_condition << '\n'
              << "failed in line " << m_line << R"( of file ")" << m_file
              << R"(" while executing the function)" << '\n'
              << "   " << m_function << '\n'
              << "This raised the exception" << '\n'
              << "   " << m_name << '\n'
              << '\n'
              << "Extra information:" << '\n'
              << extra() << '\n';

    // Set the what string:
    m_what_str = converter.str();
  } catch (...) {
    // Default string with some extra nulls at the end in case of
    // accidentially overwritten memory at the end of the string
    m_what_str = "Failed to generate the exception message.\n\0\0\0";  // NOLINT
  }
}

}  // namespace krims
