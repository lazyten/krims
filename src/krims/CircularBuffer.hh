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
#include "IteratorUtils/CircularIterator.hh"
#include <list>
#include <memory>

namespace krims {

template <typename T, typename Container = std::list<T>>
class CircularBuffer {
public:
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;

  //! The type of the inner container
  typedef Container container_type;

  typedef typename container_type::size_type size_type;
  typedef CircularIterator<typename container_type::iterator> iterator;
  typedef CircularIterator<typename container_type::const_iterator>
        const_iterator;

  static_assert(
        std::is_same<value_type, typename container_type::value_type>::value,
        "The type T needs to agree with the value_type of the Container type.");

  /** \name Constructor
   *
   * \param max_size  The maximal size of the buffer
   * */
  CircularBuffer(size_type max_size)
        : CircularBuffer{max_size, std::initializer_list<T>{}} {}

  /** \name Constructor
   *
   * \param max_size  The maximal size of the buffer
   * \param il  initial list of elements for the buffer
   *            Assumes size of il to be no greater than
   *            max_size.
   **/
  CircularBuffer(size_type max_size, std::initializer_list<T> il)
        : m_storage{il},
          m_max_size{max_size},
          m_first{circular_begin(m_storage, 0)} {
    assert_greater_equal(il.size(), max_size);
  }

  /* \name Modifiers
   */
  ///@{
  /** \name Push an element before the first element,
   *  possibly overwriting the current last element of the
   *  circular buffer if max_size() has been reached.
   */
  void push_front(value_type val);

  /** \name Push an element after the last element,
   *  possibly overwriting the current first element of the
   *  circular buffer if max_size() has been reached.
   */
  void push_back(value_type val);
  //@}

  /* \name Element access
   */
  ///@{
  //@{
  /* \name Access the first element */
  reference front() { return *m_first; }
  const_reference front() const { return *m_first; }
  //@}

  //@{
  /* \name Access the last element */
  reference back() {
    // Return the last actual element of the buffer, this is the one
    // previous to first in the circular sense
    return *std::prev(m_first);
  }
  const_reference back() const { return *std::prev(m_first); }
  //@}
  ///@}

  /* \name Iterators
   */
  ///@{
  iterator begin() {
    // Return a circular iterator, which is a begin iterator.
    return circular_begin<typename container_type::iterator>(
          std::begin(m_storage), std::end(m_storage), m_first.position());
  }

  iterator end() {
    // Return a circular iterator, which is a begin iterator.
    return circular_end<typename container_type::iterator>(
          std::begin(m_storage), std::end(m_storage), m_first.position());
  }

  const_iterator begin() const { return cbegin(); }

  const_iterator end() const { return cend(); }

  const_iterator cbegin() const {
    return circular_begin<typename container_type::const_iterator>(
          m_storage.cbegin(), m_storage.cend(), m_first.position());
  }

  const_iterator cend() const {
    return circular_end<typename container_type::const_iterator>(
          m_storage.cbegin(), m_storage.cend(), m_first.position());
  }
  ///@}

  /** \name Discard all elements of the buffer
   *
   * The size is zero, but max_size is unaltered.
   * */
  void clear() {
    // Clear storage and reset iterator:
    m_storage.clear();
    m_first = circular_begin(m_storage, 0);
  }
  ///@}

  /* \name Capacity
   */
  ///@{
  /** Return the maximal size of the buffer */
  size_type max_size() const { return m_max_size; }

  /** \brief Change the maximal size.
   *
   * If the max_size is increased, space for
   * more values is added at the back.
   * If max_size is decreased, leftover elements
   * at the back are deleted.
   */
  void max_size(size_type msize);

  /** Return the actual size of the buffer */
  size_type size() const { return m_storage.size(); }

  /** Test whether container is empty */
  bool empty() const { return m_storage.empty(); }
  ///@}

private:
  //! Storage for the buffer
  container_type m_storage;

  //! Maximal size
  size_type m_max_size;

  /* Iterator to the virtual first element of the buffer
   *
   * Note that this is also the virtual past-the end element of this
   * (cyclic) buffer
   * */
  iterator m_first;
};

//
// ---------------------------------------------------------------
//

template <typename T, typename Container>
void CircularBuffer<T, Container>::push_front(value_type val) {
  assert_dbg(max_size() != 0, ExcInvalidState("max_size is zero"));

  if (m_storage.size() == m_max_size) {
    // Maximal size reached:
    // Decrement pointer (circularly) and change value
    --m_first;
    *m_first = std::move(val);
    return;
  }

  // Else we actually add an element:
  size_t ssize = m_storage.size();
  if (ssize == 0) {
    // Just push front and update iterator
    m_storage.push_front(std::move(val));
    m_first = circular_begin(m_storage, 0);
  } else {
    // Push and update m_first circular iterator.
    auto first_iterator = m_storage.insert(m_first.position(), std::move(val));
    m_first = circular_begin(m_storage, first_iterator);
  }

  // Check that we inserted something
  assert_dbg(ssize + 1 == m_storage.size(), ExcInternalError());
}

template <typename T, typename Container>
void CircularBuffer<T, Container>::push_back(value_type val) {
  assert_dbg(max_size() != 0, ExcInvalidState("max_size is zero"));

  if (m_storage.size() == m_max_size) {
    // Maximal size reached:
    // Change value and increment first pointer (circularly)
    // Here we use the fact that the past-the-end iterator in this circular
    // buffer is equvalant to m_first.
    *m_first = std::move(val);
    ++m_first;
    return;
  }

  size_t ssize = m_storage.size();
  if (ssize == 0 || m_first.position() == std::begin(m_storage)) {
    // if we are at the actual front, insert at back position
    m_storage.push_back(std::move(val));
    m_first = circular_begin(m_storage, 0);
  } else {
    // Else insert before the first element, i.e. circularly at the back
    m_storage.insert(m_first.position(), std::move(val));
    // Update circular iterator
    m_first = circular_begin(m_storage, m_first.position());
  }

  // Check that we inserted something
  assert_dbg(ssize + 1 == m_storage.size(), ExcInternalError());
}

template <typename T, typename Container>
void CircularBuffer<T, Container>::max_size(size_type msize) {
  m_max_size = msize;

  if (msize == 0) {
    // Clear all elements of the buffer:
    m_storage.clear();
    m_first = circular_begin(m_storage, 0);
  } else if (msize < m_storage.size()) {
    // We have to bin some elements of the buffer
    typedef typename container_type::iterator cont_iter;

    // The element after the one to be deleted is
    // the element past the last one in the new array.
    // In the circular sense, this is m_first
    iterator end_remove_range = m_first;

    // The first element we want to delete is the one that does
    // not fit inside the msize and is located the furthest away from the start.
    // This is the same as first advanced by msize times.
    iterator begin_remove_range = std::next(m_first, msize);

    // Delete elements at the back of the container.
    //
    // The idea is to first find the end of the first deletetion operaton,
    // by checking whatever comes first: end_remove_range or
    // std::end(m_storage)
    cont_iter del_begin = begin_remove_range.position();
    cont_iter del_end = del_begin;
    for (; del_end != std::end(m_storage) &&
           del_end != end_remove_range.position();
         ++del_end) {
      // Update the begin_remove_range,
      // such that we are prepared for the next step
      ++begin_remove_range;
    }
    m_storage.erase(del_begin, del_end);

    // Delete remaining elements which sit at the front
    del_begin = begin_remove_range.position();
    del_end = end_remove_range.position();
    m_storage.erase(del_begin, del_end);

    // Update m_first:
    m_first = circular_begin(m_storage, m_first.position());
  }
}

}  // namespace krims
