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

#include <catch.hpp>
#include <cstdio>
#include <krims/FileUtils/read_binary.hh>
#include <krims/FileUtils/write_binary.hh>
#include <rapidcheck.h>

namespace krims {
using namespace rc;

static constexpr const char* temp_file = "temporary_data_please_remove_me";

template <typename T>
void test() {
  const auto size = *gen::inRange<size_t>(0, 50).as("Vector size");
  const auto vec = *gen::container<std::vector<T>>(size, rc::gen::arbitrary<T>());
  const std::string filename(temp_file);

  write_binary(vec, filename);

  // Read and check:
  std::vector<T> other;
  read_binary(filename, other);
  REQUIRE(other.size() == size);
  for (size_t i = 0; i < size; ++i) {
    REQUIRE(other[i] == vec[i]);
  }

  // Read and see it fail:
  RC_ASSERT_THROWS_AS(read_binary(filename, other, size + 1), ExcInvalidBinaryFile);

  // Read and succeed:
  read_binary(filename, other, size);
  REQUIRE(other.size() == size);
  for (size_t i = 0; i < size; ++i) {
    REQUIRE(other[i] == vec[i]);
  }
}

TEST_CASE("binary_read, binary_write tests", "[binary_read_write]") {
  SECTION("Test ints") { CHECK(rc::check("Test binary read/write ints", test<int>)); }
  SECTION("Test doubles") {
    CHECK(rc::check("Test binary read/write doubles", test<double>));
  }

  /** TODO Strings do not work and have been disabled ...
  SECTION("Test strings") {
    CHECK(rc::check("Test binary read/write strings", test<std::string>));
  }
  */

  // Remove the file again:
  std::remove(temp_file);
}  // binary_read, binary_write

}  // namespace krims
