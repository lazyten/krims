//
// Copyright (C) 2016-17 by the krims authors
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
#include <krims/Algorithm/argsort.hh>
#include <rapidcheck.h>

namespace krims {
namespace tests {
using namespace rc;

namespace argsort_tests {
/** Struct which implicitly performs a range of sorting
operations upon construction which we need for testing
and comparing the results.
**/
template <typename T, typename Comparator>
struct SortedArrays {
  std::vector<T> sorted;
  std::vector<size_t> argsort_res;
  std::vector<size_t> argsort_sorted;

  SortedArrays(const std::vector<T>& unsorted)
        : sorted{}, argsort_res{}, argsort_sorted{} {
    Comparator comp{};

    // Sort the array the normal way.
    sorted = unsorted;
    std::sort(std::begin(sorted), std::end(sorted), comp);

    // Argsort the arrays:
    argsort_res    = argsort(std::begin(unsorted), std::end(unsorted), comp);
    argsort_sorted = argsort(std::begin(sorted), std::end(sorted), comp);
  }
};

/** Explicit specialisation for less, since this is the default comparator */
template <typename T>
struct SortedArrays<T, std::less<T>> {
  std::vector<T> sorted;
  std::vector<size_t> argsort_res;
  std::vector<size_t> argsort_sorted;

  SortedArrays(const std::vector<T>& unsorted)
        : sorted{}, argsort_res{}, argsort_sorted{} {
    // Sort the array the normal way.
    sorted = unsorted;
    std::sort(std::begin(sorted), std::end(sorted));

    // Argsort the arrays:
    argsort_res    = argsort(std::begin(unsorted), std::end(unsorted));
    argsort_sorted = argsort(std::begin(sorted), std::end(sorted));
  }
};

/** Functor for testing the argsort functionality */
template <typename T, typename Comparator = std::less<T>>
struct TestFunctor {
  void operator()() const {
    const auto size = *gen::inRange<size_t>(0, 101).as("Size of vector to sort");
    const auto array =
          *gen::container<std::vector<T>>(size, gen::arbitrary<T>()).as("Vector to sort");

    // Perform the sorting:
    const SortedArrays<T, Comparator> sorted(array);

    // The sorted argsort should be just the indices
    // but since the sorting might be unstable,
    for (size_t i = 0; i < size; ++i) {
      const size_t argsort_si = sorted.argsort_sorted[i];
      RC_ASSERT(sorted.sorted[argsort_si] == sorted.sorted[i]);
    }

    // The sorted array should be recovered using the argsort indices
    for (size_t i = 0; i < size; ++i) {
      const size_t argsort_i = sorted.argsort_res[i];
      RC_ASSERT(array[argsort_i] == sorted.sorted[i]);
    }

    const auto compOrEqual = [](const T& i, const T& j) {
      return i == j || Comparator{}(i, j);
    };

    // Check the sorting is correct
    for (size_t i = 0; i + 1 < size; ++i) {
      const size_t argsort_i     = sorted.argsort_res[i];
      const size_t argsort_iplus = sorted.argsort_res[i + 1];
      RC_ASSERT(compOrEqual(array[argsort_i], array[argsort_iplus]));
    }
  }
};

}  // namespace argsort_tests

TEST_CASE("argsort function", "[argsort]") {
  using argsort_tests::TestFunctor;

  SECTION("Test argsort with default comparator") {
    REQUIRE(check("Argsort with default comparator", TestFunctor<int>{}));
  }

  SECTION("Test argsort with greater as comparator") {
    REQUIRE(check("Argsort with greater as comparator",
                  TestFunctor<int, std::greater<int>>{}));
  }
}
}  // namespace tests
}  // namespace krims
