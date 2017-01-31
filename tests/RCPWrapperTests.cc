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
#include <krims/RCPWrapper.hh>
#include <vector>

namespace krims {
namespace tests {
namespace rcpwrapper_tests {

/** An extremely simple class which is also
 *  subscribable */
struct SimpleSubscribable : public Subscribable {
  int data;
  SimpleSubscribable() : data(0){};
  explicit SimpleSubscribable(int d) : data{d} {};

  bool operator==(const SimpleSubscribable& other) const { return data == other.data; }
  bool operator!=(const SimpleSubscribable& other) const { return !operator==(other); }
};

/** A derived subscribable class */
struct SimpleSubscribableChild : public SimpleSubscribable {
  explicit SimpleSubscribableChild(int d) : SimpleSubscribable(d) {}
  SimpleSubscribableChild() = default;
};
}  // namespace rcpwrapper_tests

TEST_CASE("RCPWrapperTests", "[RCPWrapper]") {
  using namespace rcpwrapper_tests;

  // A SimpleSubscribableChild with data 1
  SimpleSubscribableChild s(1);

  // A subscription to it
  auto ssub = make_subscription(s, "test");

  // A shared pointer to a SimpleSubscribableChild with content 5
  auto sshared = std::make_shared<SimpleSubscribableChild>(5);

  SECTION("Make RCPWrapper and check content") {
    RCPWrapper<SimpleSubscribableChild> subwrap(ssub);
    RCPWrapper<SimpleSubscribableChild> ptrwrap(sshared);

    REQUIRE(!subwrap.is_shared_ptr());
    REQUIRE(ptrwrap.is_shared_ptr());
    REQUIRE(subwrap->data == 1);
    REQUIRE(ptrwrap->data == 5);

    // Implicit conversion
    RCPWrapper<SimpleSubscribable> basewrap(subwrap);
    REQUIRE(basewrap->data == 1);
  }  // RCPWrapper from SubscriptionPointer

  SECTION("RCPWrapper copy constructor") {
    RCPWrapper<SimpleSubscribableChild> subwrap(ssub);
    RCPWrapper<SimpleSubscribableChild> ptrwrap(sshared);

    // Copy constructor
    // (The NOLINT comment suppresses clang-tidy warnings about the unnecessary copy)
    RCPWrapper<SimpleSubscribableChild> subwrap2(subwrap);  // NOLINT
    RCPWrapper<SimpleSubscribableChild> ptrwrap2(ptrwrap);  // NOLINT
    REQUIRE(!subwrap2.is_shared_ptr());
    REQUIRE(ptrwrap2.is_shared_ptr());
    REQUIRE(subwrap2->data == 1);
    REQUIRE(ptrwrap2->data == 5);

    // Modify data and check that original affected as well.
    subwrap2->data = 42;
    ptrwrap2->data = 43;

    REQUIRE(subwrap->data == 42);
    REQUIRE(ptrwrap->data == 43);
  }  // RCPWrapper copy constructor

  SECTION("Conversion from RCPWrapper to shared and subscription pointer") {
    // Conversion to subscription pointer
    RCPWrapper<SimpleSubscribableChild> subwrap(ssub);
#if DEBUG
    REQUIRE(s.n_subscriptions() == 2);
    REQUIRE(s.subscribers()[0] == "test");
    REQUIRE(s.subscribers()[1] == "test");
#endif

    SubscriptionPointer<SimpleSubscribableChild> sptr1 = subwrap;
    REQUIRE(sptr1->data == 1);
#if DEBUG
    REQUIRE(s.n_subscriptions() == 3);
    REQUIRE(s.subscribers()[2] == "test");
    REQUIRE(s.subscribers()[1] == "test");
    REQUIRE(s.subscribers()[0] == "test");
#endif

    // Conversion to shared pointer
    RCPWrapper<SimpleSubscribableChild> ptrwrap(sshared);
    std::shared_ptr<SimpleSubscribableChild> ptr(ptrwrap);
    REQUIRE(ptr->data == 5);

    SubscriptionPointer<SimpleSubscribableChild> sptr2 = ptrwrap;
    REQUIRE(sptr2->data == 5);
#if DEBUG
    REQUIRE(sshared->n_subscriptions() == 1);
    REQUIRE(sshared->subscribers()[0] == "RCPWrapper");
#endif
  }  // RCPWrapper from shared and subscription pointers

}  // TEST_CASE
}  // namespace tests
}  // namespace krims
