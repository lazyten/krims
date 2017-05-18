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
#include <krims/DataFiles/read_binary.hh>
#include <krims/DataFiles/write_binary.hh>
#include <krims/FileSystem/dirname.hh>
#include <krims/FileSystem/path_exists.hh>
#include <random>
#include <rapidcheck.h>

namespace krims {
namespace tests {
using namespace rc;

// The random number generator to generate random (hence non-existent) filenames.
std::default_random_engine engine;
std::uniform_int_distribution<size_t> dist(0, 100000);

template <typename T>
void test() {
  const auto size = *gen::inRange<size_t>(0, 50).as("Vector size");
  const auto vec = *gen::container<std::vector<T>>(size, rc::gen::arbitrary<T>())
                          .as("Data to read/write");
  const std::string filename = std::string("temp.").append(std::to_string(dist(engine)));

  write_binary(vec, filename);

  // Read and check:
  std::vector<T> other;
  read_binary(filename, other);
  RC_ASSERT(other.size() == size);
  for (size_t i = 0; i < size; ++i) {
    RC_ASSERT(other[i] == vec[i]);
  }

  // Read and see it fail:
  RC_ASSERT_THROWS_AS(read_binary(filename, other, size + 1), ExcInvalidBinaryFile);

  // Read and succeed:
  read_binary(filename, other, size);
  RC_ASSERT(other.size() == size);
  for (size_t i = 0; i < size; ++i) {
    RC_ASSERT(other[i] == vec[i]);
  }

  // Remove the file again:
  std::remove(filename.c_str());
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

  SECTION("Test read_binary128") {
    const std::string f128sample = dirname(__FILE__) + "/float128sample.bin";
    REQUIRE(path_exists(f128sample));

    std::vector<long double> reference{0.0L,
                                       0.0L / -1.0L,
                                       -2.0L,
                                       4.0L,
                                       15.0L,
                                       1.0L / 10.0L,
                                       1.0L / 10101010.0L,
                                       -std::numeric_limits<long double>::infinity(),
                                       std::numeric_limits<long double>::infinity(),
                                       1e123L,
                                       -34873289471.0L};

    // Load without size constraint
    std::vector<long double> data;
    read_binary128(f128sample, data);
    CHECK(data.size() == reference.size());

    // Load with size constraint:
    read_binary128(f128sample, data, reference.size());

    for (size_t i = 0; i < reference.size(); ++i) {
      if (std::isfinite(reference[i]) && reference[i] != 0) {
        CHECK(std::abs(reference[i] - data[i]) < 1e-19);
      } else {
        CHECK(reference[i] == data[i]);
      }
    }
  }  // read_binary128

}  // binary_read, binary_write

}  // namespace tests
}  // namespace krims
