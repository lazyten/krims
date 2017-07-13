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
#include <cstring>

namespace krims {

// TODO Multiplex for C++17 filesystem stuff
std::string realpath(const std::string& path) {
  if (path.empty()) {
    return "";
  }

  char* rp = ::realpath(path.c_str(), nullptr);
  if (rp == nullptr) {
    const int errval    = errno;
    const size_t buflen = 1024;
    char buffer[buflen] = {0};

#ifdef _GNU_SOURCE
    // The GNU compliant version returns the message as a
    // char pointer into the provided buffer
    char* msg = strerror_r(errval, buffer, buflen);
    assert_throw(false, ExcRealpathError(errval, std::string(msg)));
#else
    // The XSI compliant version only returns an int
    // and stores the message in the buffer
    int ret = strerror_r(errval, buffer, buflen);

    // If ret is unequal to 0 we had an error in the strerror_r call.
    assert_throw(ret == 0, ExcRealpathError(errval, "Unknown error"));

    // Throw the actual error message
    buffer[buflen - 1] = '\0';
    assert_throw(false, ExcRealpathError(errval, std::string(&buffer[0])));
#endif  // _GNU_SOURCE
  }

  std::string ret = std::string(rp);
  free(rp);
  return ret;
}

}  // namespace krims
