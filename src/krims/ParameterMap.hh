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
#include "ExceptionSystem.hh"
#include "PointerWrapper.hh"
#include "SubscriptionPointer.hh"
#include "TypeUtils.hh"
#include <map>
#include <memory>
#include <string>

namespace krims {

/** ParameterMap is essentially a map from std::string to objects of
 *  arbitrary type.
 *
 *  This way an arbitrary amount of arbitrary objects can be passed
 *  around using this object and they can be quickly accessed by the
 *  std::string key.
 */
class ParameterMap {
public:
  class EntryValue;

public:
  typedef std::map<std::string, EntryValue> inner_map_type;
  typedef std::pair<const std::string, EntryValue> entry_type;

  //
  // Exception declaration:
  //
  /** Exception to indicate that a wrong type was requested */
  DefException2(ExcWrongTypeRequested, std::string, std::string,
                << "Requested invalid type " << arg1 << " from ParameterMap."
                << " The value has type " << arg2 << ".");

  /** Exception thrown if a key is not valid */
  DefException1(ExcUnknownKey, std::string, << "The key " << arg1
                                            << " is unknown.");

  //
  // The entry class
  //
  /** \brief Class to contain an entry in a parameter map, i.e. the thing the
   *  key string actually points to.
   *
   * Can be constructed from elementary types by copying their value or from
   * a std::shared_ptr or a SubscriptionPointer to the object we emplace in the
   * map or from an object which is subscribable (which will automatically be
   * subscribed to.
   */
  class EntryValue {
  public:
    /** \brief Default constructor: Constructs empty object */
    EntryValue() : m_object_ptr_ptr{nullptr} {}

    /** \brief Make a EntryValue out of a type which is cheap to copy.
     *
     * This includes std::string and all relevant numeric types (integers,
     * floating point numbers, complex numbers)
     **/
    template <typename T, typename std::enable_if<IsCheaplyCopyable<T>::value,
                                                  int>::type = 0>
    EntryValue(T t) {
      copy_in<T>(t);
    }

    /** \brief Make an EntryValue out of a const char*.
     *
     * This behaves like the equivalent EntryValue of a  std::string */
    EntryValue(const char* s) : EntryValue(std::string(s)) {}

    /** \brief Make an EntryValue from a shared pointer */
    template <typename T>
    EntryValue(std::shared_ptr<T> t_ptr);

    /** Make an EntryValue from a Subscribable object */
    template <typename T,
              typename std::enable_if<std::is_base_of<Subscribable, T>::value,
                                      int>::type = 0>
    EntryValue(T& t);

    /** Obtain a non-const pointer to the internal object */
    template <typename T>
    PointerWrapper<T> get_ptr();

    /** Obtain a const pointer to the internal object */
    template <typename T>
    PointerWrapper<const T> get_ptr() const;

    /** Obtain a reference to the internal object */
    template <typename T>
    T& get() {
      return *get_ptr<T>();
    }

    /** Obtain a const reference to the internal object */
    template <typename T>
    const T& get() const {
      return *get_ptr<T>();
    }

    /** Is the object empty */
    bool empty() const { return m_object_ptr_ptr == nullptr; }

  private:
    //! Stupidly copy the object and set the m_object_ptr_ptr
    template <typename T>
    void copy_in(T t);

    /** The stored pointer to the Pointerwrapper<T>
     * In other words: Twice dereferencing this will always
     * get us the object back.
     */
    std::shared_ptr<void> m_object_ptr_ptr;

#ifdef DEBUG
    std::string m_type_name;
#endif
  };

  /** \name Constructors */
  ///@{
  /** \brief default constructor
   * Constructs empty map */
  ParameterMap() : m_container{} {}

  /** \brief Construct parameter map from initialiser list of entry_types */
  ParameterMap(std::initializer_list<entry_type> il) : m_container(il) {}
  ///@}

  /** \name Modifiers */
  ///@{
  /** \brief Insert or update using a shared pointer */
  template <typename T>
  void update(const std::string& key, std::shared_ptr<T> object_ptr) {
    m_container[key] = EntryValue{std::move(object_ptr)};
  }

  /** Insert or update using a Subscribable object */
  template <typename T,
            typename std::enable_if<std::is_base_of<Subscribable, T>::value,
                                    int>::type = 0>
  void update(const std::string& key, T& t) {
    m_container[key] = EntryValue{t};
  }

  /** Insert or update using an object which can be cheaply copied */
  template <typename T,
            typename std::enable_if<IsCheaplyCopyable<T>::value, int>::type = 0>
  void update(const std::string& key, T t) {
    m_container[key] = EntryValue{std::move(t)};
  }

  /** Insert or update using a const char*, which behaves as if it was a string
   */
  void update(const std::string& key, const char* s) {
    m_container[key] = EntryValue{s};
  }

  /** Insert or update a key with a copy of an element */
  template <typename T>
  void update_copy(std::string key, T object) {
    m_container[key] = EntryValue{std::make_shared<T>(object)};
  }

  /** \brief Try to remove an element.
   *
   * Return the number of removed elements (i.e. 0 or 1)*/
  size_t erase(const std::string& key) { return m_container.erase(key); }

  void clear() noexcept { m_container.clear(); }
  ///@}

  /** Return the value at a given key in a specific type
   *
   * If the value cannot be found an ExcUnknownKey is thrown.
   * If the type requested is wrong the program is aborted.
   */
  template <typename T>
  T& at(const std::string& key);

  /** \brief Return the value at a given key in a specific type (const version)
   * See non-const version for details.
   */
  template <typename T>
  const T& at(const std::string& key) const;

