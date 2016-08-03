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
#include <krims/ParameterMap.hh>
#include <krims/SubscriptionPointer.hh>
#include <rapidcheck.h>

namespace krims {
namespace tests {
using namespace rc;

namespace parametermap_tests {
template <typename T>
struct DummySubscribable : public Subscribable, public std::array<T, 4> {
  typedef std::array<T, 4> base_type;
  DummySubscribable(T d1, T d2, T d3, T d4) {
    this->at(0) = d1;
    this->at(1) = d2;
    this->at(2) = d3;
    this->at(3) = d4;
  }
};
}

// TODO redo these tests with const parameter maps

TEST_CASE("ParameterMap tests", "[parametermap]") {
  using namespace parametermap_tests;

  // Some data:
  int i = 5;
  std::string s{"test"};
  DummySubscribable<double> dum{1., 2., 3., 4.};

  SECTION("Can add data to ParameterMap") {
    // Insert some data into a map.
    ParameterMap m{};
    m.update_copy("string", s);
    m.update_copy("integer", i);
    m.update("dummy", dum);

    // See if we get it back:
    REQUIRE(m.at<int>("integer") == i);
    REQUIRE(m.at<std::string>("string") == s);
    REQUIRE(m.at<DummySubscribable<double>>("dummy") == dum);
  }

//
// ---------------------------------------------------------------
//

#ifdef DEBUG
  SECTION("Check that type safety is assured") {
    // Add data to map.
    ParameterMap m{};
    m.update_copy("s", s);
    m.update_copy("i", i);
    m.update("dum", dum);

    // Extract using the wrong type
    REQUIRE_THROWS_AS(m.at<double>("i"), ParameterMap::ExcWrongTypeRequested);
    REQUIRE_THROWS_AS(m.at<double>("s"), ParameterMap::ExcWrongTypeRequested);
    REQUIRE_THROWS_AS(m.at<double>("dum"), ParameterMap::ExcWrongTypeRequested);
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Check for UnknownKey") {
    // Add data to map.
    ParameterMap m{};
    m.update_copy("i", i);

    REQUIRE_THROWS_AS(m.at<bool>("blubber"), ParameterMap::ExcUnknownKey);
    REQUIRE_THROWS_AS(m.at<int>("blubb"), ParameterMap::ExcUnknownKey);
    REQUIRE(m.at<int>("i") == i);
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Check for Getting the shared pointer back.") {
    // Add data to map.
    ParameterMap m{};
    m.update_copy("string", s);
    m.update_copy("integer", i);
    m.update("dum", dum);

    PointerWrapper<std::string> sptr = m.at_ptr<std::string>("string");
    REQUIRE(sptr.is_shared_ptr());
    REQUIRE(*sptr == s);
    REQUIRE(*static_cast<std::shared_ptr<std::string>>(sptr) == s);

    std::shared_ptr<int> iptr = m.at_ptr<int>("integer");
    REQUIRE(*iptr == i);

    PointerWrapper<DummySubscribable<double>> dumptr =
          m.at_ptr<DummySubscribable<double>>("dum");
    REQUIRE_THROWS_AS(
          auto s =
                static_cast<std::shared_ptr<DummySubscribable<double>>>(dumptr),
          ExcDisabled);
  }
#endif

  //
  // ---------------------------------------------------------------
  //

  SECTION("Test update from cheaply copyable data.") {
    ParameterMap m;
    double d = 3.4;
    m.update("double", d);
    m.update("noref", 3.141592);
    m.update("word", "some");
    m.update("dum", dum);

    REQUIRE(m.at<double>("double") == d);
    REQUIRE(m.at<double>("noref") == 3.141592);
    REQUIRE(m.at<std::string>("word") == "some");
    REQUIRE(m.at<DummySubscribable<double>>("dum") == dum);
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Test retrieving data from const maps") {
    ParameterMap m;
    m.update("double", 1.24);
    m.update("noref", 3.141592);
    m.update("word", "some");
    m.update("dum", dum);

    const ParameterMap& mref{m};
    REQUIRE(mref.at<double>("double") == 1.24);
    REQUIRE(mref.at<double>("noref") == 3.141592);
    REQUIRE(mref.at<std::string>("word") == "some");
    REQUIRE(mref.at<DummySubscribable<double>>("dum") == dum);
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Test at with default return") {
    ParameterMap m;
    m.update("string", s);
    m.update("integer", i);

    REQUIRE(m.at<int>("blubber", 4) == 4);
    REQUIRE(m.at<std::string>("blub", "neun") == "neun");
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Test construction from initialiser list") {
    ParameterMap m{{"value1", 1},
                   {"word", "a"},
                   {"integer", i},
                   {"string", s},
                   {"dum", dum}};

    REQUIRE(m.at<int>("value1") == 1);
    REQUIRE(m.at<std::string>("word") == "a");
    REQUIRE(m.at<int>("integer") == i);
    REQUIRE(m.at<std::string>("string") == s);
    REQUIRE(m.at<DummySubscribable<double>>("dum") == dum);
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Check that data can be erased") {
    // Add data to map.
    ParameterMap m{};
    m.update_copy("s", s);
    m.update_copy("i", i);
    m.update("dum", dum);

    // check it is there:
    REQUIRE(m.exists("i"));
    REQUIRE(m.exists("s"));
    REQUIRE(m.exists("dum"));

    // remove a few:
    m.erase("i");
    m.erase("dum");

    // check they are there (or not)
    REQUIRE(!m.exists("i"));
    REQUIRE(m.exists("s"));
    REQUIRE(!m.exists("dum"));
  }

}  // TEST_CASE
}  // namespace test
}  // namespace krims
