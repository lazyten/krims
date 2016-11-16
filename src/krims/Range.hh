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
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <utility>

namespace krims {

template <typename T>
class RangeIterator;

/** A range of integral values
 *
 * \note Empty ranges are allowed, but calling any function in order to
 * access elements of the range(``first()``,``last()``, ``operator[]``)
 * leads to undefined behaviour. In this case ``begin()`` is furthermore
 * equivalent to ``end()``.
 * */
template <typename T>
class Range {
public:
  static_assert(std::is_integral<T>::value,
                "T needs to be an integral data type");

  typedef T value_type;
  typedef size_t size_type;

  // If T is a signed type use the signed equivalent of size_type
  // to compute the differences, else use size_type;
  typedef typename std::conditional<std::is_signed<T>::value,
                                    typename std::make_signed<size_type>::type,
                                    size_type>::type diff_type;

  DefExceptionMsg(
        ExcEmptyRange,
        "The range object you attempted to use represents an empty range"
        " and hence cannot be used in this way.");

  /** \brief Construct a range
   *
   * Note that the interval is half-open, i.e. the range
   * is including the first element but not the last.
   * */
  Range(value_type first, value_type last) : m_first(first), m_last(last) {
    assert_greater_equal(m_first, m_last);
  }
  /** \brief Construct from pair
   *
   * The first element is inclusive, the last exclusive
   * (half-open interval)
   */
  explicit Range(std::pair<value_type, value_type> first_last)
        : m_first(first_last.first), m_last(first_last.second) {
    assert_greater_equal(m_first, m_last);
  }

  /** \brief Return the effective number of elements in the range
   *
   * i.e. if this class represents [3,5) it returns 2.
   * */
  size_type length() const;

  /** \brief Return the effective number of elements in the range
   *
   * alias to length()
   */
  size_type size() const { return length(); }

  /** Get the first element, which is inclusive */
  value_type first() const;

  /** Get the last element, which is exclusive */
  value_type last() const;

  /** Is this range empty */
  bool empty() const { return length() <= 0; }

  /** Check whether the provided value is in range */
  bool contains(value_type i) const { return m_first <= i && i < m_last; }

  /** Return the ith value of the range */
  value_type operator[](size_type i) const;

  /** Return an iterator to the first element of the range */
  RangeIterator<T> begin() const;

  /** Return an iterator to the last element of the range */
  RangeIterator<T> end() const;

  /** \name Shifting operations */
  ///@{
  /** Shift the range by a certain value */
  Range& operator+=(value_type i);

  /** Shift the range by a certain value */
  Range& operator-=(value_type i) { return (*this) += -i; }
  ///@}

private:
  value_type m_first;  // inclusive
  value_type m_last;   // exclusive
};

/** Output operator for ranges */
template <typename T>
std::ostream& operator<<(std::ostream& o, const Range<T>& r);

template <typename T>
Range<T> operator+(Range<T> r, T i) {
  return r += i;
}

template <typename T>
Range<T> operator+(T i, Range<T> r) {
  return r + i;
}

template <typename T>
Range<T> operator-(Range<T> r, T i) {
  return r -= i;
}

template <typename T>
Range<T>& operator-(T i, Range<T> r) {
  assert_dbg(false, krims::ExcNotImplemented());
  (void)i;
  (void)r;
}

/** Iterator for ranges */
template <typename T>
class RangeIterator : public std::iterator<std::input_iterator_tag, T> {
public:
  typedef std::iterator<std::input_iterator_tag, T> base_type;
  typedef typename base_type::value_type value_type;
  typedef typename base_type::reference reference;
  typedef typename base_type::pointer pointer;

  /** Constructs an iterator-past-the-end */
  RangeIterator() : m_current{0}, m_last{0} {}

  /** Construct an iterator which currently points at
   * current and runs until last-1 (i.e. last is *not*
   * included any more.
   * */
  RangeIterator(value_type current, value_type last)
        : m_current{current}, m_last{last} {
    assert_dbg(!is_past_the_end(), krims::ExcIteratorPastEnd());
  }

  /** Prefix increment to the next value */
  RangeIterator& operator++();

  /** Postfix increment to the next value */
  RangeIterator operator++(int);

