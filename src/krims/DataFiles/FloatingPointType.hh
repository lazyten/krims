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

#pragma once

namespace krims {

enum FloatingPointType {
  INVALID = 0,

  /** Use IEEE 754 binary32 floats (single precision)*/
  BINARY32 = 4,

  /** Use IEEE 754 binary64 floats (double precision)*/
  BINARY64 = 8,

  /** Use IEEE 754 binary80 floats (extended precision)*/
  BINARY80 = 10,

  /** Use IEEE 754 binary128 floats (quad precision)*/
  BINARY128 = 16,
};

template <typename T>
FloatingPointType floating_point_type_of();

}  // namespace krims
