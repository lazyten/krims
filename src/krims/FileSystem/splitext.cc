//
// Copyright (C) 2017 by the krims authors
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

#include "splitext.hh"
#include "krims/ExceptionSystem.hh"

namespace krims {

// TODO Multiplex for C++17 filesystem stuff
std::pair<std::string, std::string> splitext(const std::string& path) {
  const size_t slash  = path.rfind('/');
  const size_t dot    = path.rfind('.');
  const bool have_dot = dot != std::string::npos;

  // Is this a deeper path, i.e. one containing a / or
  // just a plain file name (i.e. no /)
  const bool have_slash = slash != std::string::npos;

  // The file is a hidden file with only one dot in the filename (at the beginning)
  // IF
  // either the whole path is a single file name and the only dot starts it
  // OR
  // if the path contains earlier components, i.e. '/' chars, and
  // the only dot of the file name is directly after the slash.
  const bool hidden_file = (!have_slash && dot == 0) || (have_slash && dot == slash + 1);

  // There is no '.' in the filename
  // IF
  // either there is exactly no dot present in the path
  // OR
  // if it is before the last slash (i.e. in the earlier path components)
  const bool no_dot_in_filename = !have_dot || (have_slash && dot <= slash);

  // There is no dot to do a splitting or this is
  // a hidden file, where we do no splitting
  if (no_dot_in_filename || hidden_file) {
    return {path, ""};
  } else {
    assert_internal(dot != std::string::npos);
    return {path.substr(0, dot), path.substr(dot)};
  }
}

}  // namespace krims
