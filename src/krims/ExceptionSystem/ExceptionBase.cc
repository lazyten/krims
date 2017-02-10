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

  try {
    std::ostringstream converter;

    print_exc_data(converter);
    converter << '\n' << "Extra information:" << '\n';
    print_extra(converter);

    m_what_str = converter.str();
  } catch (...) {
    // By default m_what_str contains the message
    //"Failed to generate the exception message."
  }
}

std::string ExceptionBase::extra() const {
  std::stringstream ss;
  print_extra(ss);
  return ss.str();
}

void ExceptionBase::print_exc_data(std::ostream& out) const noexcept {
  out << "The assertion" << '\n'
      << "   " << m_failed_condition << '\n'
      << "failed in line " << m_line << R"( of file ")" << m_file
      << R"(" while executing the function)" << '\n'
      << "   " << m_function << '\n'
      << "This raised the exception" << '\n'
      << "   " << m_name << '\n';
}

}  // namespace krims
