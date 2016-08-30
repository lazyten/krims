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
#include <tuple>

namespace krims {

#ifdef KRIMS_HAVE_CXX17
//
// C++17 is available
//
namespace detail {
template <typename UnOp, typename Tuple, std::size_t... Indices>
constexpr void tuple_for_each_impl(UnOp&& op, Tuple&& t,
                                   std::index_sequence<Indices...>) {
  (op(std::get<Indices>(std::forward<Tuple>(t))), ...);
  // TODO untested code
}
}  // namespace detail

/** \brief Apply a unary operator to all elements of a tuple in turn. */
template <typename UnOp, typename Tuple>
constexpr void tuple_for_each(UnOp&& op, Tuple&& t) {
  return detail::tuple_for_each_impl(
        std::forward<UnOp>(op), std::forward<Tuple>(t),
        std::make_index_sequence<
              std::tuple_size<std::decay_t<Tuple>>::value>());
}

#elif defined KRIMS_HAVE_CXX14
//
// C++14 is available
//
namespace detail {
template <typename UnOp, typename Tuple, std::size_t... Indices>
constexpr void tuple_for_each_impl(UnOp&& op, Tuple&& t,
                                   std::index_sequence<Indices...>) {
  // This is a dirty trick:
  // The parameter expansion gives rise to a comma-separated list of elements
  // hence the next line gives rise to
  //   { (op(get<0>(t),0), (op(get<1>(t),0), ... op(get<N>(t),0) }
  //
  // The comma operator in each element of the list causes the return value
  // of the function to be discarded. Instead we use the next value (int)
  // as the type. Overall we get an initialiser_list<int>
  std::initializer_list<int> l = {
        (op(std::get<Indices>(std::forward<Tuple>(t))), 0)...};

  // Fake-use local variable l
  (void)l;
}
}  // namespace detail

/** \brief Apply a unary operator to all elements of a tuple in turn. */
template <typename UnOp, typename Tuple>
constexpr void tuple_for_each(UnOp&& op, Tuple&& t) {
  return detail::tuple_for_each_impl(
        std::forward<UnOp>(op), std::forward<Tuple>(t),
        std::make_index_sequence<
              std::tuple_size<std::decay_t<Tuple>>::value>());
}

#else
//
// Only C++11 is available
//

//@{
/** \brief Apply a unary operator to all elements of a tuple in turn. */
template <typename UnOp>
constexpr void tuple_for_each(UnOp&&, std::tuple<>) {}

template <typename UnOp, typename E>
constexpr void tuple_for_each(UnOp&& op, std::tuple<E>& t) {
  op(std::get<0>(t));
}

template <typename UnOp, typename E0, typename E1>
constexpr void tuple_for_each(UnOp&& op, std::tuple<E0, E1>& t) {
  op(std::get<0>(t));
  op(std::get<1>(t));
}

template <typename UnOp, typename E0, typename E1, typename E2>
constexpr void tuple_for_each(UnOp&& op, std::tuple<E0, E1, E2>& t) {
  op(std::get<0>(t));
  op(std::get<1>(t));
  op(std::get<2>(t));
}

template <typename UnOp, typename E0, typename E1, typename E2, typename E3>
constexpr void tuple_for_each(UnOp&& op, std::tuple<E0, E1, E2, E3>& t) {
  op(std::get<0>(t));
  op(std::get<1>(t));
  op(std::get<2>(t));
  op(std::get<3>(t));
}

template <typename UnOp, typename E>
constexpr void tuple_for_each(UnOp&& op, const std::tuple<E>& t) {
  op(std::get<0>(t));
}

template <typename UnOp, typename E0, typename E1>
constexpr void tuple_for_each(UnOp&& op, const std::tuple<E0, E1>& t) {
  op(std::get<0>(t));
  op(std::get<1>(t));
}

template <typename UnOp, typename E0, typename E1, typename E2>
constexpr void tuple_for_each(UnOp&& op, const std::tuple<E0, E1, E2>& t) {
  op(std::get<0>(t));
  op(std::get<1>(t));
  op(std::get<2>(t));
}

template <typename UnOp, typename E0, typename E1, typename E2, typename E3>
constexpr void tuple_for_each(UnOp&& op, const std::tuple<E0, E1, E2, E3>& t) {
  op(std::get<0>(t));
  op(std::get<1>(t));
  op(std::get<2>(t));
  op(std::get<3>(t));
}
//@}
#endif

}  // namespace krims
