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
#include "FloatingPointType.hh"
#include <utility>

namespace krims {

/** Low level conversion function to convert between different
 *  IEEE 754 floating point formats in memory.
 *
 *  No checks on memory bounds are done. Enough space to
 *  store the input and output data types has to be present
 *  at the pointed locations.
 *
 *  The function tries to use the functionality implemented in the
 *  compiler as much as possible for doing the conversions.
 *
 *  \note Right now only BINARY128 to BINARY80 is implemented.
 */
void ieee_convert(std::pair<FloatingPointType, const char*> in,
                  std::pair<FloatingPointType, char*> out);

}  // namespace krims
