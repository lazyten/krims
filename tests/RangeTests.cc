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
#include <krims/Range.hh>
#include <rapidcheck.h>
#include <type_traits>

namespace krims {
namespace tests {
using namespace rc;

namespace range_tests {
template <typename T>
struct RangeTests {
  typedef Range<T> range_type;
  typedef typename range_type::size_type size_type;

  static void construction(T t1, T t2) {
    bool is_valid = t1 <= t2;
    RC_CLASSIFY(is_valid, "Range is valid");

    if (is_valid) {
      range_type r{t1, t2};
      RC_ASSERT(r.length() == static_cast<size_type>(static_cast<ptrdiff_t>(t2) - t1));
      RC_ASSERT(r.lower_bound() == t1);
      RC_ASSERT(r.upper_bound() == t2);
    } else {
      range_type r{t1, t2};
      RC_ASSERT(r.empty());
    }
  }

  static void element_access() {
    const auto length = *gen::positive<T>();

    RC_CLASSIFY(length == 0, "Zero range");

    // Start value
    const auto start = *gen::arbitrary<T>().as("Start index");

    // Exclude overflow cases:
    RC_PRE(start + length > length);

    // Construct the range:
    range_type r = range(start, start + length);

    // Access arbitrary element:
    const auto acc = *gen::inRange<T>(0, r.length());

    if (r.empty()) {
#ifdef DEBUG
      typedef typename range_type::ExcEmptyRange exc_type;
      RC_ASSERT_THROWS_AS(r[acc], exc_type);
      RC_ASSERT_THROWS_AS(r.front(), exc_type);
      RC_ASSERT_THROWS_AS(r.back(), exc_type);
#endif
    } else {
      RC_ASSERT(r[acc] == start + acc);
      RC_ASSERT(r.front() == start);
      RC_ASSERT(r.back() == start + length - 1);
    }
  }

  static void access_to_empty_range(T value) {
    // Generate an empty range
    range_type r{{value, value}};

    // it should be empty
    RC_ASSERT(r.empty());

    // length should be zero
    RC_ASSERT(r.length() == size_type{0});
    RC_ASSERT(r.size() == size_type{0});

    // begin and and should be identical.
    RC_ASSERT(r.begin() == r.end());

#ifdef DEBUG
    // check that element access throws:
    typedef typename range_type::ExcEmptyRange exc_type;
    RC_ASSERT_THROWS_AS(r[0], exc_type);
#endif
  }

  static void iteration() {
    const auto length = *gen::inRange<T>(0, 101);

    RC_CLASSIFY(length == 0, "Zero range");

    // Start value
    const auto start = *gen::arbitrary<T>().as("Start index");

    // Exclude overflow cases:
    RC_PRE(start + length > length);

    // Construct the range:
    range_type r = range(start, start + length);

    // Iterator variable:
    auto i = start;
    typename range_type::size_type counter{0};

    // Use range-based iterator:
    for (const auto val : r) {
      RC_ASSERT(i == val);
      ++i;
      ++counter;
    }

    RC_ASSERT(counter == r.length());
  }

