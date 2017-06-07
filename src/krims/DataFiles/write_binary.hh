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
#include "krims/ExceptionSystem.hh"
#include <fstream>
#include <type_traits>

namespace krims {

/** TODO   Assume all written binary data is little endian
 *         and convert to big endian if the system requires this.
 *
 *         See also TestBigEndian.cmake script of cmake
 *         and the setup_dependencies inside the cmake folder.
 */

/** Write a vector of a specified type as a binary file.
 *
 * \note Little endian byte order is assumed both in the file as well
 *       as the system. No conversion between endianness is attempted.
 **/
template <typename T>
void write_binary(const std::vector<T>& data, const std::string& file) {
  static_assert(std::is_arithmetic<T>::value,
                "The function write_binary has only been tested successfully with "
                "arithmetic types");
  static_assert(std::numeric_limits<T>::is_iec559 || !std::is_floating_point<T>::value,
                "Need floating point number to be IEEE 754 compliant.");

  std::ofstream out(file, std::ios::binary);
  assert_throw(out, ExcFileNotOpen(file.c_str()));

  // Write the data in binary form:
  const char* begin = reinterpret_cast<const char*>(data.data());
  const char* end   = reinterpret_cast<const char*>(&data[data.size()]);
  std::copy(begin, end, std::ostreambuf_iterator<char>(out));
  assert_throw(out, ExcIO());

  const size_t size = static_cast<size_t>(out.tellp());
  assert_throw(size == data.size() * sizeof(T), ExcIO());
}

}  // namespace krims
