//
// Copyright (C) 2017 by the krims authors
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
#include "detail/GenMapTraits.hh"

namespace krims {

/** Accessor to a GenMap object. Can be used to retrieve the key or the value
 *  or the typename of the value */
template <bool Const>
class GenMapAccessor {};

template <>
class GenMapAccessor<true> {
 public:
  typedef detail::GenMapTraits::entry_value_type entry_value_type;

  /** Return the key of the key/value pair the accessor holds */
  const std::string& key() const { return m_key; }

  /** Return the type name of the value object referred to by the key, which
   * is held in this accessor.
   *
   * The type_name is only sensible in DEBUG mode.
   */
  std::string type_name() const { return m_value.type_name(); }

  /** Return the value of the key/value pair the accessor holds (Const version).
   *
   * Raises an exception if the value object referred to by this key
   * cannot be converted to this value type.
   **/
  template <typename T>
  const T& value() const {
    return m_value.get<T>();
  }

  /** Return the value of the key/value pair the accessor holds
   * as a pointer (Const version).
   *
   * Raises an exception if the value object referred to by this key
   * cannot be converted to this value type.
   **/
  template <typename T>
  RCPWrapper<const T> value_ptr() const {
    return m_value.get_ptr<T>();
  }

  /** Return a reference to the raw value object the accessor holds. (Const version)
   *
   * \note This is an advanced method. Use only if you know what you are doing.
   **/
  const entry_value_type& value_raw() const { return m_value; }

  /** Construct an accessor */
  GenMapAccessor(const std::string key, const entry_value_type& value)
        : m_key(key), m_value(value) {}

 private:
  const std::string m_key;
  const entry_value_type& m_value;
};

template <>
class GenMapAccessor<false> : public GenMapAccessor<true> {
  // Use the const version as the base type:
  typedef GenMapAccessor<true> base_type;

 public:
  typedef detail::GenMapTraits::entry_value_type entry_value_type;

  /** Return the value of the key/value pair the accessor holds.
   *
   * Raises an exception if the value object referred to by this key
   * cannot be converted to this value type.
   **/
  template <typename T>
  T& value() {
    return m_value.get<T>();
  }

  /** Return the value of the key/value pair the accessor holds
   * as a pointer.
   *
   * Raises an exception if the value object referred to by this key
   * cannot be converted to this value type.
   **/
  template <typename T>
  RCPWrapper<T> value_ptr() {
    return m_value.get_ptr<T>();
  }

  /** Return the value of the key/value pair the accessor holds (Const version).
   *
   * Raises an exception if the value object referred to by this key
   * cannot be converted to this value type.
   **/
  template <typename T>
  const T& value() const {
    return m_value.get<T>();
  }

  /** Return the value of the key/value pair the accessor holds
   * as a pointer (Const version).
   *
   * Raises an exception if the value object referred to by this key
   * cannot be converted to this value type.
   **/
  template <typename T>
  RCPWrapper<const T> value_ptr() const {
    return m_value.get_ptr<T>();
  }

  /** Return a reference to the raw value object the accessor holds. ( Const version)
   *
   * \note This is an advanced method. Use only if you know what you are doing.
   **/
  const entry_value_type& value_raw() const { return m_value; }

  /** Return a reference to the raw value object the accessor holds.
   *
   * \note This is an advanced method. Use only if you know what you are doing.
   **/
  entry_value_type& value_raw() { return m_value; }

  /** Construct an accessor */
  GenMapAccessor(const std::string key, entry_value_type& value)
        : base_type(key, value), m_value(value) {}

 private:
  entry_value_type& m_value;
};

}  // namespace krims
