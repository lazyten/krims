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

#include <catch.hpp>
#include <krims/argsort.hh>
#include <rapidcheck.h>

namespace krims {
namespace tests {
using namespace rc;

TEST_CASE("argsort function", "[argsort]") {
  SECTION("Test argsort with default comparator") {
    auto test = [] {
      auto size = *gen::inRange<size_t>(0, 101).as("Size of vector to sort");
      auto array = *gen::container<std::vector<int>>(0, gen::arbitrary<int>())
                          .as("Vector to sort");

      // Sort the array the normal way.
      std::vector<int> sorted = array;
      std::sort(std::begin(sorted), std::end(sorted));

      // Argsort the arrays:
      std::vector<size_t> argsort_res =
            argsort(std::begin(array), std::end(array));
      std::vector<size_t> argsort_sorted =
            argsort(std::begin(sorted), std::end(sorted));

      // The sorted argsort should be just the indices
      for (size_t i = 0; i < argsort_sorted.size(); ++i) {
        RC_ASSERT(argsort_sorted[i] == i);
      }

      // The sorted array should be recovered using the argsort indices
      for (size_t i = 0; i < argsort_res.size(); ++i) {
        RC_ASSERT(array[argsort_res[i]] == sorted[i]);
      }

      // Check the sorting is correct
      for (size_t i = 0; i + 1 < sorted.size(); ++i) {
        RC_ASSERT(array[argsort_res[i]] < array[argsort_res[i + 1]]);
      }
    };
    REQUIRE(check("Argsort with default comparator", test));
  }  // Test argsort with default comparator

  SECTION("Test argsort with greater as comparator") {
    auto test = [] {
      auto greater = [](int i, int j) { return i > j; };

      auto size = *gen::inRange<size_t>(0, 101).as("Size of vector to sort");
      auto array = *gen::container<std::vector<int>>(0, gen::arbitrary<int>())
                          .as("Vector to sort");

      // Sort the array the normal way.
      std::vector<int> sorted = array;
      std::sort(std::begin(sorted), std::end(sorted), greater);

      // Argsort the arrays:
      std::vector<size_t> argsort_res =
            argsort(std::begin(array), std::end(array), greater);
      std::vector<size_t> argsort_sorted =
            argsort(std::begin(sorted), std::end(sorted), greater);

      // The sorted argsort should be just the indices
      for (size_t i = 0; i < argsort_sorted.size(); ++i) {
        RC_ASSERT(argsort_sorted[i] == i);
      }

      // The sorted array should be recovered using the argsort indices
      for (size_t i = 0; i < argsort_res.size(); ++i) {
        RC_ASSERT(array[argsort_res[i]] == sorted[i]);
      }

      // Check the sorting is correct
      for (size_t i = 0; i + 1 < sorted.size(); ++i) {
        RC_ASSERT(array[argsort_res[i]] > array[argsort_res[i + 1]]);
      }
    };
    REQUIRE(check("Argsort with greater as comparator", test));
  }  // Test argsort with default comparator
}
}  // tests
}  // krims
