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
#include "ExceptionSystem.hh"
#include "RCPWrapper.hh"
#include "SubscriptionPointer.hh"
#include "TypeUtils.hh"
#include "demangle.hh"
#include <iterator>
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
 *
 *  TODO: Documentation how "/" are special and go into submaps down the tree
 */
class ParameterMap : public Subscribable {
 public:
  class EntryValue;
  class KeyIterator;

  // TODO: When modifying an entry using update, make sure that the type
  //       stays unchanged

 public:
  typedef std::map<std::string, EntryValue> inner_map_type;
  typedef std::pair<const std::string, EntryValue> entry_type;

  //
  // Exception declaration:
  //
  /** Exception to indicate that a wrong type was requested */
  DefException2(ExcWrongTypeRequested, std::string, std::string,
                << "Requested invalid type '" << arg1 << "' from ParameterMap."
                << " The value has type '" << arg2 << "'.");

  /** Exception thrown if a key is not valid */
  DefException1(ExcUnknownKey, std::string, << "The key " << arg1 << " is unknown.");

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
    template <typename T, typename std::enable_if<!std::is_reference<T>::value &&
                                                        IsCheaplyCopyable<T>::value,
                                                  int>::type = 0>
    EntryValue(T t) {
      // Note about the enable_if:
      //   - We need to make sure that T is the actual type (and not a
      //     reference)
      //   - T should be cheap to copy
      copy_in<T>(t);
    }

    /** \brief Make an EntryValue out of a const char*.
     *
     * This behaves like the equivalent EntryValue of a  std::string */
    EntryValue(const char* s) : EntryValue(std::string(s)) {}

    /** \brief Make an EntryValue from a shared pointer */
    template <typename T, typename = krims::enable_if_t<
                                !std::is_same<ParameterMap, decay_t<T>>::value>>
    EntryValue(std::shared_ptr<T> t_ptr);

    /** \brief Make an EntryValue from an RCPWrapper */
    template <typename T, typename = krims::enable_if_t<
                                !std::is_same<ParameterMap, decay_t<T>>::value>>
    EntryValue(RCPWrapper<T> t_ptr);

    /** Make an EntryValue from a Subscribable object (which is not a ParameterMap) */
    template <typename T, typename std::enable_if<
                                std::is_base_of<Subscribable, T>::value &&
                                      !IsCheaplyCopyable<T>::value &&
                                      !std::is_same<ParameterMap, decay_t<T>>::value,
                                int>::type = 0>
    EntryValue(T& t);

    /** Make an EntryValue from an rvalue reference */
    template <typename T,
              typename = typename std::enable_if<
                    !std::is_reference<T>::value && !IsCheaplyCopyable<T>::value &&
                    !std::is_same<ParameterMap, decay_t<T>>::value>::type>
    EntryValue(T&& t) : EntryValue{std::make_shared<T>(std::move(t))} {}
    // Note about the enable_if:
    //   - We need to make sure that T is the actual type (and not a
    //     reference)
    //   - T should not be cheap to copy (else first constructor applies)
    //   - T should not be a ParameterMap (we do not want maps in maps)

    EntryValue(const EntryValue&) = default;
    EntryValue(EntryValue&&) = default;
    ~EntryValue() = default;
    EntryValue& operator=(const EntryValue&) = default;
    EntryValue& operator=(EntryValue&&) = default;

    /** Obtain a non-const pointer to the internal object */
    template <typename T>
    RCPWrapper<T> get_ptr();

    /** Obtain a const pointer to the internal object */
    template <typename T>
    RCPWrapper<const T> get_ptr() const;

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

