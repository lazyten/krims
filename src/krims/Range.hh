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
#include "macros/deprecated.hh"
#include <cstddef>
#include <iostream>
#include <krims/TypeUtils/UsingLibrary.hh>
#include <utility>

namespace krims {

template <typename T>
class RangeIterator;

/** A range of integral values
 *
 * \note Empty ranges are allowed, but calling any function in order to
 * access elements of the range(``front()``,``back()``, ``operator[]``)
 * leads to undefined behaviour. In this case ``begin()`` is furthermore
 * equivalent to ``end()``.
 * */
template <typename T>
class Range {
 public:
  static_assert(std::is_integral<T>::value, "T needs to be an integral data type");

  typedef T value_type;
  typedef size_t size_type;

  // If T is a signed type use the signed equivalent of size_type
  // to compute the differences, else use size_type;
  typedef typename std::conditional<std::is_signed<T>::value,
                                    typename std::make_signed<size_type>::type,
                                    size_type>::type diff_type;

  DefExceptionMsg(ExcEmptyRange,
                  "The range object you attempted to use represents an empty range"
                  " and hence cannot be used in this way.");

  /** \brief Construct an empty range */
  Range() : m_first(0), m_last(0) {}

  /** \brief Construct a range
   *
   * Construct the range [first, last). If last is smaller or equal
   * to first, the range is empty.
   *
   * \note The interval is half-open, i.e. the range
   * is including the first element but not the last.
   * */
  Range(value_type first, value_type last)
        : m_first(first), m_last(std::max(first, last)) {
    assert_greater_equal(m_first, m_last);
  }
  /** \brief Construct from pair
   *
   * Construct the range [first, second). If second is smaller or equal
   * to first, the range is empty.
   *
   * \note The interval is half-open, i.e. first is inclusive, but second
   * exclusive.
   */
  explicit Range(std::pair<value_type, value_type> first_last)
        : m_first(first_last.first),
          m_last(std::max(first_last.first, first_last.second)) {
    assert_greater_equal(m_first, m_last);
  }

  /** \brief Return the effective number of elements in the range
   *
   * i.e. if this class represents [3,5) it returns 2.
   * */
  size_type length() const {
    return static_cast<size_type>(static_cast<diff_type>(m_last) -
                                  static_cast<diff_type>(m_first));
  }

  /** \brief Return the effective number of elements in the range
   *
   * alias to length()
   */
  size_type size() const { return length(); }

  /** Get the first element of the range */
  value_type front() const {
    assert_dbg(!empty(), ExcEmptyRange());
    return m_first;
  }

  /** Get the last element of the range  */
  value_type back() const {
    assert_dbg(!empty(), ExcEmptyRange());
    return m_last - 1;
  }

  /** Return the lower bound of the range, which is inclusive. */
  value_type lower_bound() const { return m_first; }

  /** Return the lower bound of the range, which is exclusive. */
  value_type upper_bound() const { return m_last; }

  /** Is this range empty */
  bool empty() const { return length() <= 0; }

  /** Check whether the provided value is in range */
  bool contains(value_type i) const { return m_first <= i && i < m_last; }

  /** Return the ith value of the range */
  value_type operator[](size_type i) const;

  /** Return an iterator to the first element of the range */
  RangeIterator<T> begin() const {
    if (empty()) return end();
    return RangeIterator<T>{m_first};
  }

  /** Return an iterator to the last element of the range */
  RangeIterator<T> end() const { return RangeIterator<T>{m_last}; }

  /** \name Shifting operations */
  ///@{
  /** Shift the range by a certain value */
  Range& operator+=(value_type i);

  /** Shift the range by a certain value */
  Range& operator-=(value_type i);
  ///@}

  template <typename U>
  bool operator==(const Range<U>& other) const {
    typedef krims::common_type_t<U, T> type;
    return static_cast<type>(lower_bound()) == static_cast<type>(other.lower_bound()) &&
           static_cast<type>(upper_bound()) == static_cast<type>(other.upper_bound());
  }

