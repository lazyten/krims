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
#include <krims/Algorithm/join.hh>
#include <rapidcheck.h>

namespace krims {
namespace tests {
using namespace rc;

namespace join_tests {

// Run the actual tests:
template <typename Container>
void run_test(const Container& array) {
  const char sepc = *gen::arbitrary<char>();
  RC_PRE(sepc != '\0');
  const std::string sep(1, sepc);

  // Join using a for loop
  std::stringstream res;

  for (auto it = std::begin(array); it != std::end(array); ++it) {
    if (it != std::begin(array)) res << sep;
    res << *it;
  }

  const std::string joined = join(std::begin(array), std::end(array), sep);
  RC_ASSERT(res.str() == joined);
}

//@{
/// The test functor to use.
template <typename Container>
struct TestFunctor {
  void operator()() const {
    const auto size = *gen::inRange<size_t>(0, 101).as("Size of container to join");
    const auto array = *gen::container<Container>(
                              size, gen::arbitrary<typename Container::value_type>())
                              .as("Container to join");
    run_test(array);
  }
};

template <typename T, size_t N>
struct TestFunctor<std::array<T, N>> {
  void operator()() const {
    const auto array = *gen::arbitrary<std::array<T, N>>().as("Container to join");
    run_test(array);
  }
};
//@}

}  // namespace join_tests

TEST_CASE("join function", "[join]") {
  using join_tests::TestFunctor;

  SECTION("Test join with int vector") {
    REQUIRE(check("Join with int vector", TestFunctor<std::vector<int>>{}));
  }
  SECTION("Test join with double vector") {
    REQUIRE(check("Join with double vector", TestFunctor<std::vector<double>>{}));
  }
  SECTION("Test join with strung vector") {
    REQUIRE(check("Join with string vector", TestFunctor<std::vector<std::string>>{}));
  }
  SECTION("Test join with string array") {
    REQUIRE(check("Join with string array", TestFunctor<std::array<std::string, 20>>{}));
  }
  SECTION("Test join with int array") {
    REQUIRE(check("Join with int array", TestFunctor<std::array<int, 20>>{}));
  }
}
}  // namespace tests
}  // namespace krims
