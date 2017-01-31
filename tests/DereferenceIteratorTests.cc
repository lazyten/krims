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
#include <krims/IteratorUtils.hh>
#include <rapidcheck.h>
#include <vector>

namespace krims {
namespace tests {
using namespace rc;

template <typename Iterator>
void showValue(const DereferenceIterator<Iterator>& it, std::ostream& o) {  // NOLINT
  o << "Inner iterator: " << it.operator->() << std::endl
    << "element: " << *it << std::endl;
}

//
// ---------------------------------------------------------------
//

TEST_CASE("DereferenceIteratorTests", "[DereferenceIterator]") {
  // The type to do the tests with
  typedef int testtype;

  SECTION("Test DereferenceIterator of normal std::vector iterator") {
    auto test = [] {
      typedef std::vector<std::shared_ptr<testtype>> cont_type;
      cont_type v = *gen::container<cont_type>(
                           gen::makeShared<testtype>(gen::arbitrary<testtype>()))
                           .as("Test container");

      auto it = std::begin(v);
      DereferenceIterator<decltype(it)> itbegin(it);
      const DereferenceIterator<decltype(it)> itend(std::end(v));

      size_t n_elem = 0;
      for (; itbegin != itend; ++itbegin, ++it, ++n_elem) {
        RC_ASSERT(*itbegin == **it);
      }
      RC_ASSERT(n_elem == v.size());
    };

    REQUIRE(
          rc::check("DereferenceIterator: Running over the full range of "
                    "non-const iterator",
                    test));
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Test DereferenceIterator of constant std::vector iterator") {
    auto test = [] {
      typedef std::vector<std::shared_ptr<testtype>> cont_type;
      cont_type v = *gen::container<cont_type>(
                           gen::makeShared<testtype>(gen::arbitrary<testtype>()))
                           .as("Test container");

      auto it = v.cbegin();
      DereferenceIterator<decltype(it)> itbegin(it);
      const DereferenceIterator<decltype(it)> itend(v.cend());

      size_t n_elem = 0;
      for (; itbegin != itend; ++itbegin, ++it, ++n_elem) {
        RC_ASSERT(*itbegin == **it);
      }
      RC_ASSERT(n_elem == v.size());
    };

    REQUIRE(rc::check(
          "DereferenceIterator: Running over the full range of const iterator", test));
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Check that pre- and post-decrement/increment agree") {
    auto test = [] {
      typedef std::vector<std::shared_ptr<testtype>> cont_type;
      cont_type v = *gen::container<cont_type>(
                           gen::makeShared<testtype>(gen::arbitrary<testtype>()))
                           .as("Test container");

      RC_PRE(v.size() > 0u);
      size_t pos = *gen::inRange<size_t>(0, v.size()).as("position to offset to");

      DereferenceIterator<typename cont_type::iterator> it(std::begin(v));
      it += pos;

      auto itp1 = ++it;
      auto itp2 = it++;
      RC_ASSERT(itp1 == itp2);

      auto itm1 = --it;
      auto itm2 = it--;
      RC_ASSERT(itm1 == itm2);
    };

    REQUIRE(rc::check("DereferenceIterator: Agreement of pre-/post- decrement/increment",
                      test));
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Check that ++(--it) and --(++it)) are identical.") {
    auto test = [] {
      typedef std::vector<std::shared_ptr<testtype>> cont_type;
      cont_type v = *gen::container<cont_type>(
                           gen::makeShared<testtype>(gen::arbitrary<testtype>()))
                           .as("Test container");

      RC_PRE(v.size() > 0u);
      size_t pos = *gen::inRange<size_t>(0, v.size()).as("position to offset to");

      DereferenceIterator<typename cont_type::iterator> it(std::begin(v));
      it += pos;

      auto res1 = --(++it);
      RC_ASSERT(res1 == it);

      auto res2 = ++(--it);
      RC_ASSERT(res2 == it);

      RC_ASSERT(*res1 == *res2);
      RC_ASSERT(res1 == res2);
    };

    REQUIRE(
          rc::check("DereferenceIterator: Check that ++(--it) and --(++it)) "
                    "are identical.",
                    test));
  }

  //
  // ---------------------------------------------------------------
  //

  SECTION("Check offset lookup and +n/-n") {
    auto test = [] {
      typedef std::vector<std::shared_ptr<testtype>> cont_type;
      cont_type v = *gen::container<cont_type>(
                           gen::makeShared<testtype>(gen::arbitrary<testtype>()))
                           .as("Test container");

      RC_PRE(v.size() > 0u);
      size_t pos = *gen::inRange<size_t>(0, v.size()).as("position to offset to");

      typedef DereferenceIterator<typename cont_type::iterator> it_type;
      const it_type itorig(std::begin(v));

      it_type it = itorig;
      RC_ASSERT(it == itorig);
      RC_ASSERT(it.operator->() == itorig.operator->());

      it += pos;

      RC_ASSERT(it.operator->() == (itorig + pos).operator->());
      RC_ASSERT(*it == *(itorig + pos));
      RC_ASSERT(it == (itorig + pos));
      RC_ASSERT(*(it - pos) == *itorig);
      RC_ASSERT((it - pos) == itorig);
      RC_ASSERT(*it == itorig[pos]);

      it_type itback = it;
      itback -= pos;
      RC_ASSERT(itback == itorig);
      RC_ASSERT(*itback == *itorig);
      RC_ASSERT((itback + pos) == it);
      RC_ASSERT(*(itback + pos) == *it);
      RC_ASSERT((pos + itback) == it);
      RC_ASSERT(*(pos + itback) == *it);
    };

    REQUIRE(rc::check("DereferenceIterator: Check offset lookup and +n/-n", test));
  }

}  // TEST_CASE
}  // namespace tests
}  // namespace krims