    /** The stored pointer to the RCPWrapper<T>
     * In other words: Twice dereferencing this will always
     * get us the object back.
     */
    std::shared_ptr<void> m_object_ptr_ptr;

#ifdef DEBUG
    std::string m_type_name;
#endif
  };

  class KeyIterator
        : public std::iterator<std::bidirectional_iterator_tag, const std::string> {
    // Make parent class a friend (such that we can get the internal raw map iterator)
    friend ParameterMap;

   public:
    typedef inner_map_type::const_iterator iter_type;

    /** Dereference key iterator */
    const std::string& operator*() const {
      key_cache = strip_location_prefix(m_iter->first);
      return key_cache;
    }

    /** Obtain pointer to key iterator */
    const std::string* operator->() const { return &operator*(); }

    /** Prefix increment to the next key */
    KeyIterator& operator++() {
      ++m_iter;
      return *this;
    }

    /** Postfix increment to the next key */
    KeyIterator operator++(int) {
      KeyIterator copy(*this);
      this->operator++();
      return copy;
    }

    /** Prefix decrement to the next key */
    KeyIterator& operator--() {
      --m_iter;
      return *this;
    }

    /** Postfix decrement to the next key */
    KeyIterator operator--(int) {
      KeyIterator copy(*this);
      this->operator--();
      return copy;
    }

    bool operator==(const KeyIterator& other) { return m_iter == other.m_iter; }
    bool operator!=(const KeyIterator& other) { return m_iter != other.m_iter; }

    KeyIterator(iter_type iter, const std::string& location)
          : m_iter(iter), m_location(location) {}

   private:
    /** Undo the operation of make_full_key, i.e. strip off the
     * first location part and get a relative path to it*/
    std::string strip_location_prefix(const std::string& key) const;

    iter_type m_iter;               //< Iterator to the current key,value pair
    std::string m_location;         //< Subtree location we iterate over
    mutable std::string key_cache;  //< Cache for the current truncated key string
  };

  /** \name Constructors, destructors and assignment */
  ///@{
  /** \brief default constructor
   * Constructs empty map */
  ParameterMap() : m_container_ptr{std::make_shared<inner_map_type>()}, m_location{""} {}

  /** \brief Construct parameter map from initialiser list of entry_types */
  ParameterMap(std::initializer_list<entry_type> il) : ParameterMap{} { update(il); };

  ~ParameterMap() = default;
  ParameterMap(ParameterMap&&) = default;

  /** \brief Copy constructor */
  ParameterMap(const ParameterMap& other);

  /** \brief Assignment operator */
  ParameterMap& operator=(ParameterMap other);
  ///@}

  /** \name Modifiers */
  ///@{
  /** \brief Insert or update a key.
   *
   * All objects which can be implicitly converted to an EntryValue can be used.
   * This includes:
   *   - Cheaply copyable types (numbers, std::string, enums)
   *   - RValue references
   *   - LValue references of subscribable objects
   *   - Shared pointers
   */
  void update(const std::string& key, EntryValue e) {
    (*m_container_ptr)[make_full_key(key)] = std::move(e);
  }

  /** \brief Update many entries using an initialiser list
   *
   * TODO More details, have an example
   * */
  void update(std::initializer_list<entry_type> il);

  /** \brief Update many entries using another ParameterMap
   *
   * The entries are updated relative to the given key paths.
   * I.e. if key == "blubber" and the map \t map contairs "foo" and
   * "bar", then "blubber/foo" and "blubber/bar" will be updated.
   * */
  void update(const std::string& key, const ParameterMap& map);

  /** \brief Update many entries using another ParameterMap
   *
   * The entries are updated relative to the given key paths.
   * I.e. if key == "blubber" and the map \t map contairs "foo" and
   * "bar", then "blubber/foo" and "blubber/bar" will be updated.
   * */
  void update(const std::string& key, ParameterMap&& map);

  /** \brief Update many entries using another ParameterMap
   *
   * The entries are updated in paths relative to /
   * */
  void update(const ParameterMap& other) { update("/", other); }

  /** \brief Update many entries using another ParameterMap
   *
   * The entries are updated in paths relative to /
   */
  void update(ParameterMap&& other) { update("/", std::move(other)); }

  /** Insert or update a key with a copy of an element */
  template <typename T>
  void update_copy(std::string key, T object) {
    (*m_container_ptr)[make_full_key(key)] = EntryValue{std::make_shared<T>(object)};
  }

  /** Insert a default value for a key, i.e. no existing key will be touched,
   * only new ones inserted (That's why the method is still const)
   */
  void insert_default(const std::string& key, EntryValue e) const {
    auto itkey = m_container_ptr->find(make_full_key(key));
    if (itkey == std::end(*m_container_ptr)) {
      // Key not found, hence insert default.
      (*m_container_ptr)[make_full_key(key)] = std::move(e);
    }
  }

  /** Insert default values for many entries at once using an initialiser list.
   *
   * Only inserts values that do not already exist in the map
   * (That's why the method is const)
   */
  void insert_default(std::initializer_list<entry_type> il) const {
    for (entry_type t : il) {
      insert_default(t.first, t.second);
    }
  }

  //@{
  /** \brief Try to remove an element
   *  which is referenced by this string
   *
   *  \return The number of removed elements (i.e. 0 or 1)
   **/
  size_t erase(const std::string& key) {
    return m_container_ptr->erase(make_full_key(key));
  }

  /** \brief Try to remove an element referenced by a key iterator
   *
   *  \return The iterator referencing the key *after* the last
   *          element removed
   **/
  KeyIterator erase(KeyIterator position) {
    auto res = m_container_ptr->erase(position.m_iter);
    return KeyIterator(std::move(res), m_location);
  }

  /** \brief Try to remove a range of elements
   *
   *  \return The iterator referencing the key *after* the last
   *          element removed
   **/
  KeyIterator erase(KeyIterator first, KeyIterator last) {
    auto res = m_container_ptr->erase(first.m_iter, last.m_iter);
    return KeyIterator(std::move(res), m_location);
  }

  /** \brief Try to remove a full submap path including all
   *         child key entries.
   *
   *  \note  The function is equivalent to ``this->submap(path).clear()``.
   *  \return The number of key-value entries removed from the map
   */
  void erase_recursive(const std::string& path) {
    erase(begin_keys(path), end_keys(path));
  }
  //@}

  /** Remove all elements from the map
   *
   * \note This takes the location of submaps into account,
   * i.e. only the elements of the submap are deleted and
   * not all elements of the parent. */
  void clear();
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
   * The returned object is a RCPWrapper, meaning that it internally
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
  RCPWrapper<T> at_ptr(const std::string& key);

  /** Return the pointer to the value of a specific key. (const version)
   *
   * See non-const version for details
   */
  template <typename T>
  RCPWrapper<const T> at_ptr(const std::string& key) const;

  /** Check weather a key exists */
  bool exists(const std::string& key) const {
    return m_container_ptr->find(make_full_key(key)) != std::end(*m_container_ptr);
  }

  /** \brief Get a submap starting pointing at a different location.
   *
   * The ParameterMap allows the hierarchical organisation of data
   * in the form of UNIX paths. So the key "bla/blubber/foo" actually
   * emplaces a value in the path "/bla/blubber/foo". If one obtains
   * a submap at location "/bla", the same key is now available under
   * the path "/blubber/foo", whereas "/bla/blubber/foo" no longer exists.
   *
   * All supplied paths are subject to the standard UNIX path normalisation
   * in other words "/bla/../bla/blubber/./foo" is equivalent to
   * "/bla/blubber/foo". The leading "/" may be avoided. Hence "bla/blubber/foo"
   * is in turn equal to the aforementioned paths and yields the same
   * entry.
   *
   * Escaping a submap via ".." is not possible. This means that the submap
   * at location "bla" does still not contain a key "../bla/blubber/foo",
   * but it does contain a key "../blubber/foo", since the leading ".."
   * has no effect (we are at the root of the ParameterMap)
   * */
  ParameterMap submap(const std::string& location) {
    // Construct new map, but starting at a different location
    return ParameterMap{m_container_ptr, make_full_key(location)};
  }

  /** Get a const submap */
  const ParameterMap submap(const std::string& location) const {
    // Construct new map, but starting at a different location
    return ParameterMap{m_container_ptr, make_full_key(location)};
  }

  // TODO alias names, i.e. link one name to a different one.
  //      but be careful not to get a cyclic graph.

  /** \name Key iterators */
  //@{
  /** Return a begin iterator which runs over all keys of this map */
  KeyIterator begin_keys() const { return begin_keys("/"); }

  /** Return an end iterator to run over the keys of this map */
  KeyIterator end_keys() const { return end_keys("/"); }

  //@{
  /** Return a begin iterator which runs over all keys of a submap */
  KeyIterator begin_keys(const std::string& path) const;

  /** Return an end iterator matching to begin_keys(path) */
  KeyIterator end_keys(const std::string& path) const;
  //@}

 private:
  /** \brief Construct parameter map from another map and a new location. */
  ParameterMap(std::shared_ptr<inner_map_type> map, std::string newlocation)
        : m_container_ptr{map}, m_location{newlocation} {}

  /** Make the actual container key from a key supplied by the user
   *  Care is taken such that we cannot escape the subtree.
   * */
  std::string make_full_key(const std::string& key) const;

  std::shared_ptr<inner_map_type> m_container_ptr;

  /** The location we are currently on in the tree
   * may not end with a slash (but a full key like "/tree"
   */
  std::string m_location;
};

