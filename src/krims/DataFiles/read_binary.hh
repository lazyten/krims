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
#include <iterator>

namespace krims {

DefException2(ExcInvalidBinaryFile, std::string, std::string,
              << "The binary file \"" << arg1 << "\" could not be read: " << arg2);

/** Read a binary file into a vector of a specified type.
 *
 * No checking on the integrity is done. The data is read and the bytes
 * converted to the requested type using a reinterpret_cast.
 *
 * The out buffer is resized automatically.
 *
 * \note Since the data is written into the std::vector<T> using
 * a reinterpret_cast from the raw bytes, you cannot perform casting of
 * any kind. A call like
 * ```
 * std::vector<double> data;
 * read_binary<long double>("bla", data)
 * ```
 * will result in undefined behaviour!
 *
 * \note Little endian byte order is assumed both in the file as well
 *       as the system. No conversion between endianness is attempted.
 **/
template <typename T>
void read_binary(const std::string& file, std::vector<T>& out) {
  static_assert(std::is_arithmetic<T>::value,
                "The function write_binary has only been tested successfully with "
                "arithmetic types");
  static_assert(std::numeric_limits<T>::is_iec559 || !std::is_floating_point<T>::value,
                "Need floating point number to be IEEE 754 compliant.");

  std::ifstream in(file, std::ios::binary);
  assert_throw(in, ExcFileNotOpen(file.c_str()));

  // Determine filesize (in bytes)
  in.seekg(0, std::ios::end);
  const size_t fsize = static_cast<size_t>(in.tellg());
  in.seekg(0, std::ios::beg);

  // Now check that we have an integer multiple of the size of the type T
  assert_throw(fsize % sizeof(T) == 0,
               ExcInvalidBinaryFile(file, "File size ( == " + std::to_string(fsize) +
                                                ") is not an integer multiple of the "
                                                "size of the requested type ( == " +
                                                std::to_string(sizeof(T)) + ")."));
  const size_t size = fsize / sizeof(T);

  // Resize output buffer and read data:
  out.resize(size);
  std::copy(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>{},
            reinterpret_cast<char*>(out.data()));
}

/** Read a binary file into a vector of specified type and check that
 *  a given number of elements has been read.
 *
 *  Effectively calls the function with two arguments defined above.
 *  See this documentation for more details.
 */
template <typename T>
void read_binary(const std::string& file, std::vector<T>& out, size_t expected_size) {
  read_binary(file, out);
  assert_throw(out.size() == expected_size,
               ExcInvalidBinaryFile(
                     file, "Number of elements read from file ( == " +
                                 std::to_string(out.size()) +
                                 ") differs from expected number of elements ( == " +
                                 std::to_string(expected_size) + ")."));
}

}  // namespace krims
