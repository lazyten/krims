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

#pragma once
#include "SubscriptionPointer.hh"
#include "exceptions.hh"
#include <map>
#include <memory>
#include <string>

namespace krims {

/** ParameterMap is essentially a map from std::string to
 *  a shared pointer of void. This way an arbitrary amount of arbitrary
 *  objects can be passed around using this object and they can be quickly
 *  accessed by the std::string key.
 */
class ParameterMap {

  // TODO: Use default argument in get functions (if key is not defined)
  //       integral and basic types should be supplied by value.

  // TODO use a dual pointer class that contains either a subscription pointer
  // or a shared pointer. It is default constructed from both a subscription
  // pointer or a shared pointer.
  // Further it can be converted to the aprorpiate pointertype explicitly
  // and provides check functions to see which pointer it contains.

public:
  //
  // Exception declaration:
  //
  /** Exception to indicate that a wrong type was requested */
  DefException3(ExcWrongTypeRequested, std::string, std::string, std::string,
                << "Requested type " << arg1 << " from ParameterMap using key "
                << arg2 << ". The value however has type " << arg3 << ".");

  /** Exception thrown if a key is not valid */
  DefException1(ExcUnknownKey, std::string, << "The key " << arg1
                                            << " is unknown.");

  DefExceptionMsg(ExcWrongPointerRequested,
                  "Cannot return shared pointer if object was stored using a "
                  "SubscriptionPointer.");

private:
  //
  // The ExtractValueFromPointer class
  //
  /** Helper class to make sure that no compiler error gets produced
   * when T is not a Subscribable
   */
  template <typename T,
            typename = typename std::is_base_of<Subscribable, T>::type>
  struct DereferencePointerSubscriptionPointer {
    typedef T& result_type;
    typedef const std::shared_ptr<void> argument_type;

    T& operator()(std::shared_ptr<void> p) const;
  };

  /** Specialisation for objects which are not subscribable
   *
   * Does nothing.
   * */
  template <typename T>
  struct DereferencePointerSubscriptionPointer<T, std::false_type> {
    typedef T& result_type;
    typedef const std::shared_ptr<void> argument_type;

    T& operator()(std::shared_ptr<void>);

  private:
    /** A dummy value to return */
    static constexpr T* m_ptr = nullptr;
  };

  //
  // The entry class
  //
  class Entry {
  public:
    /** Default constructor: Construct empty object */
    Entry();

    template <typename T>
    explicit Entry(std::shared_ptr<T> ptr);

    template <typename T>
    explicit Entry(SubscriptionPointer<T> ptr);

    template <typename T>
    T& get(const std::string& key);

    template <typename T>
    const T& get(const std::string& key) const;

    template <typename T>
    std::shared_ptr<T> get_ptr(const std::string& key);

    template <typename T>
    std::shared_ptr<const T> get_ptr(const std::string& key) const;

  private:
    // The stored pointer
    std::shared_ptr<void> m_object_ptr;

    // Is the object_ptr a pointer to the object
    // or a pointer to a subscription_pointer
    bool m_via_subscription_ptr;
#ifdef DEBUG
    std::string m_type_name;
#endif
  };

public:
  /** Insert or update using a shared pointer */
  template <typename T>
  void update(std::string key, std::shared_ptr<T> object_ptr);

  /** Insert or update using a SubscriptionPointer */
  template <typename T>
  void update(std::string key, SubscriptionPointer<T> object_ptr);

  // TODO have simple overload for primitive types (int, double, ...
  //      which calls update_copy automatically.
  //
  //      Have simple overload for Subscriabables which makes a subscription
  //      automatically

  /** Insert or update a key with a copy of an element */
  template <typename T>
  void update_copy(std::string key, T object);

  /** Remove an element */
  void erase(const std::string& key);

  /** Check weather a key exists */
  bool exists(const std::string& key) const;

  /** TODO: More functions: Clear, operator[], size, iterator */

  /** Get the value of an element
   */
  template <typename T>
  T& at(const std::string& key);

  /** Return the value at a given key in a specific type
   */
  template <typename T>
  const T& at(const std::string& key) const;

  /** Return the pointer to the value of a specific key.
   *
   * This only works if the data has been supplied to the
   * parameter map via a shared pointer as well or has
   * been supplied by update_copy().
   */
  template <typename T>
  std::shared_ptr<T> at_ptr(const std::string& key);

