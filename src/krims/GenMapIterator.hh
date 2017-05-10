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
#include "ExceptionSystem.hh"
#include "GenMapAccessor.hh"
#include <iterator>
#include <map>

namespace krims {

template <bool Const>
class GenMapIterator
      : std::iterator<std::bidirectional_iterator_tag, GenMapAccessor<Const>> {
 public:
  typedef typename detail::GenMapTraits::map_type map_type;

  //** The iterator type which is used in this class
  // to iterate over the map contained in GenMap. */
  typedef krims::conditional_t<Const, typename map_type::const_iterator,
                               typename map_type::iterator>
        iter_type;

  /** Dereference GenMap iterator */
  GenMapAccessor<Const>& operator*() const { return *operator->(); }

  /** Obtain pointer to GenMap accessor */
  GenMapAccessor<Const>* operator->() const;

  /** Prefix increment to the next key */
  GenMapIterator& operator++() {
    ++m_iter;
    m_acc_ptr.reset();  // Reset cache
    return *this;
  }

  /** Postfix increment to the next key */
  GenMapIterator operator++(int) {
    GenMapIterator copy(*this);
    this->operator++();
    return copy;
  }

  /** Prefix decrement to the next key */
  GenMapIterator& operator--() {
    --m_iter;
    m_acc_ptr.reset();  // Reset cache
    return *this;
  }

  /** Postfix decrement to the next key */
  GenMapIterator operator--(int) {
    GenMapIterator copy(*this);
    this->operator--();
    return copy;
  }

  bool operator==(const GenMapIterator& other) const { return m_iter == other.m_iter; }
  bool operator!=(const GenMapIterator& other) const { return m_iter != other.m_iter; }

  /** Explicit conversion to the inner iterator type */
  explicit operator iter_type() { return m_iter; }

  GenMapIterator(iter_type iter, std::string location)
        : m_acc_ptr(nullptr), m_iter(iter), m_location(std::move(location)) {}

  GenMapIterator() : m_acc_ptr(nullptr), m_iter(), m_location() {}

 private:
  /** Undo the operation of GenMap::make_full_key, i.e. strip off the
   * first location part and get a relative path to it*/
  std::string strip_location_prefix(const std::string& key) const;

  /** Cache for the accessor of the current value.
   *  A stored nullptr implies that the accessor needs to rebuild
   *  before using it.*/
  mutable std::shared_ptr<GenMapAccessor<Const>> m_acc_ptr;

  /** Iterator to the current key,value pair */
  iter_type m_iter;

  /** Subtree location we iterate over */
  std::string m_location;
};

//
// -----------------------------------------------
//

template <bool Const>
GenMapAccessor<Const>* GenMapIterator<Const>::operator->() const {
  if (m_acc_ptr == nullptr) {
    // Generate accessor for current state
    const std::string key_stripped = strip_location_prefix(m_iter->first);
    m_acc_ptr = std::make_shared<GenMapAccessor<Const>>(key_stripped, m_iter->second);
  }

  return m_acc_ptr.get();
}

template <bool Const>
std::string GenMapIterator<Const>::strip_location_prefix(const std::string& key) const {
  // The first part needs to be exactly the location:
  assert_internal(key.size() >= m_location.size());
  assert_internal(0 == key.compare(0, m_location.size(), m_location));

  if (key.size() <= m_location.size()) {
    return "/";
  } else {
    std::string res = key.substr(m_location.size());
    assert_internal(res[0] == '/' || res.length() == 0);
    assert_internal(res.back() != '/');
    return res;
  }
}

}  // namespace krims