//
// -----------------------------------------------------------------
//

//
// EntryValue subclass
//
template <typename T, typename>
ParameterMap::EntryValue::EntryValue(std::shared_ptr<T> t_ptr) {
  // see copy_in and m_object_ptr_ptr comments for details why this is done
  m_object_ptr_ptr = std::make_shared<RCPWrapper<T>>(std::move(t_ptr));
#ifdef DEBUG
  m_type_name = std::string(typeid(T).name());
#endif
}

template <typename T, typename>
ParameterMap::EntryValue::EntryValue(RCPWrapper<T> t_ptr) {
  // see copy_in and m_object_ptr_ptr comments for details why this is done
  m_object_ptr_ptr = std::make_shared<RCPWrapper<T>>(std::move(t_ptr));
#ifdef DEBUG
  m_type_name = std::string(typeid(T).name());
#endif
}

template <typename T,
          typename std::enable_if<std::is_base_of<Subscribable, T>::value &&
                                        !IsCheaplyCopyable<T>::value &&
                                        !std::is_same<ParameterMap, decay_t<T>>::value,
                                  int>::type>
ParameterMap::EntryValue::EntryValue(T& t) {
  SubscriptionPointer<T> t_ptr = make_subscription(t, "EntryValue");

  // see copy_in and m_object_ptr_ptr comments for details why this is done
  m_object_ptr_ptr = std::make_shared<RCPWrapper<T>>(std::move(t_ptr));

#ifdef DEBUG
  m_type_name = std::string(typeid(T).name());
#endif
}