  /** Return the pointer to the value of a specific key.
   *
   * This only works if the data has been supplied to the
   * parameter map via a shared pointer as well or has
   * been supplied by update_copy().
   */
  template <typename T>
  std::shared_ptr<const T> at_ptr(const std::string& key) const;

private:
  // TODO use unordered_map !!! (amortised constant []
  std::map<std::string, Entry> m_container;
};

//
// -----------------------------------------------------------------
//

//
// FullDereference
//
template <typename T, typename B>
T& ParameterMap::DereferencePointerSubscriptionPointer<T, B>::operator()(
      std::shared_ptr<void> p) const {
  // Extract pointer to subscription pointer
  auto ptr_ptr = std::static_pointer_cast<SubscriptionPointer<T>>(p);

  // return the value by twice dereferencing:
  return *(*ptr_ptr);
}

template <typename T>
T& ParameterMap::DereferencePointerSubscriptionPointer<T, std::false_type>::
operator()(std::shared_ptr<void>) {
  assert_dbg(false, exceptions::ExcNotImplemented());
  return *m_ptr;
}

//
// Entry subclass
//
template <typename T>
ParameterMap::Entry::Entry(std::shared_ptr<T> ptr)
      : m_object_ptr(ptr), m_via_subscription_ptr(false) {
#ifdef DEBUG
  m_type_name = typeid(T).name();
#endif
}

template <typename T>
ParameterMap::Entry::Entry(SubscriptionPointer<T> ptr)
      : m_object_ptr(std::make_shared<SubscriptionPointer<T>>(ptr)),
        m_via_subscription_ptr(true) {
#ifdef DEBUG
  m_type_name = typeid(T).name();
#endif
}

template <typename T>
T& ParameterMap::Entry::get(const std::string& key) {
  // check that the correct type is requested:
  assert_dbg(m_type_name == typeid(T).name(),
             ExcWrongTypeRequested(typeid(T).name(), key, m_type_name));

  if (m_via_subscription_ptr) {
    DereferencePointerSubscriptionPointer<T> dereference;
    return dereference(m_object_ptr);
  } else {
    // Extract pointer to type and dereference it
    return *std::static_pointer_cast<T>(m_object_ptr);
  }
}

template <typename T>
const T& ParameterMap::Entry::get(const std::string& key) const {
  // check that the correct type is requested:
  assert_dbg(m_type_name == typeid(T).name(),
             ExcWrongTypeRequested(typeid(T).name(), key, m_type_name));

  if (m_via_subscription_ptr) {
    DereferencePointerSubscriptionPointer<T> dereference;
    return dereference(m_object_ptr);
  } else {
    // Extract pointer to type and dereference it
    return *std::static_pointer_cast<T>(m_object_ptr);
  }
}

template <typename T>
std::shared_ptr<T> ParameterMap::Entry::get_ptr(const std::string& key) {
  // check that the correct type is requested:
  assert_dbg(m_type_name == typeid(T).name(),
             ExcWrongTypeRequested(typeid(T).name(), key, m_type_name));

  assert_dbg(!m_via_subscription_ptr, ExcWrongPointerRequested());
  return std::static_pointer_cast<T>(m_object_ptr);
}

template <typename T>
std::shared_ptr<const T> ParameterMap::Entry::get_ptr(
      const std::string& key) const {
  // check that the correct type is requested:
  assert_dbg(m_type_name == typeid(T).name(),
             ExcWrongTypeRequested(typeid(T).name(), key, m_type_name));

  assert_dbg(!m_via_subscription_ptr, ExcWrongPointerRequested());
  return std::static_pointer_cast<const T>(m_object_ptr);
}

//
// ParameterMap
//
template <typename T>
void ParameterMap::update(std::string key, std::shared_ptr<T> object_ptr) {
  // Insert or update a new element:
  m_container[key] = Entry{object_ptr};
}

/** Insert or update using a SubscriptionPointer */
template <typename T>
void ParameterMap::update(std::string key, SubscriptionPointer<T> object_ptr) {
  // Insert or update a new element:
  m_container[key] = Entry{object_ptr};
}

/** Insert or update a key with a copy of an element */
template <typename T>
void ParameterMap::update_copy(std::string key, T object) {
  m_container[key] = Entry{std::make_shared<T>(object)};
}

/** Get the value of an element
 */
template <typename T>
T& ParameterMap::at(const std::string& key) {
  assert_dbg(exists(key), ExcUnknownKey(key));
  return m_container.at(key).get<T>(key);
}

/** Return the value at a given key in a specific type
 */
template <typename T>
const T& ParameterMap::at(const std::string& key) const {
  assert_dbg(exists(key), ExcUnknownKey(key));
  return m_container.at(key).get<T>(key);
}

template <typename T>
std::shared_ptr<T> ParameterMap::at_ptr(const std::string& key) {
  assert_dbg(exists(key), ExcUnknownKey(key));
  return m_container.at(key).get_ptr<T>(key);
}

template <typename T>
std::shared_ptr<const T> ParameterMap::at_ptr(const std::string& key) const {
  return m_container.at(key).get_ptr<T>(key);
}

// TODO at_with_default function

}  // krims