  /** \brief Get the value of an element.
   *
   * If the key cannot be found, returns the reference provided instead.
   * If the type requested is wrong the program is aborted.
   */
  template <typename T>
  T& at(const std::string& key, T& default_value);

  /** \brief Get the value of an element (const version).
   * See non-const version for details.
   */
  template <typename T>
  const T& at(const std::string& key, const T& default_value) const;

  /** \brief Return the pointer to the value of a specific key.
   *
   * The returned object is a PointerWrapper, meaning that it internally
   * may contain a shared pointer, but it may also contain a
   * SubscriptionPointer. It can be used as if it was a normal pointer,
   * but in order to extract the actual shared_ptr out of it an explicit
   * cast to the appropriate shared_ptr type is required.
   *
   * In case this is not possible (i.e. it does contain a
   * SubscriptionPointer instead) an exception is raised in Debug mode.
   * Use the contains_shared_ptr() function to check this.
   */
  template <typename T>
  PointerWrapper<T> at_ptr(const std::string& key);

  /** Return the pointer to the value of a specific key. (const version)
   *
   * See non-const version for details
   */
  template <typename T>
  PointerWrapper<const T> at_ptr(const std::string& key) const;

  // TODO find, iterators

  /** Check weather a key exists */
  bool exists(const std::string& key) const {
    return m_container.find(key) != std::end(m_container);
  }

private:
  inner_map_type m_container;
};

//
// -----------------------------------------------------------------
//

//
// EntryValue subclass
//
template <typename T>
ParameterMap::EntryValue::EntryValue(std::shared_ptr<T> t_ptr) {
  // see copy_in and m_object_ptr_ptr comments for defails why this is done
  m_object_ptr_ptr = std::make_shared<PointerWrapper<T>>(std::move(t_ptr));
#ifdef DEBUG
  m_type_name = std::string(typeid(T).name());
#endif
}

template <typename T, typename std::enable_if<
                            std::is_base_of<Subscribable, T>::value, int>::type>
ParameterMap::EntryValue::EntryValue(T& t) {
  SubscriptionPointer<T> t_ptr = make_subscription(t, "EntryValue");

  // see copy_in and m_object_ptr_ptr comments for defails why this is done
  m_object_ptr_ptr = std::make_shared<PointerWrapper<T>>(std::move(t_ptr));

#ifdef DEBUG
  m_type_name = std::string(typeid(T).name());
#endif
}

template <typename T>
void ParameterMap::EntryValue::copy_in(T t) {
  // Make a shared pointer out of T:
  auto t_ptr = std::make_shared<T>(std::move(t));

  // Now enwrap the t_ptr inside a PointerWrapper and make
  // a shared_ptr to the Wrapper, then set it
  m_object_ptr_ptr = std::make_shared<PointerWrapper<T>>(t_ptr);

#ifdef DEBUG
  // Keep an eye on the type name
  m_type_name = std::string(typeid(T).name());
#endif
}

template <typename T>
PointerWrapper<T> ParameterMap::EntryValue::get_ptr() {
  assert_dbg(m_type_name == std::string(typeid(T).name()),
             ExcWrongTypeRequested(std::string(typeid(T).name()), m_type_name));
  assert_dbg(!empty(), ExcInvalidPointer());

  // We need to cast and then dereference to get the PointerWrapper of the
  // appropriate type out.
  return *std::static_pointer_cast<PointerWrapper<T>>(m_object_ptr_ptr);
}

template <typename T>
PointerWrapper<const T> ParameterMap::EntryValue::get_ptr() const {
  assert_dbg(m_type_name == std::string(typeid(T).name()),
             ExcWrongTypeRequested(std::string(typeid(T).name()), m_type_name));
  assert_dbg(!empty(), ExcInvalidPointer());

  // We need to cast and then dereference to get the PointerWrapper of the
  // appropriate type out.
  return *std::static_pointer_cast<PointerWrapper<const T>>(m_object_ptr_ptr);
}

//
// ParameterMap
//
template <typename T>
T& ParameterMap::at(const std::string& key) {
  assert_dbg(exists(key), ExcUnknownKey(key));
  EntryValue& e = m_container.at(key);
  return e.get<T>();
}

template <typename T>
const T& ParameterMap::at(const std::string& key) const {
  assert_dbg(exists(key), ExcUnknownKey(key));
  const EntryValue& e = m_container.at(key);
  return e.get<T>();
}

template <typename T>
T& ParameterMap::at(const std::string& key, T& default_value) {
  auto itkey = m_container.find(key);
  if (itkey == std::end(m_container)) {
    // Key not found, return default:
    return default_value;
  } else {
    // Key found, return mapped value
    return itkey->second;
  }
}

template <typename T>
const T& ParameterMap::at(const std::string& key,
                          const T& default_value) const {
  auto itkey = m_container.find(key);
  if (itkey == std::end(m_container)) {
    // Key not found, return default:
    return default_value;
  } else {
    // Key found, return mapped value
    return itkey->second.get<T>();
  }
}

template <typename T>
PointerWrapper<T> ParameterMap::at_ptr(const std::string& key) {
  assert_dbg(exists(key), ExcUnknownKey(key));
  EntryValue& e = m_container.at(key);
  return e.get_ptr<T>();
}

template <typename T>
PointerWrapper<const T> ParameterMap::at_ptr(const std::string& key) const {
  assert_dbg(exists(key), ExcUnknownKey(key));
  const EntryValue& e = m_container.at(key);
  return e.get_ptr<T>();
}

}  // krims
