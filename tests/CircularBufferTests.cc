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
#include <krims/CircularBuffer.hh>
#include <list>
#include <rapidcheck.h>
#include <rapidcheck/state.h>

// have an extra verbose output for rapidcheck function tests:
//#define HAVE_CIRCBUFF_RC_CLASSIFY

namespace krims {
namespace tests {
using namespace rc;

namespace circular_buffer_tests {
/* Model for testing CircularBuffer */
template <typename T>
struct CircularBufferModel {
  /** Maximum size of the circular buffer */
  size_t max_size;

  /** List containing the actual data
   * in the actual expected order.
   * Contains size elements */
  std::list<T> data;

  void assert_equivalent_to(const CircularBuffer<T>& b) const {
    RC_ASSERT(b.max_size() == max_size);
    RC_ASSERT(b.size() == data.size());
    RC_ASSERT(b.empty() == data.empty());

    if (b.size() > 0) {
      RC_ASSERT(b.front() == data.front());
      RC_ASSERT(b.back() == data.back());

      auto itdata = std::begin(data);
      for (auto it = b.begin(); it != b.end(); ++it, ++itdata) {
        RC_ASSERT(*itdata == *it);
      }
    }  // if
  }
};

//
// Operations and commands
//

/** Push a random object to the front of the buffer */
template <typename T>
struct PushFront : rc::state::Command<CircularBufferModel<T>, CircularBuffer<T>> {
  typedef CircularBufferModel<T> model_type;
  typedef CircularBuffer<T> sut_type;

  T t;  //< New object to push_front;
  PushFront() : t{*gen::arbitrary<T>()} {};

  void apply(model_type& model) const override {
    RC_PRE(model.max_size > 0u);
    model.data.push_front(t);

    if (model.data.size() > model.max_size) {
      model.data.resize(model.max_size);
    }
  }

  void run(const model_type& model, sut_type& sut) const override {
#ifdef HAVE_CIRCBUFF_RC_CLASSIFY
    RC_CLASSIFY(true, "PushFront");
#endif

    // Perform on sut
    sut.push_front(t);

    // Check with model:
    auto nmodel = this->nextState(model);
    nmodel.assert_equivalent_to(sut);
  }

  void show(std::ostream& os) const override { os << "PushFront (" << t << ")"; }
};  // PushFront

/** Push a random object to the back of the buffer */
template <typename T>
struct PushBack : rc::state::Command<CircularBufferModel<T>, CircularBuffer<T>> {
  typedef CircularBufferModel<T> model_type;
  typedef CircularBuffer<T> sut_type;

  T t;  //< New object to push_back;
  PushBack() : t{*gen::arbitrary<T>()} {};

  void apply(model_type& model) const override {
    RC_PRE(model.max_size > 0u);
    model.data.push_back(t);

    while (model.data.size() > model.max_size) {
      model.data.pop_front();
    }
  }

  void run(const model_type& model, sut_type& sut) const override {
#ifdef HAVE_CIRCBUFF_RC_CLASSIFY
    RC_CLASSIFY(true, "PushBack");
#endif

    // Perform on sut
    sut.push_back(t);

    // Check with model:
    auto nmodel = this->nextState(model);
    nmodel.assert_equivalent_to(sut);
  }

  void show(std::ostream& os) const override { os << "PushBack (" << t << ")"; }
};  // PushBack

/** Clear the buffer */
template <typename T>
struct Clear : rc::state::Command<CircularBufferModel<T>, CircularBuffer<T>> {
  typedef CircularBufferModel<T> model_type;
  typedef CircularBuffer<T> sut_type;

  void apply(model_type& model) const override {
    RC_PRE(model.max_size > 0u);
    model.data.clear();
  }

  void run(const model_type& model, sut_type& sut) const override {
#ifdef HAVE_CIRCBUFF_RC_CLASSIFY
    RC_CLASSIFY(true, "Clear");
#endif

    // Perform on sut
    sut.clear();

    // Check with model:
    this->nextState(model).assert_equivalent_to(sut);
  }

