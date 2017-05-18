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

#include "read_binary.hh"
#include <krims/DataFiles/ieee_convert.hh>

namespace krims {

void read_binary128(const std::string& file, std::vector<long double>& out) {
  const FloatingPointType intype = FloatingPointType::BINARY128;

  // Read the full content into a buffer:
  std::vector<char> buf;
  krims::read_binary(file, buf);

  assert_throw(
        buf.size() % 16 == 0,
        ExcInvalidBinaryFile(
              file, "The size of the file ( == " + std::to_string(buf.size()) +
                          ") is not divisible by 16, which is however required for files "
                          "with only IEEE quadruple precision 128bit floats."));

  // Convert buffer to long double:
  const FloatingPointType outtype = krims::floating_point_type_of<long double>();
  out.resize(buf.size() / 16);

  for (size_t i = 0; i < out.size(); ++i) {
    krims::ieee_convert({intype, &buf[i * 16]},
                        {outtype, reinterpret_cast<char*>(&out[i])});
  }
}

void read_binary128(const std::string& file, std::vector<long double>& out,
                    size_t expected_size) {
  const FloatingPointType intype = FloatingPointType::BINARY128;

  // Read the full content into a buffer:
  std::vector<char> buf;
  krims::read_binary(file, buf, expected_size * 16);

  // Convert buffer to long double:
  const FloatingPointType outtype = krims::floating_point_type_of<long double>();
  out.resize(expected_size);

  for (size_t i = 0; i < out.size(); ++i) {
    krims::ieee_convert({intype, &buf[i * 16]},
                        {outtype, reinterpret_cast<char*>(&out[i])});
  }
}

}  // namespace krims