  template <typename U>
  bool operator!=(const Range<U>& other) const {
    return !((*this) == other);
  }

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
  return std::move(r) + i;
}

template <typename T>
Range<T> operator-(Range<T> r, T i) {
  return r -= i;
}

template <typename T>
Range<T> operator-(const Range<T>& r) {
  static_assert(std::is_signed<T>::value,
                "T needs to be a signed type for this to work.");
  return {-r.upper_bound() + 1, -r.lower_bound() + 1};
}

template <typename T>
Range<T> operator-(T i, const Range<T>& r);

/** Iterator for ranges */
template <typename T>
class RangeIterator : public std::iterator<std::input_iterator_tag, T> {
 public:
  typedef std::iterator<std::input_iterator_tag, T> base_type;
  typedef typename base_type::value_type value_type;
  typedef typename base_type::reference reference;
  typedef typename base_type::pointer pointer;

  /** Constructs an iterator-past-the-end */
  RangeIterator() : m_current{invalid} {}

  /** Construct an iterator which currently points at
   * current and runs until last-1 (i.e. last is *not*
   * included any more.
   * */
  RangeIterator(value_type current) : m_current{current} {
    assert_dbg(m_current != invalid,
               krims::ExcInvalidState(
                     "Cannot construct an iterator from the special value for invalid."));
  }

  /** Prefix increment to the next value */
  RangeIterator& operator++();

  /** Postfix increment to the next value */
  RangeIterator operator++(int);

  /** Return the value of the element we point to. */
  value_type operator*() const {
    assert_dbg(m_current != invalid,
               krims::ExcInvalidState(
                     "Cannot use a default-constructed iterator in that way."));
    return m_current;
  }

  /** Access the members of the element we point to. */
  const pointer operator->() const {
    assert_dbg(m_current != invalid,
               krims::ExcInvalidState(
                     "Cannot use a default-constructed iterator in that way."));
    return &m_current;
  }

  /** Check if two iterators are equal */
  bool operator==(const RangeIterator& other) const {
    return m_current == other.m_current;
  }

  /** Check whether two iterators are unequal */
  bool operator!=(const RangeIterator& other) const { return !(*this == other); }

 private:
  static constexpr T invalid = std::numeric_limits<T>::max();
  value_type m_current;
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
typename Range<T>::value_type Range<T>::operator[](size_type i) const {
  assert_dbg(!empty(), ExcEmptyRange());
  assert_range(0u, i, length());
  return m_first + i;
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
  return *this;
}

template <typename T>
Range<T>& Range<T>::operator-=(value_type i) {
  if (i < 0) {
    assert_dbg(m_first - i >= m_first, krims::ExcOverflow());
    assert_dbg(m_last - i >= m_last, krims::ExcOverflow());
  } else {
    assert_dbg(m_first - i <= m_first, krims::ExcUnderflow());
    assert_dbg(m_last - i <= m_last, krims::ExcUnderflow());
  }

  m_first -= i;
  m_last -= i;
  return *this;
}

template <typename T>
std::ostream& operator<<(std::ostream& o, const Range<T>& r) {
  o << "[" << r.lower_bound() << "," << r.upper_bound() << ")";
  return o;
}

template <typename T>
Range<T> operator-(T i, const Range<T>& r) {
  if (r.empty()) {
    return {i - r.lower_bound(), i - r.upper_bound()};
  }

  const T lower = i + 1 - r.upper_bound();
  const T upper = i + 1 - r.lower_bound();

  if (r.upper_bound() >= 0) {
    assert_dbg(lower <= i + 1, krims::ExcUnderflow());
  } else {
    assert_dbg(lower >= i + 1, krims::ExcOverflow());
  }

  if (r.lower_bound() >= 0) {
    assert_dbg(upper <= i + 1, krims::ExcUnderflow());
  } else {
    assert_dbg(upper >= i + 1, krims::ExcOverflow());
  }

  return {lower, upper};
}

//
// ------------------------------------------------
//

template <typename T>
RangeIterator<T>& RangeIterator<T>::operator++() {
  assert_dbg(
        m_current != invalid,
        krims::ExcInvalidState("Cannot use a default-constructed iterator in that way."));
  m_current++;
  return (*this);
}

template <typename T>
RangeIterator<T> RangeIterator<T>::operator++(int) {
  assert_dbg(
        m_current != invalid,
        krims::ExcInvalidState("Cannot use a default-constructed iterator in that way."));
  RangeIterator<T> copy{*this};
  ++(*this);
  return copy;
}

}  // namespace krims
