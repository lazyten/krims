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
#include <krims/ExceptionSystem.hh>
#include <rapidcheck.h>

namespace krims {
namespace tests {
using namespace rc;

template <typename T, typename... Args>
void throw_exception(Args... args) {
  assert_throw(false, T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
void throw_exception_dbg(Args... args) {
  assert_dbg(false, T(std::forward<Args>(args)...));
}

TEST_CASE("Exception system", "[exception]") {
  SECTION("Test throw or raise mechanism") {
#ifdef DEBUG
    // Test if an ExcNotImplemented exception would be thrown:
    REQUIRE_THROWS_AS(throw_exception_dbg<ExcNotImplemented>(), ExcNotImplemented);

    // Test if an ExcIO exception would be thrown:
    REQUIRE_THROWS_AS(throw_exception_dbg<ExcIO>(), ExcIO);
#else
    // Test that indeed nothing happens here:
    REQUIRE_NOTHROW(throw_exception_dbg<ExcNotImplemented>());
    REQUIRE_NOTHROW(throw_exception_dbg<ExcIO>());
#endif

    // Test if an ExcNotImplemented exception would be thrown:
    REQUIRE_THROWS_AS(throw_exception<ExcNotImplemented>(), ExcNotImplemented);

    // Test if an ExcIO exception would be thrown:
    REQUIRE_THROWS_AS(throw_exception<ExcIO>(), ExcIO);
  }

  //
  // ----------------------------------------------------------------
  //

  SECTION("Test assertion helper functions") {
    // TODO template this in the type of value and bound
    //      use a testing namespace above for that
    auto test_assert_range = [](long lower_bound, long value) {
      // Arbitrary size:
      auto size = *gen::positive<decltype(value)>();

      // Calculate upper bound
      auto upper_bound = lower_bound + size + 1;

      // Should this assertion fail?
      const bool should_catch_something = (value < lower_bound) || (upper_bound <= value);

      // Classify according to upper property
      RC_CLASSIFY(should_catch_something, "Assertion failed");

#ifdef DEBUG
      if (should_catch_something) {
        // sometimes we catch something in debug mode.
        RC_ASSERT_THROWS_AS(assert_range(lower_bound, value, upper_bound),
                            ExcOutsideRange<decltype(value)>);
      } else {
        // Assert the lower bound. If error, throw
        assert_range(lower_bound, value, upper_bound);
      }
#else
      // we should never catch anything in release mode.
      assert_range(lower_bound, value, upper_bound);
#endif
    };
    CHECK(rc::check("Test assert_range", test_assert_range));

    //
    // ---------------------------------------------------------
    //

    // TODO template this in the type of value and bound
    //      use a testing namespace above for that
    auto test_assert_greater_equal = [](long value1, long value2) {
      // Should this assertion fail?
      const bool should_catch_something = !(value1 <= value2);

      // Classify according to upper property
      RC_CLASSIFY(should_catch_something, "Assertion failed");

#ifdef DEBUG
      if (should_catch_something) {
        // sometimes we catch something in debug mode.
        RC_ASSERT_THROWS_AS(assert_greater_equal(value1, value2),
                            ExcTooLarge<decltype(value1)>);
      } else {
        // Assert that value2 greater or equal than value 2
        // If error throw.
        assert_greater_equal(value1, value2);
      }
#else
      // we should never catch anything in release mode.
      assert_greater_equal(value1, value2);
#endif
    };
    CHECK(rc::check("Test assert_greater_equal", test_assert_greater_equal));

    //
    // ---------------------------------------------------------
    //

    // TODO template this in the type of value and bound
    //      use a testing namespace above for that
    auto test_assert_greater = [](long value1, long value2) {
      // Should this assertion fail?
      const bool should_catch_something = !(value1 < value2);

      // Classify according to upper property
      RC_CLASSIFY(should_catch_something, "Assertion failed");

#ifdef DEBUG
      if (should_catch_something) {
        // sometimes we catch something in debug mode.
        RC_ASSERT_THROWS_AS(assert_greater(value1, value2),
                            ExcTooLargeOrEqual<decltype(value1)>);
      } else {
        // Assert the lower bound. If error, throw
        assert_greater(value1, value2);
      }
#else
      // we should never catch anything in release mode.
      assert_greater(value1, value2);
#endif
    };
    CHECK(rc::check("Test assert_greater", test_assert_greater));

    //
    // ---------------------------------------------------------
    //

    auto test_assert_finite = [] {
      double value =
            *gen::map(gen::pair(gen::arbitrary<double>(),
                                gen::elementOf(std::vector<double>{{0., 1., 2.}})),
                      [](std::pair<double, double> dp) { return dp.first / dp.second; });

      // Should this assertion fail?
      const bool should_catch_something = !std::isfinite(value);

      // Classify according to upper property
      RC_CLASSIFY(should_catch_something, "Assertion failed");

#ifdef DEBUG
      if (should_catch_something) {
        // sometimes we catch something in debug mode.
        RC_ASSERT_THROWS_AS(assert_finite(value), ExcNumberNotFinite);
      } else {
        assert_finite(value);
      }
#else
      // we should never catch anything in release mode.
      assert_greater(value1, value2);
#endif
    };
    CHECK(rc::check("Test assert_finite", test_assert_finite));

    //
    // ---------------------------------------------------------
    //

    auto test_assert_size = [] {
      // The signed data type equivalent to size_t
      typedef typename std::make_signed<size_t>::type ssize;

      // Generate a first size and a difference
      const ssize ssize1 = *gen::positive<ssize>();
      const ssize diff   = *gen::arbitrary<int>();

      // Assert that difference is not too large
      RC_PRE(-diff < ssize1);

      // Build second size:
      const ssize ssize2 = ssize1 + diff;

      // Assert that size2 is positive:
      RC_PRE(ssize2 > 0);

      // Convert both to unsigned size_t
      const size_t size1 = static_cast<size_t>(ssize1);
      const size_t size2 = static_cast<size_t>(ssize2);

      // Should this assertion fail?
      const bool should_catch_something = (size1 != size2);

      // Classify according to upper property
      RC_CLASSIFY(should_catch_something, "Assertion failed");

#ifdef DEBUG
      if (should_catch_something) {
        // sometimes we catch something in debug mode.
        RC_ASSERT_THROWS_AS(assert_size(size1, size2), ExcSizeMismatch);
      } else {
        // Assert the sizes, if they do not match, throw
        assert_size(size1, size2);
      }
#else
      // we should never catch anything in release mode.
      assert_size(size1, size2);
#endif
    };
    CHECK(rc::check("Test assert_size", test_assert_size));

    //
    // ---------------------------------------------------------
    //
    //
    // TODO test for assert_element_sizes
    //      test for assert_finite
    //      test for assert_save_conversion_to
  }
}
}  // namespace tests
}  // namespace krims
