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
#include "krims/ExceptionSystem.hh"
#include <iterator>
#include <type_traits>

namespace krims {
namespace detail {
template <typename T>
using PointedToTypeT =
      typename std::remove_reference<decltype(*std::declval<T>())>::type;
}  // namespace detail

/** Enwrap an iterator and return the dereferenced values of the internal
 * iterator
 *  Ideal for iterating over containers, which contain pointers to actual
 * objects */
template <typename Iterator>
struct DereferenceIterator {
  typedef detail::PointedToTypeT<
        typename std::iterator_traits<Iterator>::value_type>
        value_type;
  typedef std::random_access_iterator_tag iterator_category;
  typedef value_type* pointer;
  typedef value_type& reference;
  typedef typename std::add_const<reference>::type const_reference;
  typedef typename std::iterator_traits<Iterator>::difference_type
        difference_type;

  static_assert(
        std::is_same<typename std::iterator_traits<Iterator>::iterator_category,
                     std::random_access_iterator_tag>::value,
        "Currently DereferenceIterator is only implemented for "
        "bidirectional inner iterators");

  /** Construct using an inner iterator */
  DereferenceIterator(Iterator inner) : m_inner(inner) {}

  /** Default-construct */
  DereferenceIterator() : m_inner() {}

  bool operator==(const DereferenceIterator& other) const {
    return m_inner == other.m_inner;
  }
  bool operator!=(const DereferenceIterator& other) const {
    return m_inner != other.m_inner;
  }

  reference operator*() const {
    assert_dbg(*m_inner != nullptr, ExcInvalidPointer());
    return **m_inner;
  }

  pointer operator->() const { return &(operator*()); }

  DereferenceIterator& operator++() {
    ++m_inner;
    return *this;
  }

  DereferenceIterator operator++(int) {
    DereferenceIterator copy(*this);
    ++m_inner;
    return copy;
  }

  //
  // Bidirectional iterator
  //
  DereferenceIterator& operator--() {
    --m_inner;
    return *this;
  }

  DereferenceIterator operator--(int) {
    DereferenceIterator copy(*this);
    --m_inner;
    return copy;
  }

  //
  // Random-access iterator
  //
  DereferenceIterator operator-(difference_type n) const {
    DereferenceIterator copy(*this);
    copy.m_inner -= n;
    return copy;
  }

  DereferenceIterator operator+(difference_type n) const {
    DereferenceIterator copy(*this);
    copy.m_inner += n;
    return copy;
  }

  DereferenceIterator& operator-=(difference_type n) {
    m_inner -= n;
    return *this;
  }

  DereferenceIterator& operator+=(difference_type n) {
    m_inner += n;
    return *this;
  }

  reference operator[](difference_type n) {
    assert_dbg(m_inner[n] != nullptr, ExcInvalidPointer());
    return *(m_inner[n]);
  }

  const_reference operator[](difference_type n) const {
    assert_dbg(m_inner[n] != nullptr, ExcInvalidPointer());
    return *(m_inner[n]);
  }

  bool operator<(const DereferenceIterator& i) const {
    return (m_inner < i.m_inner);
  }

  bool operator>(const DereferenceIterator& i) const {
    return (m_inner > i.m_inner);
  }

  bool operator<=(const DereferenceIterator& i) const {
    return (m_inner <= i.m_inner);
  }

  bool operator>=(const DereferenceIterator& i) const {
    return (m_inner >= i.m_inner);
  }

private:
  Iterator m_inner;
};

template <typename Iterator>
DereferenceIterator<Iterator> operator+(
      typename DereferenceIterator<Iterator>::difference_type n,
      DereferenceIterator<Iterator> i) {
  return (i + n);
}

}  // namespace krims