  /** Return the value of the element we point to. */
  value_type operator*() const;

  /** Access the members of the element we point to. */
  const pointer operator->() const;

  /** Check if two iterators are equal */
  bool operator==(const RangeIterator& other) const;

  /** Check whether two iterators are unequal */
  bool operator!=(const RangeIterator& other) const;

private:
  /** Does this data structure represent an
   *  iterator-past-the-end */
  bool is_past_the_end() const { return m_current >= m_last; }

  value_type m_current;
  value_type m_last;
};

//
// Helper functions for ranges:
//
/** Return a range interval from 0 to \p t, i.e. 0 is included, but \p t not. */
template <typename T>
Range<T> range(const T& t) {
  return Range<T>{0, t};
}

/** Return a range interval from \p t1 to \p t2, where \p t1 is included,
 * but \p t2 not. */
template <typename T>
Range<T> range(const T& t1, const T& t2) {
  return Range<T>{t1, t2};
}

//
// ---------------------------------------------------
//

template <typename T>
typename Range<T>::value_type Range<T>::first() const {
  assert_dbg(!empty(), ExcEmptyRange());
  return m_first;
}

template <typename T>
typename Range<T>::value_type Range<T>::last() const {
  assert_dbg(!empty(), ExcEmptyRange());
  return m_last;
}

template <typename T>
typename Range<T>::size_type Range<T>::length() const {
  return static_cast<diff_type>(m_last) - static_cast<diff_type>(m_first);
}

template <typename T>
typename Range<T>::value_type Range<T>::operator[](size_type i) const {
  assert_dbg(!empty(), ExcEmptyRange());
  assert_range(0u, i, length());
  return m_first + i;
}

template <typename T>
RangeIterator<T> Range<T>::begin() const {
  if (empty()) return end();
  return RangeIterator<T>{m_first, m_last};
}

template <typename T>
RangeIterator<T> Range<T>::end() const {
  return RangeIterator<T>{};
}

template <typename T>
Range<T>& Range<T>::operator+=(value_type i) {
  if (i > 0) {
    assert_dbg(m_first + i >= m_first, krims::ExcOverflow());
    assert_dbg(m_last + i >= m_last, krims::ExcOverflow());
  } else {
    assert_dbg(m_first + i <= m_first, krims::ExcUnderflow());
    assert_dbg(m_last + i <= m_last, krims::ExcUnderflow());
  }

  m_first += i;
  m_last += i;
}

template <typename T>
std::ostream& operator<<(std::ostream& o, const Range<T>& r) {
  if (r.empty()) {
    o << "[0,0)";
  } else {
    o << "[" << r.first() << "," << r.last() << ")";
  }
  return o;
}

//
// ------------------------------------------------
//

template <typename T>
RangeIterator<T>& RangeIterator<T>::operator++() {
  assert_dbg(!is_past_the_end(), krims::ExcIteratorPastEnd());
  m_current++;
  return (*this);
}

template <typename T>
RangeIterator<T> RangeIterator<T>::operator++(int) {
  assert_dbg(!is_past_the_end(), krims::ExcIteratorPastEnd());
  RangeIterator<T> copy{*this};
  ++(*this);
  return copy;
}

template <typename T>
typename RangeIterator<T>::value_type RangeIterator<T>::operator*() const {
  assert_dbg(!is_past_the_end(), krims::ExcIteratorPastEnd());
  return m_current;
}

template <typename T>
const typename RangeIterator<T>::pointer RangeIterator<T>::operator->() const {
  assert_dbg(!is_past_the_end(), krims::ExcIteratorPastEnd());
  return &m_current;
}

template <typename T>
bool RangeIterator<T>::operator==(const RangeIterator& other) const {
  // The iterators are equal if they are either both past the end
  // or their m_current and their m_last agrees

  const bool both_past_the_end = is_past_the_end() && other.is_past_the_end();
  const bool identical_values =
        (m_current == other.m_current && m_last == other.m_last);
  return both_past_the_end || identical_values;
}

template <typename T>
bool RangeIterator<T>::operator!=(const RangeIterator& other) const {
  return !(*this == other);
}

}  // namespace krims