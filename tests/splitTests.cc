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
#include <iterator>
#include <krims/Algorithm/join.hh>
#include <krims/Algorithm/split.hh>
#include <rapidcheck.h>

namespace krims {
namespace tests {

// Run the actual tests:
template <typename Container>
void run_test(const Container& array, char sepc) {
  const std::string sep(1, sepc);
  const std::string joined = join(std::begin(array), std::end(array), sep);
  RC_LOG() << "String to split: " << joined << std::endl;

  std::vector<std::string> splitted;
  std::back_insert_iterator<decltype(splitted)> backit(splitted);
  split(joined, backit, sepc);

  if (joined.empty()) {
    RC_ASSERT(splitted.empty());
  } else {
    RC_ASSERT(splitted.size() == array.size());
    auto itar = std::begin(array);
    auto itsp = std::begin(splitted);
    for (; itar != std::end(array); ++itar, ++itsp) {
      RC_ASSERT(*itar == *itsp);
    }
  }
}

rc::Gen<std::string> string_gen(char sep) {
  using namespace rc;
  auto char_gen = gen::distinctFrom(gen::arbitrary<char>(), sep);
  return gen::container<std::string>(char_gen);
}

TEST_CASE("split function", "[split]") {
  using namespace rc;

  SECTION("Test split with vector") {
    typedef std::vector<std::string> con_type;
    auto testable = [] {
      const char sepc = *gen::elementOf(std::vector<char>{',', '.', ' ', '\n', 'a', 'z',
                                                          '3', '1', '\t'})
                               .as("Separator character");
      const auto size = *rc::gen::inRange<size_t>(0, 101).as("Size of container");
      const auto array =
            *rc::gen::container<con_type>(size, string_gen(sepc)).as("container");
      run_test(array, sepc);
    };
    REQUIRE(rc::check("Split with vector", testable));
  }

  SECTION("Test split with array") {
    typedef std::array<std::string, 20> con_type;
    auto testable = [] {
      const char sepc  = *gen::nonZero<char>().as("Separator character");
      const auto array = *rc::gen::container<con_type>(string_gen(sepc)).as("container");
      run_test(array, sepc);
    };
    REQUIRE(rc::check("Split with array", testable));
  }
}  // split

}  // namespace tests
}  // namespace krims
