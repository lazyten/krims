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

#pragma once
#ifdef DEBUG
#include "krims/ExceptionSystem.hh"
#include "krims/demangle.hh"
#include <algorithm>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>
#endif  // DEBUG

namespace krims {
#ifdef DEBUG

// forward declare SubscriptionPointer
template <typename Subscribable>
class SubscriptionPointer;

/** Class which handles subscribtions from a subscription pointer
 * If upon deletion still subscriptions exist, it throws an exception in debug
 * mode
 *
 * \note In RELEASE mode this class is just a dummy marker class,
 *       which contains no functionality whatsoever.
 */
class Subscribable {
  // Declare SubscriptionPointer as friend.
  template <typename T>
  friend class ::krims::SubscriptionPointer;

 public:
  /** A swap function for Subscribables */
  friend void swap(Subscribable&, Subscribable&) {
    // do nothing since the pointers that point to first,
    // still point to the first object and those which
    // point to second object still point to the second.
  }

  //
  // Exception declarations
  //

  /** Exception to indicate that Subscribable is still used */
  DefException3(ExcStillUsed, std::string, size_t, std::string,
                << "Object of type \"" << arg1 << "\" is still used by " << arg2
                << " other objects, which are (from old to new): " << arg3);

  /** Exception to indicate that Subscriber is not known. */
  DefException2(ExcUnknownSubscriberId, std::string, std::string,
                << "No subscriber with identifier \"" << arg1
                << "\" is known to have subscribed to the class " << arg2 << ".");

  //
  // Constructor, destructor and assignment
  //

  /** Check if all subscriptions have been removed
   *
   * @note This function is empty unless we are in DEBUG mode
   */
  virtual ~Subscribable() { assert_no_subscriptions(); }

  /** Default constructor */
  Subscribable() = default;

  /** Default move constructor */
  explicit Subscribable(Subscribable&& other) {
    // check that other has no subscriptions
    // if it does have subscriptions, raise an
    // Exception there:
    other.assert_no_subscriptions();
  }

  /** Copy constructor */
  explicit Subscribable(const Subscribable&) {
    // Since copies are different objects,
    // do not copy the list of subscribers
    // or anything else
  }

  /** Copy assignment operator */
  Subscribable& operator=(const Subscribable&) {
    // All pointers to this object stay intact, so there is no reason
    // to do anything here.
    return *this;
  }

  /** Move assignment operator */
  Subscribable& operator=(Subscribable&&) {
    // All pointers to this object stay intact, so there is no reason
    // to do anything here.
    return *this;
  }

  //
  // Access subscriptions
  //
  /** Return the current number of subscriptions to this object.
   *
   * @note Only defined in DEBUG mode.
   * */
  size_t n_subscriptions() const {
    // Note: Getting the size of a list in C++11 is constant time!
    return m_subscribers.size();
  }

  /** Return the current subscriptions to this object.
   * The list contains a *copy* of the identification strings passed
   * on the subscribe call.
   * The first object is the object which has most recently subscribed
   * and conversely the last object is the oldest object which has still
   * not cancelled its subscription to this Subscribable.
   *
   * @note Only defined in DEBUG  mode.
   */
  std::vector<std::string> subscribers() const {
    std::vector<std::string> v(m_subscribers.size());
    std::transform(std::begin(m_subscribers), std::end(m_subscribers), std::begin(v),
                   [](const std::shared_ptr<const std::string>& p) { return *p; });
    return v;
  }

 protected:
  /** Assert that this has no subscriptions made to it.
   * If this is not the case, than we throw via assert_throw.
   *
   * @note In RELEASE mode this function does not exist.
   */
  void assert_no_subscriptions() const {
    if (m_subscribers.size() > 0) {
      // build the string of subscribing objects
      std::string subscribers;
      for (auto& p : m_subscribers) {
        subscribers.append(" ").append(*p);
      }

      // Raise the exception
      // Note: There is no reason to continue here since we will anyways
      //       have dangling pointers in the SubscriptionPointer classes
      //       after this has occurred. There is no way we can get out of
      //       this gracefully.
      const std::string classname = m_classname.size() == 0 ? "(unknown)" : m_classname;
      assert_throw(false,
                   ExcStillUsed(classname, m_subscribers.size(), std::move(subscribers)));
    }
  }

 private:
  //
  // Deal with subscriptions:
  //

  /** Remove a subscription.
   *
   * @param id Reference to the same string object which was used upon
   *           subscription
   *
   * @note Does only exist in DEBUG mode
   * */
  void unsubscribe(const std::shared_ptr<const std::string>& id_ptr) const {
    for (auto it = std::begin(m_subscribers); it != std::end(m_subscribers); ++it) {
      // check if the pointers agree
      if (it->get() == id_ptr.get()) {
        std::lock_guard<std::mutex> guard(m_mut_subscr);
        m_subscribers.erase(it);
        return;
      }
    }
    const std::string classname = m_classname.size() == 0 ? "(unknown)" : m_classname;
    assert_dbg(false, ExcUnknownSubscriberId(*id_ptr, classname));
  }

  /** Get a subscription
   *
   * @param id The id to print if the subscription is not removed properly
   *           before deletion
   *
   * @note Does only exist in DEBUG mode
   * */
  void subscribe(const std::shared_ptr<const std::string>& id_ptr) const {
    // Make sure only one thread at a time executes this function:
    std::lock_guard<std::mutex> guard(m_mut_subscr);

    // Set classname here, since this is actually executed by the
    // precise object we subscribe to and not the generic Subscribable
    // class. So here we have the "proper" type available in this.
    if (m_classname.size() == 0) {
      m_classname = demangled_string(typeid(*this).name());
    }

    m_subscribers.push_back(id_ptr);
  }

  /** List to contain the pointer of string object, which are passed
   *  on subscription.
   *
   * Marked as mutable in order to allow to subscribe / unsubscribe from
   * const references as well.
   */
  mutable std::vector<std::shared_ptr<const std::string>> m_subscribers{};

  /** Mutex to guard m_subscribers */
  mutable std::mutex m_mut_subscr{};

  /**
   * Name of the actual child Subscribable class
   * Set on call of the subscribe function
   *
   * Marked as mutable in order to allow to subscribe / unsubscribe from
   * const references as well.
   * */
  mutable std::string m_classname{};
};

#else

/** In RELEASE mode the Subscribable class is just a dummy marker class,
 * which contains no functionality whatsoever.
 */
class Subscribable {};

#endif  // DEBUG
}  // namespace krims