template <typename T>
void ParameterMap::EntryValue::copy_in(T t) {
  // Make a shared pointer out of T:
  auto t_ptr = std::make_shared<T>(std::move(t));

  // Now enwrap the t_ptr inside a RCPWrapper and make
  // a shared_ptr to the Wrapper, then set it
  m_object_ptr_ptr = std::make_shared<RCPWrapper<T>>(t_ptr);

#ifdef DEBUG
  // Keep an eye on the type name
  m_type_name = std::string(typeid(T).name());
#endif
}

template <typename T>
RCPWrapper<T> ParameterMap::EntryValue::get_ptr() {
  assert_dbg(m_type_name == std::string(typeid(T).name()),
             ExcWrongTypeRequested(real_typename<T>(), demangled_string(m_type_name)));
  assert_dbg(!empty(), ExcInvalidPointer());

  // We need to cast and then dereference to get the RCPWrapper of the
  // appropriate type out.
  return *std::static_pointer_cast<RCPWrapper<T>>(m_object_ptr_ptr);
}

template <typename T>
RCPWrapper<const T> ParameterMap::EntryValue::get_ptr() const {
  assert_dbg(m_type_name == std::string(typeid(T).name()),
             ExcWrongTypeRequested(real_typename<T>(), demangled_string(m_type_name)));
  assert_dbg(!empty(), ExcInvalidPointer());

  // We need to cast and then dereference to get the RCPWrapper of the
  // appropriate type out.
  return *std::static_pointer_cast<RCPWrapper<const T>>(m_object_ptr_ptr);
}

//
// ParameterMap
//
template <typename T>
T& ParameterMap::at(const std::string& key) {
  auto itkey = m_container_ptr->find(make_full_key(key));
  assert_throw(itkey != std::end(*m_container_ptr), ExcUnknownKey(key));
  return itkey->second.get<T>();
}

template <typename T>
const T& ParameterMap::at(const std::string& key) const {
  auto itkey = m_container_ptr->find(make_full_key(key));
  assert_throw(itkey != std::end(*m_container_ptr), ExcUnknownKey(key));
  return itkey->second.get<T>();
}

template <typename T>
T& ParameterMap::at(const std::string& key, T& default_value) {
  auto itkey = m_container_ptr->find(make_full_key(key));
  if (itkey == std::end(*m_container_ptr)) {
    // Key not found, return default:
    return default_value;
  } else {
    // Key found, return mapped value
    return itkey->second.get<T>();
  }
}

template <typename T>
const T& ParameterMap::at(const std::string& key, const T& default_value) const {
  auto itkey = m_container_ptr->find(make_full_key(key));
  if (itkey == std::end(*m_container_ptr)) {
    // Key not found, return default:
    return default_value;
  } else {
    // Key found, return mapped value
    return itkey->second.get<T>();
  }
}

template <typename T>
RCPWrapper<T> ParameterMap::at_ptr(const std::string& key) {
  auto itkey = m_container_ptr->find(make_full_key(key));
  assert_throw(itkey != std::end(*m_container_ptr), ExcUnknownKey(key));
  return itkey->second.get_ptr<T>();
}

template <typename T>
RCPWrapper<const T> ParameterMap::at_ptr(const std::string& key) const {
  auto itkey = m_container_ptr->find(make_full_key(key));
  assert_throw(itkey != std::end(*m_container_ptr), ExcUnknownKey(key));
  return itkey->second.get_ptr<T>();
}

}  // krims