  static void access_to_past_the_end_iterator() {
#ifdef DEBUG
    krims::RangeIterator<size_t> it;

    CHECK_THROWS_AS(*it, krims::ExcInvalidState);
    CHECK_THROWS_AS(++it, krims::ExcInvalidState);
    CHECK_THROWS_AS(it++, krims::ExcInvalidState);
#endif
  }
};

}  // namespace range_tests

TEST_CASE("Range tests", "[range]") {
  using namespace range_tests;

  SECTION("Range construction") {
    REQUIRE(rc::check("Range construction (size_t)", RangeTests<size_t>::construction));
    REQUIRE(rc::check("Range construction (int)", RangeTests<int>::construction));
  }

  SECTION("Element access") {
    REQUIRE(rc::check("Element access (size_t)", RangeTests<size_t>::element_access));
    REQUIRE(rc::check("Element access (int)", RangeTests<int>::element_access));
  }

  SECTION("Empty range properties") {
    REQUIRE(rc::check("Empty range properties (size_t)",
                      RangeTests<size_t>::access_to_empty_range));
    REQUIRE(rc::check("Empty range properties (int)",
                      RangeTests<int>::access_to_empty_range));
  }

  SECTION("Range equality") {
    auto r1 = krims::range(3, 5);
    krims::Range<size_t> r2(3, 5);
    auto r3 = krims::range(3, 4);
    auto r4 = range(4, 5);
    krims::Range<int> r5(4, 5);

    CHECK(r1 == r1);
    CHECK(r1 == r2);
    CHECK(r1 != r3);
    CHECK(r1 != r4);
    CHECK(r1 != r5);

    CHECK(r2 == r1);
    CHECK(r2 == r2);
    CHECK(r2 != r3);
    CHECK(r2 != r4);
    CHECK(r2 != r5);

    CHECK(r3 != r1);
    CHECK(r3 != r2);
    CHECK(r3 == r3);
    CHECK(r3 != r4);
    CHECK(r3 != r5);

    CHECK(r4 != r1);
    CHECK(r4 != r2);
    CHECK(r4 != r3);
    CHECK(r4 == r4);
    CHECK(r4 == r5);

    CHECK(r5 != r1);
    CHECK(r5 != r2);
    CHECK(r5 != r3);
    CHECK(r5 == r4);
    CHECK(r5 == r5);
  }

  SECTION("Iteraton") {
    REQUIRE(rc::check("Iteration (size_t)", RangeTests<size_t>::iteration));
    REQUIRE(rc::check("Iteration (int)", RangeTests<int>::iteration));
  }

  SECTION("Past-the-end-iterator properties (size_t)") {
    RangeTests<size_t>::access_to_past_the_end_iterator();
  }

  SECTION("Past-the-end-iterator properties (int)") {
    RangeTests<int>::access_to_past_the_end_iterator();
  }

  SECTION("Tests for +=, -=, - and + on Ranges") {
    CHECK((3 - Range<int>(2, 4)) == Range<int>(0, 2));
    CHECK((3 - Range<int>(1, 6)) == Range<int>(-2, 3));
    CHECK((0 - Range<int>(1, 3)) == Range<int>(-2, 0));
    CHECK((3 - Range<int>(1, 1)) == Range<int>(2, 2));

    auto testable = [](long start) {
      unsigned int off = *gen::arbitrary<unsigned int>().as("Range length");

      Range<long> range{{start, start + off}};

      long add = *gen::arbitrary<int>().as("Offset to add");
      range += add;
      RC_ASSERT(range.lower_bound() == start + add);
      RC_ASSERT(range.upper_bound() == start + off + add);

      range -= add;
      RC_ASSERT(range.lower_bound() == start);
      RC_ASSERT(range.upper_bound() == start + off);

      RC_ASSERT((range + add) - add == range);
      RC_ASSERT((add + range) - add == range);
      RC_ASSERT((range - add) + add == range);
      RC_ASSERT(add - (add - range) == range);

      auto res = add - range;

      if (off != 0) {
        RC_ASSERT(res.lower_bound() == add - start - off + 1);
        RC_ASSERT(res.upper_bound() == add - start + 1);
      } else {
        RC_ASSERT(res.lower_bound() == add - start);
        RC_ASSERT(res.upper_bound() == add - start);
      }
    };

    CHECK(rc::check("Tests for shifting ranges around", testable));
  }  // Tests for +=, -=, - and + on Ranges

  SECTION("Tests for intersection, subset and superset") {
    Range<size_t> empty{{4, 4}};
    Range<size_t> empty2{{5, 5}};

    CHECK(empty.subset_of(empty2));
    CHECK(empty.superset_of(empty2));
    CHECK(intersection(empty, empty2).empty());
    CHECK(empty.contains(empty));
    CHECK(empty2.contains(empty));

    Range<size_t> r1{{2, 6}};
    CHECK(empty.subset_of(r1));
    CHECK(r1.contains(empty));
    CHECK(r1.contains(r1));
    CHECK(intersection(r1, empty).empty());

    Range<size_t> r2{{1, 4}};
    CHECK_FALSE(r1.contains(r2));
    CHECK_FALSE(r2.contains(r1));

    auto inter = intersection(r1, r2);
    Range<size_t> inter_ref{{2, 4}};
    CHECK(inter == inter_ref);
    CHECK(inter.subset_of(r1));
    CHECK(inter.subset_of(r2));
    CHECK(r1.contains(inter));
    CHECK(r2.contains(inter));

    Range<size_t> r3{{5, 6}};
    CHECK(intersection(r2, r3).empty());
    CHECK(r1.contains(r3));
    CHECK(intersection(r1, r3) == r3);
    CHECK_FALSE(intersection(r1, r3).empty());
    CHECK(r3.subset_of(r1));
    CHECK(r3.subset_of(r3));
    CHECK(r1.superset_of(r1));
    CHECK(r1.superset_of(r3));
    CHECK_FALSE(r3.superset_of(r1));
  }

}  // TEST_CASE
}  // namespace tests
}  // namespace krims
