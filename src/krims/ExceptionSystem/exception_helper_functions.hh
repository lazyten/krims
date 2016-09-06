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
#include "Exceptions.hh"
#include <type_traits>

// Note: These functions are deliberately chosen to go into the global
//       namespace, such that they are easily available from everywhere.

/**
 * Uses assert_dbg in order to check that a number is within a range,
 * if not raise an exception.
 *
 * The macro takes the following arguments:
 * <ol>
 * <li> The lower bound
 * <li> The number to check
 * <li> The upper bound plus one
 * </ol>
 */
template <typename T, typename U, typename V>
inline void assert_range(T start, U number, V end) {
#ifdef DEBUG
  typedef typename std::common_type<T, U, V>::type ct;
  assert_dbg((start <= number) && (number < end),
             ::krims::ExcOutsideRange<ct>(number, start, end));
#endif
}

/**
 * Uses assert_dbg in order to check that a rhs is greater or equal
 * to a lhs
 *
 * Takes the following arguments
 * <ol>
 * <li> lhs number
 * <li> rhs number
 * </ol>
 */
template <typename T, typename U>
inline void assert_greater_equal(T lhs, U rhs) {
#ifdef DEBUG
  typedef typename std::common_type<T, U>::type ct;
  assert_dbg(lhs <= rhs, ::krims::ExcTooLarge<ct>(lhs, rhs));
#endif
}

/**
 * Uses assert_dbg in order to check that a rhs is strictly greater to
 * a lhs
 *
 * Takes the following arguments
 * <ol>
 * <li> lhs number
 * <li> rhs number
 * </ol>
 */
template <typename T, typename U>
inline void assert_greater(T lhs, U rhs) {
#ifdef DEBUG
  typedef typename std::common_type<T, U>::type ct;
  assert_dbg(lhs < rhs, ::krims::ExcTooLargeOrEqual<ct>(lhs, rhs));
#endif
}

/**
 * Uses assert_dbg in order to check that a rhs is exactly equal to a lhs.
 *
 * Takes the following arguments
 * <ol>
 * <li> lhs number
 * <li> rhs number
 * </ol>
 */
template <typename T, typename U>
inline void assert_equal(T lhs, U rhs) {
#ifdef DEBUG
  typedef typename std::common_type<T, U>::type ct;
  assert_dbg(lhs == rhs, ::krims::ExcNotEqual<ct>(lhs, rhs));
#endif
}

/**
 * Assert whether two sizes match
 */
inline void assert_size(size_t size1, size_t size2) {
  assert_dbg(size1 == size2, ::krims::ExcSizeMismatch(size1, size2));
}

/**
 * Assert whether all elements of the container cont have the size
 * vsize
 */
template <typename Container>
inline void assert_element_sizes(const Container &cont, size_t vsize) {
#ifdef DEBUG
  for (auto it = std::begin(cont); it != std::end(cont); ++it) {
    assert_size(it->size(), vsize);
  }
#endif
}

template <typename T>
inline void assert_finite(T t) {
  assert_dbg(std::isfinite(t), ::krims::ExcNumberNotFinite(t));
}
