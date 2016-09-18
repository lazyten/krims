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
#include "exception_defs.hh"
#include "krims/TypeUtils.hh"

// Note: no krims namespace, since these are all macros anyway

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
#define assert_range(start, number, end)                                 \
  {                                                                      \
    assert_dbg((start <= number) && (number < end),                      \
               ::krims::ExcOutsideRange<decltype(start + number + end)>( \
                     number, start, end));                               \
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
#define assert_greater_equal(lhs, rhs)                               \
  {                                                                  \
    assert_dbg(lhs <= rhs,                                           \
               ::krims::ExcTooLarge<decltype(lhs + rhs)>(lhs, rhs)); \
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
#define assert_greater(lhs, rhs)                                            \
  {                                                                         \
    assert_dbg(lhs < rhs,                                                   \
               ::krims::ExcTooLargeOrEqual<decltype(lhs + rhs)>(lhs, rhs)); \
  }

/**
 * Uses assert_dbg in order to check that a rhs is exactly
 * equal to a lhs.
 *
 * Takes the following arguments
 * <ol>
 * <li> lhs number
 * <li> rhs number
 * </ol>
 */
#define assert_equal(lhs, rhs)                                       \
  {                                                                  \
    assert_dbg(lhs == rhs,                                           \
               ::krims::ExcNotEqual<decltype(lhs + rhs)>(lhs, rhs)); \
  }

/**
 * Assert whether two sizes match
 */
#define assert_size(lhs, rhs) \
  { assert_dbg(lhs == rhs, ::krims::ExcSizeMismatch(lhs, rhs)); }

/**
 * Assert whether all elements of the container cont have
 * the size
 * vsize
 */
#define assert_element_sizes(cont, vsize)                          \
  {                                                                \
    for (auto it = std::begin(cont); it != std::end(cont); ++it) { \
      assert_size(vsize, it->size());                              \
    }                                                              \
  }

namespace krims {
template <typename T>
bool isfinite(T t) {
  return std::isfinite(t);
}

template <typename T>
bool isfinite(std::complex<T> t) {
  return std::isfinite(t.real()) && std::isfinite(t.imag());
}
}  // krims

/** Assert that a value is finite */
#define assert_finite(value) \
  { assert_dbg(::krims::isfinite(value), ::krims::ExcNumberNotFinite(value)) }

/** Assert that a value is non-zero */
#define assert_nonzero(value) \
  { assert_dbg(value != static_cast<decltype(value)>(0), ::krims::ExcZero()) }
