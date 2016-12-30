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

    RCPWrapper<std::string> sptr = m.at_ptr<std::string>("string");
    REQUIRE(sptr.is_shared_ptr());
    REQUIRE(*sptr == s);
    REQUIRE(*static_cast<std::shared_ptr<std::string>>(sptr) == s);

    std::shared_ptr<int> iptr = m.at_ptr<int>("integer");
    REQUIRE(*iptr == i);

    RCPWrapper<DummySubscribable<double>> dumptr =
          m.at_ptr<DummySubscribable<double>>("dum");
    REQUIRE_THROWS_AS(
          auto s = static_cast<std::shared_ptr<DummySubscribable<double>>>(dumptr),
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

  SECTION("Test update for various pointer types.") {
    ParameterMap m;
    auto dptr = std::make_shared<double>(3.4);
    auto sptr = make_subscription(dum, "dum");
    RCPWrapper<DummySubscribable<double>> rcpwrap(sptr);

    m.update("double", dptr);
    m.update("rcp", rcpwrap);

    REQUIRE(m.at<double>("double") == *dptr);
    REQUIRE(m.at<DummySubscribable<double>>("rcp") == dum);
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
    ParameterMap m{
          {"value1", 1}, {"word", "a"}, {"integer", i}, {"string", s}, {"dum", dum}};

    REQUIRE(m.at<int>("value1") == 1);
    REQUIRE(m.at<std::string>("word") == "a");
    REQUIRE(m.at<int>("integer") == i);
    REQUIRE(m.at<std::string>("string") == s);
    REQUIRE(m.at<DummySubscribable<double>>("dum") == dum);
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Test insert_default") {
    const ParameterMap m{{"double", 3.4}, {"pi", 3.141592}};
    REQUIRE(m.at<double>("double") == 3.4);
    REQUIRE(m.at<double>("pi") == 3.141592);

    m.insert_default("string", "blubba");
    m.insert_default({{"one", 1}, {"two", 2}});

    REQUIRE(m.at<std::string>("string") == "blubba");
    REQUIRE(m.at<int>("one") == 1);
    REQUIRE(m.at<int>("two") == 2);
    REQUIRE(m.at<double>("double") == 3.4);
    REQUIRE(m.at<double>("pi") == 3.141592);
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Check basic path transformations") {
    // Add data to map.
    ParameterMap m{};
    m.update("one/two/three", "3");
    m.update("three/two/one", 4);
    m.update("", "test");

    // check it is there:
    REQUIRE(m.at<std::string>("one/two/three") == "3");
    REQUIRE(m.at<int>("three/two/one") == 4);
    REQUIRE(m.at<std::string>("") == "test");

    // Check the path normalisation:
    REQUIRE(m.at<std::string>("/one/two/three") == "3");
    REQUIRE(m.at<std::string>("/one/two//three") == "3");
    REQUIRE(m.at<int>("three/./two/one") == 4);
    REQUIRE(m.at<int>("three/./two/./one") == 4);
    REQUIRE(m.at<std::string>("one/two/three/") == "3");
    REQUIRE(m.at<int>("three/two/one/.") == 4);
    REQUIRE(m.at<std::string>("/one/./two/////three") == "3");
    REQUIRE(m.at<int>("/././/three///two/./one/") == 4);
    REQUIRE(m.at<int>("three/two/../two/one") == 4);
    REQUIRE(m.at<int>("../../../three/two/one") == 4);
    REQUIRE(m.at<int>("/../../../one/../three/two/one") == 4);

    REQUIRE(m.at<std::string>("/") == "test");
    REQUIRE(m.at<std::string>(".") == "test");
    REQUIRE(m.at<std::string>("/../") == "test");
    REQUIRE(m.at<std::string>("/../.") == "test");
    REQUIRE(m.at<std::string>("/.././") == "test");
    REQUIRE(m.at<std::string>(".././") == "test");
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
    m.erase("/i/././");
    m.erase("dum");

    // check they are there (or not)
    REQUIRE(!m.exists("i"));
    REQUIRE(m.exists("s"));
    REQUIRE(!m.exists("dum"));

    // Add more data to map
    m.update({{"tree/sub", s},
              {"tree/i", i},
              {"dum", dum},
              {"tree/value", 9},
              {"tree", "root"},
              {"/", "god"}});
    REQUIRE(m.exists("tree/sub"));
    REQUIRE(m.exists("tree/i"));
    REQUIRE(m.exists("dum"));
    REQUIRE(m.exists("tree/value"));
    REQUIRE(m.exists("tree"));
    REQUIRE(m.exists("/"));
    REQUIRE(m.exists("s"));

    m.erase_recursive("tree");
    REQUIRE(m.exists("/"));
    REQUIRE(m.exists("s"));
    REQUIRE(m.exists("dum"));
    REQUIRE_FALSE(m.exists("tree/sub"));
    REQUIRE_FALSE(m.exists("tree/i"));
    REQUIRE_FALSE(m.exists("tree/value"));
    REQUIRE_FALSE(m.exists("tree"));

    auto key = *m.begin_keys();
    m.erase(m.begin_keys());
    REQUIRE_FALSE(m.exists(key));
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Check submap functionality") {
    // Add data to map.
    ParameterMap m{{"tree/sub", s},   {"tree/i", i},    {"dum", dum},
                   {"tree/value", 9}, {"tree", "root"}, {"/", "god"}};

    // check it is there:
    REQUIRE(m.exists("tree/sub"));
    REQUIRE(m.exists("tree/i"));
    REQUIRE(m.exists("tree"));
    REQUIRE(m.exists("tree/value"));
    REQUIRE(m.exists("dum"));
    REQUIRE(m.exists("/"));

    ParameterMap sub = m.submap("tree");

    // Check existence:
    REQUIRE_FALSE(sub.exists("tree/sub"));
    REQUIRE_FALSE(sub.exists("tree/i"));
    REQUIRE_FALSE(sub.exists("dum"));
    REQUIRE(sub.exists("sub"));
    REQUIRE(sub.exists("i"));
    REQUIRE(sub.exists("value"));
    REQUIRE(sub.exists("/"));

    // Check value is appropriate:
    REQUIRE(sub.at<std::string>("sub") == s);
    REQUIRE(sub.at<int>("i") == i);
    REQUIRE(sub.at<int>("value") == 9);
    REQUIRE(sub.at<std::string>("/") == "root");
    REQUIRE(sub.at<std::string>("..") == "root");
    REQUIRE(sub.at<std::string>("../..") == "root");
    REQUIRE(sub.at<std::string>("/../.") == "root");
    REQUIRE(sub.at<std::string>(".././") == "root");
    REQUIRE(m.at<std::string>("/") == "god");

    // Check adding a new value in the submap:
    sub.update("neu", 1.23);
    sub.update("value", 10);
    REQUIRE(m.at<double>("tree/neu") == 1.23);
    REQUIRE(m.at<int>("tree/value") == 10);

    // Check path normalisation for submap:
    ParameterMap sub2 = m.submap("/./tree/.");
    REQUIRE(sub2.at<std::string>("sub") == s);
    REQUIRE(sub2.at<int>("i") == i);
    REQUIRE(sub2.at<int>("value") == 10);

    // Check that clearing a subtree leaves the rest intact
    sub.clear();
    REQUIRE(m.at<std::string>("/") == "god");
    REQUIRE(m.exists("dum"));
    REQUIRE(m.exists("/"));
    REQUIRE_FALSE(m.exists("neu"));
    REQUIRE_FALSE(m.exists("tree/neu"));
    REQUIRE_FALSE(m.exists("tree"));
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Check begin_keys() and end_keys()") {
    // Add data to map.
    ParameterMap m{{"tree/sub", s},  {"tree/i", i}, {"dum", dum},    {"tree/value", 9},
                   {"tree", "root"}, {"/", "god"},  {"/zzz", "end"}, {"/zz", "mend"}};

    // Check we get all keys for starters:
    std::vector<std::string> ref{"/",         "/dum",        "/tree", "/tree/i",
                                 "/tree/sub", "/tree/value", "/zz",   "/zzz"};
    auto itref = std::begin(ref);
    for (auto it = m.begin_keys(); it != m.end_keys(); ++it, ++itref) {
      REQUIRE(itref != std::end(ref));
      CHECK(*itref == *it);
    }
    CHECK(itref == std::end(ref));

    // Get a submap:
    ParameterMap sub = m.submap("tree");

    // Check we get all keys of the submap:
    std::vector<std::string> subref{"/", "/i", "/sub", "/value"};
    auto itsubref = std::begin(subref);
    for (auto it = sub.begin_keys(); it != sub.end_keys(); ++it, ++itsubref) {
      REQUIRE(itsubref != std::end(subref));
      CHECK(*itsubref == *it);
    }
    CHECK(itsubref == std::end(subref));

    itsubref = std::begin(subref);
    for (auto it = m.begin_keys("tree"); it != m.end_keys("tree"); ++it, ++itsubref) {
      REQUIRE(itsubref != std::end(subref));
      CHECK(*itsubref == *it);
    }
    CHECK(itsubref == std::end(subref));
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Check that copying submaps works.") {
    // Add data to map.
    ParameterMap m{{"tree/sub", s},   {"tree/i", i},    {"dum", dum},
                   {"tree/value", 9}, {"tree", "root"}, {"/", "god"}};

    // Get a submap:
    const ParameterMap sub = m.submap("/tree");

    // Copy it:
    ParameterMap copy = sub;

    // Check existence:
    REQUIRE_FALSE(copy.exists("tree/sub"));
    REQUIRE_FALSE(copy.exists("tree/i"));
    REQUIRE_FALSE(copy.exists("dum"));
    REQUIRE(copy.exists("sub"));
    REQUIRE(copy.exists("i"));
    REQUIRE(copy.exists("value"));
    REQUIRE(copy.exists("/"));

    // Check value is appropriate:
    REQUIRE(copy.at<std::string>("sub") == s);
    REQUIRE(copy.at<int>("i") == i);
    REQUIRE(copy.at<int>("value") == 9);
    REQUIRE(copy.at<std::string>("/") == "root");
    REQUIRE(copy.at<std::string>("..") == "root");
    REQUIRE(copy.at<std::string>("../..") == "root");
    REQUIRE(copy.at<std::string>("/../.") == "root");
    REQUIRE(copy.at<std::string>(".././") == "root");
    REQUIRE(m.at<std::string>("/") == "god");

    // Check adding a new value in the copy does not
    // affect original.
    copy.update("neu", 1.23);
    copy.update("value", 10);
    REQUIRE_FALSE(m.exists("tree/neu"));
    REQUIRE(m.at<int>("tree/value") == 9);
    REQUIRE_FALSE(sub.exists("neu"));
    REQUIRE(sub.at<int>("value") == 9);
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Check that updating from other maps works.") {
    // Add data to map.
    ParameterMap m{{"tree/sub", s},   {"tree/i", i},    {"dum", dum},
                   {"tree/value", 9}, {"tree", "root"}, {"/", "god"}};

    // Make a second map
    ParameterMap n{{"house/window", true},
                   {"house/open", "14-15"},
                   {"garden", false},
                   {"blubber", "blabla"},
                   {"house/value", 14}};

    // Make submaps:
    ParameterMap subm = m.submap("tree");
    ParameterMap subn = n.submap("house");

    CHECK(subn.at<bool>("window") == true);
    CHECK(subn.at<std::string>("open") == "14-15");
    CHECK(subn.at<int>("value") == 14);
    CHECK(subm.at<int>("i") == i);
    CHECK(subm.at<int>("value") == 9);
    CHECK_FALSE(subm.exists("window"));
    CHECK_FALSE(subm.exists("open"));

    subm.update(subn);

    CHECK(subm.at<int>("i") == i);
    CHECK(subm.at<int>("value") == 14);
    CHECK(subm.at<bool>("window") == true);
    CHECK(subm.at<std::string>("open") == "14-15");
    CHECK(m.at<int>("tree/value") == 14);
    CHECK(m.at<std::string>("tree/open") == "14-15");
    CHECK(m.at<bool>("tree/window") == true);

    m.update("tree/value", 42);
    CHECK(subm.at<int>("value") == 42);
    CHECK(subn.at<int>("value") == 14);
    CHECK(n.at<int>("house/value") == 14);

    std::vector<std::string> ref{"/",           "/dum",        "/tree",
                                 "/tree/i",     "/tree/open",  "/tree/sub",
                                 "/tree/value", "/tree/window"};
    auto itref = std::begin(ref);
    for (auto it = m.begin_keys(); it != m.end_keys(); ++it, ++itref) {
      REQUIRE(itref != std::end(ref));
      CHECK(*itref == *it);
    }
    CHECK(itref == std::end(ref));

    std::vector<std::string> refn{"/blubber", "/garden", "/house/open", "/house/value",
                                  "/house/window"};
    auto itrefn = std::begin(refn);
    for (auto it = n.begin_keys(); it != n.end_keys(); ++it, ++itrefn) {
      REQUIRE(itrefn != std::end(refn));
      CHECK(*itrefn == *it);
    }
    CHECK(itrefn == std::end(refn));

    // Append map n to m:
    m.update("mapn", n);

    std::vector<std::string> refm{"/",
                                  "/dum",
                                  "/mapn/blubber",
                                  "/mapn/garden",
                                  "/mapn/house/open",
                                  "/mapn/house/value",
                                  "/mapn/house/window",
                                  "/tree",
                                  "/tree/i",
                                  "/tree/open",
                                  "/tree/sub",
                                  "/tree/value",
                                  "/tree/window"};

    auto itrefm = std::begin(refm);
    for (auto it = m.begin_keys(); it != m.end_keys(); ++it, ++itrefm) {
      REQUIRE(itrefm != std::end(refm));
      CHECK(*itrefm == *it);
    }
    CHECK(itrefm == std::end(refm));

    CHECK(m.at<int>("mapn/house/value") == 14);
    CHECK(m.at<bool>("mapn/house/window") == true);
    CHECK(m.at<std::string>("mapn/house/open") == "14-15");
  }

  //
  // ---------------------------------------------------------------
  //

  // TODO Test that changing data in copies / submaps does the intended thing
  // TODO Test mass update from initialiser list

}  // TEST_CASE
}  // namespace test
}  // namespace krims
