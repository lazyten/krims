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

#include <iomanip>
#include <krims/DataFiles/ieee_convert.hh>
#include <krims/DataFiles/read_binary.hh>

int main(int argc, char** argv) {
  using krims::FloatingPointType;

  if (argc < 2) {
    std::cerr << "Please supply one argument:" << '\n'
              << "The path to a binary data file with numbers in IEEE BINARY128 format."
              << std::endl;
    return 1;
  }

  // Read the full content into a buffer:
  std::vector<char> buf;
  krims::read_binary(std::string(argv[1]), buf);

  if (buf.size() % 16 != 0) {
    std::cerr << "File size not a multiple of 16." << std::endl;
    return 1;
  }

  // Convert buffer to long double:
  std::vector<long double> out(buf.size() / 16);
  FloatingPointType intype = FloatingPointType::BINARY128;
  const FloatingPointType outtype = krims::floating_point_type_of<long double>();

  for (size_t i = 0; i < out.size(); ++i) {
    krims::ieee_convert({intype, &buf[i * 16]},
                        {outtype, reinterpret_cast<char*>(&out[i])});
  }

  // Print findings
  for (const auto& val : out) {
    std::cout << std::setprecision(25) << val << std::endl;
  }
  return 0;
}
