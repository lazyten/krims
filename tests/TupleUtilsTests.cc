
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
#include <krims/TupleUtils.hh>
#include <krims/config.hh>
#include <rapidcheck.h>

namespace krims {
namespace tests {
namespace tuple_utils_tests {
struct Multfac {
  int fac;

  Multfac(int fac_) : fac(fac_) {}

  template <typename T>
  void operator()(T& t) {
    t *= fac;
  }
};

struct Addinto {
  double& into;
  Addinto(double& into_) : into(into_) {}

  template <typename T>
  void operator()(const T& t) {
    into += t;
  }
};

struct Add {
  template <typename T, typename U>
  auto operator()(const T& t, const U& u) -> decltype(u + t) {
    return u + t;
  }
};

struct Minus {
  template <typename T>
  T operator()(const T& t) {
    return -t;
  }
};

struct Greaterzero {
  template <typename T>
  bool operator()(T& t) {
    return (t > 0);
  }
};

}  // namespace tuple_utils_tests

using namespace rc;

TEST_CASE("TupleUtils tests", "[TupleUtils]") {
  SECTION("Test apply") {
    typedef double type1;
    typedef const float type2;
    typedef unsigned int type3;
    typedef const std::string type4;
    typedef std::string type5;

    auto test = [&]() {
      auto t1 = *gen::arbitrary<std::tuple<type1, type2, type3>>().as("First tuple");
      auto t2 = *gen::arbitrary<std::tuple<type4, type5>>().as("Second tuple");

      auto sub     = [](type1 e1, type2 e2, type3 e3) { return e1 - e2 - e3; };
      auto res_sub = krims::apply(sub, t1);
      RC_ASSERT((std::get<0>(t1) - std::get<1>(t1) - std::get<2>(t1)) == res_sub);

      const std::tuple<type1, type2, type3> orig = t1;
      auto inmult = [](type1& e1, const type2& e2, type3& e3) {
        e1 *= e2;
        e3 *= e2;
        return e2;
      };
      auto res_inmult = krims::apply(inmult, t1);
      RC_ASSERT(static_cast<type1>(std::get<0>(orig) * std::get<1>(orig)) ==
                std::get<0>(t1));
      RC_ASSERT(static_cast<type3>(std::get<2>(orig) * std::get<1>(orig)) ==
                std::get<2>(t1));
      RC_ASSERT(res_inmult == std::get<1>(orig) && res_inmult == std::get<1>(t1));

      auto concat     = [](type4 e1, type5 e2) { return e1 + e2; };
      auto res_concat = krims::apply(concat, t2);
      RC_ASSERT(std::get<0>(t2) + std::get<1>(t2) == res_concat);

      auto id     = [](const type3& e2) { return e2; };
      auto val    = *gen::arbitrary<std::tuple<type3>>().as("Tuple for id");
      auto res_id = krims::apply(id, val);
      RC_ASSERT(std::get<0>(val) == res_id);

      auto prod     = []() { return 1; };
      auto res_prod = krims::apply(prod, std::tuple<>{});
      RC_ASSERT(1 == res_prod);

#ifdef KRIMS_HAVE_CXX14
      auto tboth = std::tuple_cat(t1, t2);
      auto add   = [](type1& e1, type2& e2, type3& e3, type4& e4, type5& e5) {
        return std::make_pair(e1 + e2 + e3, e4 + e5);
      };

      auto res_add = krims::apply(add, tboth);
      RC_ASSERT((std::get<0>(tboth) + std::get<1>(tboth) + std::get<2>(tboth)) ==
                res_add.first);
      RC_ASSERT((std::get<3>(tboth) + std::get<4>(tboth)) == res_add.second);
#else
      // For C++11 only up to 4 tuple elements are allowed
      std::tuple<type1, type2, type4, type5> tboth{std::get<0>(t1), std::get<1>(t1),
                                                   std::get<0>(t2), std::get<1>(t2)};
      auto add = [](type1& e1, type2& e2, type4& e4, type5& e5) {
        return std::make_pair(e1 + e2, e4 + e5);
      };

      auto res_add = krims::apply(add, tboth);
      RC_ASSERT((std::get<0>(tboth) + std::get<1>(tboth)) == res_add.first);
      RC_ASSERT((std::get<2>(tboth) + std::get<3>(tboth)) == res_add.second);
#endif
    };

    REQUIRE(rc::check("Testing apply", test));
  }  // Section apply

  SECTION("Test tuple_for_each_0") {
    auto test = [&]() {
      auto t = *gen::arbitrary<std::tuple<>>().as("Tuple");

      auto fac = *gen::arbitrary<int>().as("Multiplication factor");
      tuple_for_each(tuple_utils_tests::Multfac{fac}, t);

      // Accumulating sum.
      double sum{0};
      tuple_for_each(tuple_utils_tests::Addinto{sum}, t);
      RC_ASSERT(sum == 0);
    };

    REQUIRE(rc::check("Testing tuple_for_each for 0 elements", test));
  }  // Section tuple_for_each_0

  SECTION("Test tuple_for_each_1") {
    typedef double type1;

    auto test = [&]() {
      auto torig = *gen::arbitrary<std::tuple<type1>>().as("Tuple");
      auto fac   = *gen::arbitrary<int>().as("Multiplication factor");

      std::tuple<type1> t = torig;
      tuple_for_each(tuple_utils_tests::Multfac{fac}, t);
      RC_ASSERT(std::get<0>(torig) * fac == std::get<0>(t));

      // Accumulating sum.
      double sum{0};
      std::tuple<const type1> tcopy = torig;
      tuple_for_each(tuple_utils_tests::Addinto{sum}, tcopy);
      RC_ASSERT(sum == std::get<0>(torig));
    };

    REQUIRE(rc::check("Testing tuple_for_each for 1 element", test));
  }  // Section tuple_for_each_1

  SECTION("Test tuple_for_each_2") {
    typedef double type1;
    typedef float type2;

    auto test = [&]() {
      auto torig = *gen::arbitrary<std::tuple<type1, type2>>().as("Tuple");
      auto fac   = *gen::arbitrary<int>().as("Multiplication factor");

      std::tuple<type1, type2> t = torig;
      tuple_for_each(tuple_utils_tests::Multfac{fac}, t);
      RC_ASSERT(std::get<0>(torig) * fac == std::get<0>(t));
      RC_ASSERT(std::get<1>(torig) * fac == std::get<1>(t));

      // Accumulating sum.
      double sum{0};
      std::tuple<type1, const type2> tcopy = torig;
      tuple_for_each(tuple_utils_tests::Addinto{sum}, tcopy);
      RC_ASSERT(sum == std::get<0>(torig) + std::get<1>(torig));
    };

    REQUIRE(rc::check("Testing tuple_for_each for 2 elements", test));
  }  // Section tuple_for_each_2

  SECTION("Test tuple_for_each_3") {
    typedef double type1;
    typedef float type2;
    typedef int type3;

    auto test = [&]() {
      auto torig = *gen::arbitrary<std::tuple<type1, type2, type3>>().as("Tuple");
      auto fac   = *gen::arbitrary<int>().as("Multiplication factor");

      std::tuple<type1, type2, type3> t = torig;
      tuple_for_each(tuple_utils_tests::Multfac{fac}, t);
      RC_ASSERT(std::get<0>(torig) * fac == std::get<0>(t));
      RC_ASSERT(std::get<1>(torig) * fac == std::get<1>(t));
      RC_ASSERT(std::get<2>(torig) * fac == std::get<2>(t));

      // Accumulating sum.
      double sum{0};
      std::tuple<type1, const type2, type3> tcopy = torig;
      tuple_for_each(tuple_utils_tests::Addinto{sum}, tcopy);
      RC_ASSERT(sum == std::get<0>(torig) + std::get<1>(torig) + std::get<2>(torig));
    };

    REQUIRE(rc::check("Testing tuple_for_each for 3 elements", test));
  }  // Section tuple_for_each_3

  SECTION("Test tuple_for_each_4") {
    typedef double type1;
    typedef float type2;
    typedef int type3;
    typedef long type4;

    auto test = [&]() {
      auto torig = *gen::arbitrary<std::tuple<type1, type2, type3, type4>>().as("Tuple");
      auto fac   = *gen::arbitrary<int>().as("Multiplication factor");

      std::tuple<type1, type2, type3, type4> t = torig;
      tuple_for_each(tuple_utils_tests::Multfac{fac}, t);
      RC_ASSERT(std::get<0>(torig) * fac == std::get<0>(t));
      RC_ASSERT(std::get<1>(torig) * fac == std::get<1>(t));
      RC_ASSERT(std::get<2>(torig) * fac == std::get<2>(t));
      RC_ASSERT(std::get<3>(torig) * fac == std::get<3>(t));

      // Accumulating sum.
      double sum{0};
      std::tuple<type1, const type2, type3, const type4> tcopy = torig;
      tuple_for_each(tuple_utils_tests::Addinto{sum}, tcopy);
      RC_ASSERT(sum ==
                std::get<0>(torig) + std::get<1>(torig) + std::get<2>(torig) +
                      std::get<3>(torig));
    };

    REQUIRE(rc::check("Testing tuple_for_each for 4 elements", test));
  }  // Section tuple_for_each_4

  SECTION("Test tuple_map_4") {
    typedef double type1;
    typedef float type2;
    typedef int type3;
    typedef long type4;

    auto test = [&]() {
      auto t1 = *gen::arbitrary<std::tuple<type1, type2, type3, type4>>().as("Tuple 1");
      auto t2 = *gen::arbitrary<std::tuple<type1, type4, type3, type2>>().as("Tuple 2");
      auto res_add = tuple_map(tuple_utils_tests::Add{}, t1, t2);
      RC_ASSERT(std::get<0>(t1) + std::get<0>(t2) == std::get<0>(res_add));
      RC_ASSERT(std::get<1>(t1) + std::get<1>(t2) == std::get<1>(res_add));
      RC_ASSERT(std::get<2>(t1) + std::get<2>(t2) == std::get<2>(res_add));
      RC_ASSERT(std::get<3>(t1) + std::get<3>(t2) == std::get<3>(res_add));

      auto res_minus = tuple_map(tuple_utils_tests::Minus{}, t1);
      RC_ASSERT(std::get<0>(t1) == -std::get<0>(res_minus));
      RC_ASSERT(std::get<1>(t1) == -std::get<1>(res_minus));
      RC_ASSERT(std::get<2>(t1) == -std::get<2>(res_minus));
      RC_ASSERT(std::get<3>(t1) == -std::get<3>(res_minus));
    };

    REQUIRE(rc::check("Testing tuple_map for 4 elements", test));
  }  // Section tuple_map_4

  SECTION("Test tuple_map_3") {
    typedef double type1;
    typedef float type2;
    typedef long type3;

    auto test = [&]() {
      auto t1      = *gen::arbitrary<std::tuple<type1, type2, type3>>().as("Tuple 1");
      auto t2      = *gen::arbitrary<std::tuple<type1, type3, type2>>().as("Tuple 2");
      auto res_add = tuple_map(tuple_utils_tests::Add{}, t1, t2);
      RC_ASSERT(std::get<0>(t1) + std::get<0>(t2) == std::get<0>(res_add));
      RC_ASSERT(std::get<1>(t1) + std::get<1>(t2) == std::get<1>(res_add));
      RC_ASSERT(std::get<2>(t1) + std::get<2>(t2) == std::get<2>(res_add));

      auto res_minus = tuple_map(tuple_utils_tests::Minus{}, t1);
      RC_ASSERT(std::get<0>(t1) == -std::get<0>(res_minus));
      RC_ASSERT(std::get<1>(t1) == -std::get<1>(res_minus));
      RC_ASSERT(std::get<2>(t1) == -std::get<2>(res_minus));
    };

    REQUIRE(rc::check("Testing tuple_map for 3 elements", test));
  }  // Section tuple_map_3

  SECTION("Test tuple_map_2") {
    typedef double type1;
    typedef float type2;

    auto test = [&]() {
      auto t1      = *gen::arbitrary<std::tuple<type1, type2>>().as("Tuple 1");
      auto t2      = *gen::arbitrary<std::tuple<type2, type1>>().as("Tuple 2");
      auto res_add = tuple_map(tuple_utils_tests::Add{}, t1, t2);
      RC_ASSERT(std::get<0>(t1) + std::get<0>(t2) == std::get<0>(res_add));
      RC_ASSERT(std::get<1>(t1) + std::get<1>(t2) == std::get<1>(res_add));

      auto res_minus = tuple_map(tuple_utils_tests::Minus{}, t1);
      RC_ASSERT(std::get<0>(t1) == -std::get<0>(res_minus));
      RC_ASSERT(std::get<1>(t1) == -std::get<1>(res_minus));
    };

    REQUIRE(rc::check("Testing tuple_map for 2 elements", test));
  }  // Section tuple_map_2

  SECTION("Test tuple_map_1") {
    typedef double type1;

    auto test = [&]() {
      auto t1      = *gen::arbitrary<std::tuple<type1>>().as("Tuple 1");
      auto t2      = *gen::arbitrary<std::tuple<type1>>().as("Tuple 2");
      auto res_add = tuple_map(tuple_utils_tests::Add{}, t1, t2);
      RC_ASSERT(std::get<0>(t1) + std::get<0>(t2) == std::get<0>(res_add));
      auto res_minus = tuple_map(tuple_utils_tests::Minus{}, t1);
      RC_ASSERT(std::get<0>(t1) == -std::get<0>(res_minus));
    };

    REQUIRE(rc::check("Testing tuple_map for 1 elements", test));
  }  // Section tuple_map_1

}  // TEST_CASE
}  // namespace tests
}  // namespace krims
