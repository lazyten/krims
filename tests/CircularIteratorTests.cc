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
#include <krims/IteratorUtils.hh>
#include <rapidcheck.h>
#include <vector>

namespace krims {
namespace tests {
using namespace rc;

template <typename Iterator>
void showValue(const CircularIterator<Iterator>& it, std::ostream& os) {
  os << "Position element: " << *it.position() << std::endl
     << "Begin element: " << *it.iteration_range().first << std::endl;
}

//
// ---------------------------------------------------------------
//

TEST_CASE("CircularIteratorTests", "[CircularIterator]") {
  // The type to do the tests with
  typedef int testtype;

  SECTION("Incrementing and decrementing an empty range") {
    auto test = [](std::vector<testtype> v) {
      size_t pos = 0;
      if (v.size() > 0) {
        pos = *gen::inRange<size_t>(0, v.size())
                     .as("position of the empty range.");
      }

      // Construct the empty range iterator:
      typedef CircularIterator<decltype(v.begin())> circit_t;
      circit_t it{std::begin(v) + pos, std::begin(v) + pos,
                  std::begin(v) + pos};

#ifdef DEBUG
      RC_ASSERT_THROWS_AS(++it, ExcInvalidState);
      RC_ASSERT_THROWS_AS(--it, ExcInvalidState);
      RC_ASSERT_THROWS_AS(it++, ExcInvalidState);
      RC_ASSERT_THROWS_AS(it--, ExcInvalidState);
      RC_ASSERT_THROWS_AS(*it, ExcInvalidState);
#endif
    };

    REQUIRE(rc::check(
          "CircularIterator: Incrementing and decrementing an empty range",
          test));
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Check that pre- and post-decrement/increment agree") {
    auto test = [](std::vector<testtype> v) {
      RC_PRE(v.size() > 0u);
      size_t pos = *gen::inRange<size_t>(0, v.size())
                          .as("position to start the circle");

      auto it = circular_begin(v, pos);
      auto itp1 = ++it;
      auto itp2 = it++;
      RC_ASSERT(itp1 == itp2);

      auto itm1 = --it;
      auto itm2 = it--;
      RC_ASSERT(itm1 == itm2);
    };

    REQUIRE(rc::check(
          "CircularIterator: Agreement of pre-/post- decrement/increment",
          test));
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Check that ++(--it) and --(++it)) are identical.") {
    auto test = [](std::vector<testtype> v) {
      RC_PRE(v.size() > 0u);
      size_t pos = *gen::inRange<size_t>(0, v.size())
                          .as("position to start the circle");

      auto it = circular_begin(v, pos);
      auto res1 = --(++it);
      RC_ASSERT(res1 == it);

      auto res2 = ++(--it);
      RC_ASSERT(res2 == it);

      RC_ASSERT(*res1 == *res2);
      RC_ASSERT(res1 == res2);
    };

    REQUIRE(rc::check(
          "CircularIterator: Check that ++(--it) and --(++it)) are identical.",
          test));
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Infinite circular iteration around arbitrary vector.") {
    auto test = [](std::vector<testtype> v) {
      RC_PRE(v.size() > 0u);
      RC_PRE(v.size() < 11u);

      size_t iterations = *gen::inRange<size_t>(1, 10 * v.size())
                                 .as("Number of iterations to carry out");
      size_t startpos = *gen::inRange<size_t>(0, v.size())
                               .as("position to start the circle");

      // Test forward
      auto it = circular_begin(v, startpos);
      for (size_t i = 0; i < iterations; ++i, ++it) {
        size_t i_modded = (startpos + i) % v.size();
        if (v[i_modded] != *it) {
          RC_FAIL("v[" + std::to_string(i_modded) + "] = " +
                  std::to_string(v[i_modded]) + " == " + std::to_string(*it) +
                  " failed in forward.");
        }
      }

      // Test backward
      it = circular_begin(v, startpos);
      for (size_t i = 0; i < iterations; ++i, --it) {
        size_t i_modded = (v.size() - (i % v.size()) + startpos) % v.size();
        if (v[i_modded] != *it) {
          RC_FAIL("v[" + std::to_string(i_modded) + "] = " +
                  std::to_string(v[i_modded]) + " == " + std::to_string(*it) +
                  " failed in backward.");
        }
      }
    };

    REQUIRE(rc::check(
          "CircularIterator: Infinite circular iteration around vector", test));
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Finite iteration around arbitrary vector with offset.") {
    auto test = [](std::vector<testtype> v) {
      size_t startpos = 0;
      if (v.size() > 0) {
        startpos = *gen::inRange<size_t>(0, v.size())
                          .as("position to start the circle");
      }

      // Construct the iterators:
      auto begin = circular_begin(v, startpos);
      auto end = circular_end(v, startpos);

      // Forward iteration:
      size_t n_iter = 0;
      for (; begin != end; ++begin) {
        size_t i_modded = (n_iter + startpos) % v.size();
        if (v[i_modded] != *begin) {
          RC_FAIL("v[" + std::to_string(i_modded) + "] = " +
                  std::to_string(v[i_modded]) + " == " +
                  std::to_string(*begin) + " failed in forward.");
        }
        ++n_iter;
      }
      RC_ASSERT(n_iter == v.size());

      // Backward iteration:
      n_iter = 0;
      begin = circular_begin(v, startpos);
      for (; begin != end; --begin) {
        size_t i_modded = (v.size() - n_iter + startpos) % v.size();
        if (v[i_modded] != *begin) {
          RC_FAIL("v[" + std::to_string(i_modded) + "] = " +
                  std::to_string(v[i_modded]) + " == " +
                  std::to_string(*begin) + " failed in backward.");
        }
        ++n_iter;
      }
      RC_ASSERT(n_iter == v.size());
    };

    REQUIRE(rc::check("CircularIterator: Iteration around vector with offset",
                      test));
  }
}  // TEST_CASE
}  // namespace test
}  // namespace krims
