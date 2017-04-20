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

#include "realpath.hh"

// Use the gnu version of strerror_r:
#define _GNU_SOURCE 1
#include <cstring>

namespace krims {

// TODO Multiplex for C++17 filesystem stuff
std::string realpath(const std::string& path) {
  if (path.empty()) {
    return "";
  }

  char* rp = ::realpath(path.c_str(), nullptr);
  if (rp == nullptr) {
    const int errval = errno;
    char buffer[1024] = {0};
    char* msg = strerror_r(errval, buffer, 1024);
    assert_throw(false, ExcRealpathError(errval, std::string(msg)));
  }
  std::string ret = std::string(rp);
  free(rp);
  return ret;
}

}  // namespace krims
