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

#include "dirname.hh"
#include <cstdlib>
#include <cstring>
#include <libgen.h>

namespace krims {

// TODO Multiplex for C++17 filesystem stuff
std::string dirname(const std::string& path) {
  char* copy = ::strndup(path.c_str(), path.size());
  std::string ret(::dirname(copy));
  free(copy);
  return ret;
}

}  // namespace krims
