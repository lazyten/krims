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

#include "demangle.hh"
#include "krims/config.hh"
#include <cstdlib>
#include <cstring>
#ifdef KRIMS_HAVE_LIBSTDCXX_DEMANGLER
#include <cxxabi.h>
#endif

namespace krims {

char* demangle(const char* mangled_name) {
#ifdef KRIMS_HAVE_LIBSTDCXX_DEMANGLER
  // try to demangle the function name:
  int status;
  char* p = abi::__cxa_demangle(mangled_name, nullptr, nullptr, &status);

  if (status == 0) return p;
  return nullptr;
#else
  char* ret = static_cast<char*>(malloc(strlen(mangled_name)));
  if (ret == NULL) return NULL;
  strcpy(ret, mangled_name);
  return ret;
#endif
}

std::string demangled_string(const char* mangled) {
  char* demangled = demangle(mangled);

  if (demangled != nullptr) {
    std::string ret(demangled);
    free(demangled);
    return ret;
  }

  return std::string(mangled);
}

}  // namespace krims
