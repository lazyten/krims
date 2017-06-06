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

namespace krims {

/** \name Class which allows to iterate between a range given by a start and end
 * iterator in circular fashion.
 *
 * This means that it may start at any point between m_first and m_last and it
 * iterates once through each element in this range, but flipping around from
 * m_last to m_first on calling operator++ */
template <typename Iterator>
class CircularIterator
      : public std::iterator<std::bidirectional_iterator_tag,
                             typename Iterator::value_type,
                             typename Iterator::difference_type,
                             typename Iterator::pointer, typename Iterator::reference> {
 public:
  typedef Iterator iterator_type;
  typedef typename iterator_type::value_type value_type;
  typedef typename iterator_type::reference reference;
  typedef typename iterator_type::pointer pointer;

  static_assert(std::is_same<typename iterator_type::iterator_category,
                             std::bidirectional_iterator_tag>::value ||
                      std::is_same<typename iterator_type::iterator_category,
                                   std::random_access_iterator_tag>::value,
                "The Iterator needs to be at least a bidirectional iterator");

  //@{
  /** Defaults for the big five */
  ~CircularIterator()                       = default;
  CircularIterator()                        = default;
  CircularIterator(CircularIterator&&)      = default;
  CircularIterator(const CircularIterator&) = default;
  CircularIterator& operator=(CircularIterator&&) = default;
  CircularIterator& operator=(const CircularIterator&) = default;
  //@}

  /** \name Sensible construction yielding a valid CircularIterator
   *
   * Iterate on the range [first,end) in an infinite circle,
   * starting on position start.
   *
   * \param first  The first element of the range (should point to an actual
   * element!)
   * \param end    Iterator to past-the-last element (never touched)
   * \param start  A valid element within the range to start with.
   * \param begin_iterator Is this a begin iterator returned from a std::begin,
   * .begin() or similar function?
   *               This flag is needed to ensure the expected behaviour for
   * ranged-based iterators or iterators in for loops. The problem is that for
   * this circular iterator, the positon and range of the begin and end iterator
   * are exactly identical.
   * (Since the range is a circle, the element-past-the end is the beginning!)
   * */
  CircularIterator(iterator_type first, iterator_type end, iterator_type start,
                   bool begin_iterator = false)
        : m_begin_iterator{begin_iterator}, m_first{first}, m_pos{start}, m_end{end} {}

  //
  // Increment and decrement
  //

  /** Prefix increment to the next element */
  CircularIterator& operator++();

  /** Postfix increment to the next element */
  CircularIterator operator++(int);

  /** Prefix increment to the next element */
  CircularIterator& operator--();

  /** Postfix increment to the next element */
  CircularIterator operator--(int);

  //
  // Element access
  //
  /** Return the value of the element we point to. */
  reference operator*() const {
    assert_dbg(m_first != m_end,
               ExcInvalidState("Cannot dereference CircularIterator over empty range"));

    return *m_pos;
  }

  /** Access the members of the element we point to. */
  pointer operator->() const {
    assert_dbg(m_first != m_end,
               ExcInvalidState("Cannot dereference CircularIterator over empty range"));
    return m_pos.operator->();
  }

  //
  // Comparison
  //
  /** \brief check if two iterators are equal
   *
   * CircularIterators are equal if they iterate over the same range and point
   * to the same element and the begin_iterator flag is identical in both
   * objects.
   */
  bool operator==(const CircularIterator& other) const;

  /** \brief Check whether two iterators are unequal
   *
   * CircularIterators are unequal if they iterate over a different range or
   * point to a different element.
   */
  bool operator!=(const CircularIterator& other) const { return !(*this == other); }

  //
  // Access range and position
  //
  /** Get the iteration range as a pair of iterators marking start and end */
  std::pair<iterator_type, iterator_type> iteration_range() {
    return std::make_pair(m_first, m_end);
  }

  /** Get the iterator to the current position */
  iterator_type position() const {
    assert_dbg(m_first != m_end,
               ExcInvalidState("Cannot get position iterator of CircularIterator "
                               "over empty range"));
    return m_pos;
  }

 private:
  bool m_begin_iterator;  //< Is this a begin iterator
  iterator_type m_first;  //< First element of the range (inclusive)
  iterator_type m_pos;    //< Current position
  iterator_type m_end;    //< Last element of the range (exclusive)
};

/** \brief Convenience function to make a begin CircularIterator for a range,
 * specifying
 *  the start element as an iterator.
 *
 *  The iterator will iterate the range [begin:end), but in a circle and
 * starting
 *  from the element start.
 *  */
template <typename Iterator>
CircularIterator<Iterator> circular_begin(Iterator begin, Iterator end, Iterator start) {
  return CircularIterator<Iterator>(begin, end, start, true);
}

/** \brief Convenience function to make an end CircularIterator for a range,
 * specifying
 *  the start element as an iterator.
 *
 * The equivalent function to get a start iterator is circular_begin.
 * Use both function in pairs and supply them with the same arguments.
 *  */
template <typename Iterator>
CircularIterator<Iterator> circular_end(Iterator begin, Iterator end, Iterator start) {
  return CircularIterator<Iterator>(begin, end, start, false);
}

/** \brief Convenience function to make a begin CircularIterator for a
 * container, specifying the start element.
 *
 * The iterator will iterate the full container, but starting from the ith
 * element and looping around in circles.
 *  */
template <typename Container>
auto circular_begin(Container& c, typename Container::size_type i)
      -> CircularIterator<decltype(std::begin(c))> {
  return circular_begin(std::begin(c), std::end(c), std::next(std::begin(c), i));
}

/** \brief Convenience function to make an end CircularIterator for a container,
 * specifying the start element.
 *
 * The equivalent function to get a start iterator is circular_begin.
 * Use both function in pairs and supply them with the same arguments.
 *  */
template <typename Container>
auto circular_end(Container& c, typename Container::size_type i)
      -> CircularIterator<decltype(std::begin(c))> {
  return circular_end(std::begin(c), std::end(c), std::next(std::begin(c), i));
}

/** \brief Convenience function to make a begin CircularIterator for a
 * container, specifying the start element.
 *
 * The iterator will iterate the full container, but starting from the ith
 * element and looping around in circles.
 *  */
template <typename Container>
auto circular_begin(Container& c, decltype(std::begin(c)) start)
      -> CircularIterator<decltype(std::begin(c))> {
  return circular_begin(std::begin(c), std::end(c), start);
}

/** \brief Convenience function to make an end CircularIterator for a container,
 * specifying the start element.
 *
 * The equivalent function to get a start iterator is circular_begin.
 * Use both function in pairs and supply them with the same arguments.
 *  */
template <typename Container>
auto circular_end(Container& c, decltype(std::begin(c)) start)
      -> CircularIterator<decltype(std::begin(c))> {
  return circular_end(std::begin(c), std::end(c), start);
}

//
// ----------------------------------------------------------
//

template <typename Iterator>
CircularIterator<Iterator>& CircularIterator<Iterator>::operator++() {
  assert_dbg(m_first != m_end,
             ExcInvalidState("Cannot increment CircularIterator over empty range"));

  ++m_pos;
  if (m_pos == m_end) {
    // We are past the end: wrap around:
    m_pos = m_first;
  }

  m_begin_iterator = false;
  return *this;
}

template <typename Iterator>
CircularIterator<Iterator> CircularIterator<Iterator>::operator++(int) {
  CircularIterator copy{*this};
  ++(*this);
  return copy;
}

template <typename Iterator>
CircularIterator<Iterator>& CircularIterator<Iterator>::operator--() {
  assert_dbg(m_first != m_end,
             ExcInvalidState("Cannot decrement CircularIterator over empty range"));

  if (m_pos == m_first) {
    m_pos = std::prev(m_end);
  } else {
    --m_pos;
  }

  m_begin_iterator = false;
  return *this;
}

template <typename Iterator>
CircularIterator<Iterator> CircularIterator<Iterator>::operator--(int) {
  CircularIterator copy{*this};
  --(*this);
  return copy;
}

template <typename Iterator>
bool CircularIterator<Iterator>::operator==(const CircularIterator& other) const {
  if (m_first == m_end && other.m_first == other.m_end && m_first == other.m_first) {
    // Ranges of both iterators are equivalent and empty.
    // So m_begin_iterator and m_pos play no role.
    return true;
  }

  return m_pos == other.m_pos && m_first == other.m_first && m_end == other.m_end &&
         m_begin_iterator == other.m_begin_iterator;
}

}  // namespace krims
