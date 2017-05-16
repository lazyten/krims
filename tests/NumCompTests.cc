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
#include <krims/NumComp.hh>
#include <rapidcheck.h>

namespace krims {
namespace tests {

using namespace rc;

TEST_CASE("NumComp tests", "[NumComp]") {
  // Throw on numerical error
  NumCompConstants::default_failure_action = NumCompActionType::ThrowVerbose;

  SECTION("Test NumCompConstants::CacheChange") {
    // Set some failure action and some tolerance
    NumCompConstants::default_tolerance_factor = 1e3;

    // change inside a region:
    {
      CHECK(NumCompConstants::default_tolerance_factor == 1e3);
      CHECK(NumCompConstants::default_failure_action == NumCompActionType::ThrowVerbose);
      auto cache_change = NumCompConstants::change_temporary(1e4);
      CHECK(NumCompConstants::default_tolerance_factor == 1e4);
    }  // newtol goes out of scope
    CHECK(NumCompConstants::default_tolerance_factor == 1e3);
    CHECK(NumCompConstants::default_failure_action == NumCompActionType::ThrowVerbose);

    // Nested change
    {
      CHECK(NumCompConstants::default_tolerance_factor == 1e3);
      CHECK(NumCompConstants::default_failure_action == NumCompActionType::ThrowVerbose);
      auto cache_change = NumCompConstants::change_temporary(1e4);
      CHECK(NumCompConstants::default_tolerance_factor == 1e4);
      CHECK(NumCompConstants::default_failure_action == NumCompActionType::ThrowVerbose);
      {
        auto cache_change = NumCompConstants::change_temporary(1e2);
        CHECK(NumCompConstants::default_tolerance_factor == 1e2);
        CHECK(NumCompConstants::default_failure_action ==
              NumCompActionType::ThrowVerbose);
      }
      CHECK(NumCompConstants::default_tolerance_factor == 1e4);
      CHECK(NumCompConstants::default_failure_action == NumCompActionType::ThrowVerbose);
    }  // newtol goes out of scope
    CHECK(NumCompConstants::default_tolerance_factor == 1e3);
    CHECK(NumCompConstants::default_failure_action == NumCompActionType::ThrowVerbose);

    // change action type inside a region:
    {
      CHECK(NumCompConstants::default_tolerance_factor == 1e3);
      CHECK(NumCompConstants::default_failure_action == NumCompActionType::ThrowVerbose);
      auto cache_change = NumCompConstants::change_temporary(NumCompActionType::Return);
      CHECK(NumCompConstants::default_tolerance_factor == 1e3);
      CHECK(NumCompConstants::default_failure_action == NumCompActionType::Return);
    }  // newtol goes out of scope
    CHECK(NumCompConstants::default_tolerance_factor == 1e3);
    CHECK(NumCompConstants::default_failure_action == NumCompActionType::ThrowVerbose);

    // change both inside a region:
    {
      CHECK(NumCompConstants::default_tolerance_factor == 1e3);
      CHECK(NumCompConstants::default_failure_action == NumCompActionType::ThrowVerbose);
      auto cache_change =
            NumCompConstants::change_temporary(1e5, NumCompActionType::Return);
      CHECK(NumCompConstants::default_tolerance_factor == 1e5);
      CHECK(NumCompConstants::default_failure_action == NumCompActionType::Return);
      {
        auto cache_change =
              NumCompConstants::change_temporary(1e7, NumCompActionType::ThrowNormal);
        CHECK(NumCompConstants::default_tolerance_factor == 1e7);
        CHECK(NumCompConstants::default_failure_action == NumCompActionType::ThrowNormal);
      }
      CHECK(NumCompConstants::default_tolerance_factor == 1e5);
      CHECK(NumCompConstants::default_failure_action == NumCompActionType::Return);
    }  // newtol goes out of scope
    CHECK(NumCompConstants::default_tolerance_factor == 1e3);
    CHECK(NumCompConstants::default_failure_action == NumCompActionType::ThrowVerbose);
  }  // Test NumCompConstants::CacheChange

  //
  // -----------------------------------------------
  //

  SECTION("Test numcomp with extremely sloppy tolerance") {
    // Use a very sloppy tolerance:
    NumCompConstants::default_tolerance_factor = 1e6;

    // Note: The cast to void is to "fake-use" the comparsion result, which is
    // not of interest in this case
    REQUIRE_THROWS_AS((void)(10.000 == numcomp(10.001)), NumCompException<double>);

    try {
      (void)(10.000 == numcomp(10.001).tolerance(NumCompAccuracyLevel::SuperSloppy));
    } catch (const NumCompException<double>& e) {
      CHECK(Approx(e.tolerance) == 1e9 * std::numeric_limits<double>::epsilon());
      CHECK(e.error == Approx(0.0001));
    }

    try {
      (void)(0.000 == numcomp(10.001).tolerance(NumCompAccuracyLevel::Sloppy));
    } catch (const NumCompException<double>& e) {
      CHECK(Approx(e.tolerance) == 1e8 * std::numeric_limits<double>::epsilon());
      CHECK(e.error == Approx(1.));
    }

    try {
      (void)(0.002 == numcomp(0.001).tolerance(NumCompAccuracyLevel::Lower));
    } catch (const NumCompException<double>& e) {
      CHECK(Approx(e.tolerance) == 1e7 * std::numeric_limits<double>::epsilon());
      CHECK(e.error == Approx(0.001));
    }

    try {
      (void)(0.00003 == numcomp(0.00001));
    } catch (const NumCompException<double>& e) {
      CHECK(Approx(e.tolerance) == 1e6 * std::numeric_limits<double>::epsilon());
      CHECK(e.error == Approx(0.00002));
    }

    try {
      (void)(0. == numcomp(0.0000001).tolerance(NumCompAccuracyLevel::Extreme));
    } catch (const NumCompException<double>& e) {
      CHECK(Approx(e.tolerance) == 1e4 * std::numeric_limits<double>::epsilon());
      CHECK(e.error == Approx(0.0000001));
    }

    REQUIRE_THROWS_AS(
          (void)(0. == numcomp(0.0000001).tolerance(NumCompAccuracyLevel::Extreme)),
          NumCompException<double>);

    REQUIRE_THROWS_AS((void)(0. == numcomp(0.0000001).tolerance(1e-10)),
                      NumCompException<double>);
    REQUIRE(0. == numcomp(0.0000001).tolerance(1e-4));

    REQUIRE(100.000000 == numcomp(100.000001).tolerance(1e-8));
    REQUIRE(10.0 == numcomp(10.01).tolerance(1e-3));

    // Test some different types:
    REQUIRE(10.0f == numcomp(10.01).tolerance(1e-3));
    REQUIRE(std::complex<double>(1., 2.) ==
            numcomp(std::complex<double>(1.000001, 1.99999999999)).tolerance(1e-6));
    REQUIRE(std::complex<float>(1., 2.) ==
            numcomp(std::complex<double>(1.000001, 1.99999999999)).tolerance(1e-6));
  }  // SECTION Test numcomp with extremely sloppy tolerance

  SECTION("Test numcomp with std::vectors") {
    // Use a very sloppy tolerance:
    NumCompConstants::default_tolerance_factor = 1e10;

    // Setup two vectors where the 3rd element is problematic
    std::vector<double> v1{1.000001, 2.0000001, 10.000, 14.00000};
    std::vector<double> v2{1.000000, 2.0000001, 10.001, 14.00001};

    REQUIRE_THROWS_AS((void)(v1 == numcomp(v2)), NumCompException<double>);

    try {
      (void)(v1 == numcomp(v2));
    } catch (const NumCompException<double>& e) {
      CHECK(Approx(e.tolerance) == 1e6 * std::numeric_limits<double>::epsilon());
      CHECK(e.error == Approx(0.0001));
    }

    // Setup a vector of different length
    std::vector<double> v3{1.000000, 2.0000001, 10.001};
    REQUIRE_THROWS_AS((void)(v3 == numcomp(v2)), NumCompException<double>);
  }

}  // TEST_CASE
}  // namespace tests
}  // namespace krims