  void show(std::ostream& os) const override { os << "Clear"; }
};  // Clear

/** Change max_size */
template <typename T>
struct ChangeMaxSize : rc::state::Command<CircularBufferModel<T>, CircularBuffer<T>> {
  typedef CircularBufferModel<T> model_type;
  typedef CircularBuffer<T> sut_type;

  size_t max;  //< New max_size
  ChangeMaxSize() : max{*gen::inRange<size_t>(0, 11)} {}

  void apply(model_type& model) const override {
    model.max_size = max;
    if (model.data.size() > max) {
      model.data.resize(max);
    }
  }

  void run(const model_type& model, sut_type& sut) const override {
#ifdef HAVE_CIRCBUFF_RC_CLASSIFY
    RC_CLASSIFY(true, "ChangeMaxSize" + std::to_string(max));
#endif

    // Perform on sut
    sut.max_size(max);

    // Check with model:
    this->nextState(model).assert_equivalent_to(sut);
  }

  void show(std::ostream& os) const override { os << "ChangeMaxSize (" << max << ")"; }
};  // ChangeMaxSize

/** execute a random test of a list of the above commands
 *
 * \tparam T typename of the circular buffer
 * \tparam Commands The command list
 */
template <typename T, typename... Commands>
void exectute_random_test() {
  size_t max_size = *rc::gen::inRange<size_t>(2, 10).as("max_size");
  std::list<T> data{};

  // Setup the model and initial system state
  typedef CircularBufferModel<T> model_type;
  typedef CircularBuffer<T> sut_type;
  model_type model{};
  model.max_size = max_size;
  model.data     = std::move(data);
  sut_type sut{max_size};

  // Generate command sequence and execute random state test
  auto gen_commands = rc::state::gen::execOneOfWithArgs<Commands...>;
  state::check(model, sut, gen_commands());
}

}  // namespace circular_buffer_tests

//
// ---------------------------------------------------------------
//

TEST_CASE("Circular Buffer", "[circular_buffer]") {
  using namespace circular_buffer_tests;

  // The type to do the tests with
  typedef int test_type;

  SECTION("Pushing elements into circular buffer.") {
    auto test = [](std::vector<test_type> v) {
      RC_PRE(v.size() > 0u);

      CircularBuffer<test_type> buf{v.size() + 5};
      CircularBuffer<test_type> buf2{v.size() + 5};
      for (auto elem : v) {
        buf.push_back(elem);
        buf2.push_front(elem);
      }

      // Access with iterators
      size_t i = 0;
      for (auto it = std::begin(buf); it != std::end(buf); ++it, ++i) {
        RC_ASSERT(*it == v[i]);
      }
      RC_ASSERT(i == v.size());

      // Access with const iterators
      i = 0;
      for (auto it = buf.cbegin(); it != buf.cend(); ++it, ++i) {
        RC_ASSERT(*it == v[i]);
      }
      RC_ASSERT(i == v.size());

      // Compare buffers generated from push_front and push_back
      auto it2 = std::begin(buf2);
      std::advance(it2, v.size() - 1);
      for (auto it1 = std::begin(buf); it1 != std::end(buf); ++it1, --it2) {
        RC_ASSERT(*it1 == *it2);
      }
    };

    REQUIRE(rc::check("Pushing elements into circular buffer", test));
  }

  //
  // ------------------------------------------------------------
  //

  SECTION("Random function test of circular buffer") {
    // Typedef the operations
    typedef PushBack<test_type> op_PushBack;
    typedef PushFront<test_type> op_PushFront;
    typedef Clear<test_type> op_Clear;
    typedef ChangeMaxSize<test_type> op_ChangeMaxSize;

    REQUIRE(rc::check(
          "Random function test of circular buffer with Clear",
          exectute_random_test<test_type, op_PushBack, op_PushFront, op_Clear>));
    REQUIRE(rc::check(
          "Random function test of circular buffer with ChangeMaxSize",
          exectute_random_test<test_type, op_PushBack, op_PushFront, op_ChangeMaxSize>));
  }  // Random function test

}  // TEST_CASE
}  // namespace tests
}  // namespace krims
