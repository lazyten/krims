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
#include "krims/version_defs.hh"
#include <string>

namespace krims {

struct version {
  // Note: CMake automatically fills this version information
  // via the header version_defs.hh and the WriteVersionHeader module

  static int constexpr major{detail::__version_var_major};
  static int constexpr minor{detail::__version_var_minor};
  static int constexpr patch{detail::__version_var_patch};

  // Return the version as a string
  static std::string version_string();
};

}  // namespace krims
