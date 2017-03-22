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
#include "krims/ExceptionSystem.hh"
#include <fstream>
#include <type_traits>

namespace krims {

/** Write a vector of a specified type as a binary file. */
template <typename T>
void write_binary(const std::vector<T>& data, const std::string& file) {
  static_assert(std::is_arithmetic<T>::value,
                "The function write_binary has only been tested successfully with "
                "arithmetic types");

  std::ofstream out(file, std::ios::binary);
  assert_throw(out, ExcFileNotOpen(file.c_str()));

  // Write the data in binary form:
  const char* begin = reinterpret_cast<const char*>(data.data());
  const char* end = reinterpret_cast<const char*>(&data[data.size()]);
  std::copy(begin, end, std::ostreambuf_iterator<char>(out));
  assert_throw(out, ExcIO());

  const size_t size = static_cast<size_t>(out.tellp());
  assert_throw(size == data.size() * sizeof(T), krims::ExcInternalError());
}

}  // namespace krims
