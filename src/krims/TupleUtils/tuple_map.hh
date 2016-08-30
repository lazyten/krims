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

#ifdef KRIMS_HAVE_CXX14
//
// C++14 is available
//
namespace detail {
template <typename UnOp, typename Tuple, std::size_t... Indices>
constexpr auto tuple_map_impl(UnOp&& op, Tuple&& t,
                              std::index_sequence<Indices...>) {
  return std::make_tuple(op(std::get<Indices>(std::forward<Tuple>(t)))...);
}

template <typename BinOp, typename Tuple1, typename Tuple2,
          std::size_t... Indices>
constexpr auto tuple_map_impl(BinOp&& op, Tuple1&& t1, Tuple2&& t2,
                              std::index_sequence<Indices...>) {
  return std::make_tuple(op(std::get<Indices>(std::forward<Tuple1>(t1)),
                            std::get<Indices>(std::forward<Tuple2>(t2)))...);
}

}  // namespace detail

/** \brief Map a unary operator to all elements of a tuple  in turn.
 *
 * Return the tuple of all results.
 *
 *  Of cause the operator() of the UnOp should be generic in
 *  all types occurring.
 *
 * \note The operator has to return a non-void object, otherwise this
 * implementation fails. Use tuple_for_each for unary operators
 * returning void.
 *  */
template <typename UnOp, typename Tuple>
constexpr auto tuple_map(UnOp&& op, Tuple&& t) {
  return detail::tuple_map_impl(
        std::forward<UnOp>(op), std::forward<Tuple>(t),
        std::make_index_sequence<
              std::tuple_size<std::decay_t<Tuple>>::value>());
}

/** Map a binary operator to all elements of two tuples in turn.
 *
 * Return the tuple of all results.
 *
 *  Of cause the apply function of the operator should be generic in
 *  all pairs of types occurring.
 *
 * \note The operator has to return a non-void object, otherwise this
 * implementation fails. Use for_each for operators
 * returning void.
 *  */
template <typename BinOp, typename Tuple1, typename Tuple2>
constexpr auto tuple_map(BinOp&& op, Tuple1&& t1, Tuple2&& t2) {
  constexpr size_t size1 = std::tuple_size<std::decay_t<Tuple1>>::value;
  constexpr size_t size2 = std::tuple_size<std::decay_t<Tuple2>>::value;

  static_assert(size1 == size2,
                "Both tuples to map for the case of binary operations "
                "need to be of the same size.");

  return detail::tuple_map_impl(
        std::forward<BinOp>(op), std::forward<Tuple1>(t1),
        std::forward<Tuple2>(t2), std::make_index_sequence<size1>());
}

#else
//
// Only C++11 is available
//
// only provide explicit implementations for the cases of 0 to 4 elements

//@{
/** \brief Map a unary operator to all elements of a tuple  in turn.
 *
 * Return the tuple of all results.
 *
 *  Of cause the operator() of the UnOp should be generic in
 *  all types occurring.
 *
 * \note The operator has to return a non-void object, otherwise this
 * implementation fails. Use tuple_for_each for unary operators
 * returning void.
 *  */
template <typename UnOp>
constexpr std::tuple<> tuple_map(UnOp&&, std::tuple<>) {
  return std::tuple<>{};
}

//
// Unary -- normal references
//
template <typename UnOp, typename E0>
constexpr auto tuple_map(UnOp&& op, std::tuple<E0>& t)
      -> decltype(std::make_tuple(op(std::get<0>(t)))) {
  return std::make_tuple(op(std::get<0>(t)));
}

template <typename UnOp, typename E0, typename E1>
constexpr auto tuple_map(UnOp&& op, std::tuple<E0, E1>& t)
      -> decltype(std::make_tuple(op(std::get<0>(t)), op(std::get<1>(t)))) {
  return std::make_tuple(op(std::get<0>(t)), op(std::get<1>(t)));
}

template <typename UnOp, typename E0, typename E1, typename E2>
constexpr auto tuple_map(UnOp&& op, std::tuple<E0, E1, E2>& t)
      -> decltype(std::make_tuple(op(std::get<0>(t)), op(std::get<1>(t)),
                                  op(std::get<2>(t)))) {
  return std::make_tuple(op(std::get<0>(t)), op(std::get<1>(t)),
                         op(std::get<2>(t)));
}

template <typename UnOp, typename E0, typename E1, typename E2, typename E3>
constexpr auto tuple_map(UnOp&& op, std::tuple<E0, E1, E2, E3>& t)
      -> decltype(std::make_tuple(op(std::get<0>(t)), op(std::get<1>(t)),
                                  op(std::get<2>(t)), op(std::get<3>(t)))) {
  return std::make_tuple(op(std::get<0>(t)), op(std::get<1>(t)),
                         op(std::get<2>(t)), op(std::get<3>(t)));
}

//
// Unary -- const references
//
template <typename UnOp, typename E0>
constexpr auto tuple_map(UnOp&& op, const std::tuple<E0>& t)
      -> decltype(std::make_tuple(op(std::get<0>(t)))) {
  return std::make_tuple(op(std::get<0>(t)));
}

template <typename UnOp, typename E0, typename E1>
constexpr auto tuple_map(UnOp&& op, const std::tuple<E0, E1>& t)
      -> decltype(std::make_tuple(op(std::get<0>(t)), op(std::get<1>(t)))) {
  return std::make_tuple(op(std::get<0>(t)), op(std::get<1>(t)));
}

template <typename UnOp, typename E0, typename E1, typename E2>
constexpr auto tuple_map(UnOp&& op, const std::tuple<E0, E1, E2>& t)
      -> decltype(std::make_tuple(op(std::get<0>(t)), op(std::get<1>(t)),
                                  op(std::get<2>(t)))) {
  return std::make_tuple(op(std::get<0>(t)), op(std::get<1>(t)),
                         op(std::get<2>(t)));
}

template <typename UnOp, typename E0, typename E1, typename E2, typename E3>
constexpr auto tuple_map(UnOp&& op, const std::tuple<E0, E1, E2, E3>& t)
      -> decltype(std::make_tuple(op(std::get<0>(t)), op(std::get<1>(t)),
                                  op(std::get<2>(t)), op(std::get<3>(t)))) {
  return std::make_tuple(op(std::get<0>(t)), op(std::get<1>(t)),
                         op(std::get<2>(t)), op(std::get<3>(t)));
}
//@}

//@{
/** Map a binary operator to all elements of two tuples in turn.
 *
 * Return the tuple of all results.
 *
 *  Of cause the apply function of the operator should be generic in
 *  all pairs of types occurring.
 *
 * \note The operator has to return a non-void object, otherwise this
 * implementation fails. Use for_each for operators
 * returning void.
 *  */
template <typename BinOp>
constexpr std::tuple<> tuple_map(BinOp&&, std::tuple<>, std::tuple<>) {
  return std::tuple<>{};
}

//
// Binary -- normal references
//
template <typename BinOp, typename E0, typename T0>
constexpr auto tuple_map(BinOp&& bop, std::tuple<E0>& ee, std::tuple<T0>& tt)
      -> decltype(std::make_tuple(bop(std::get<0>(ee), std::get<0>(tt)))) {
  return std::make_tuple(bop(std::get<0>(ee), std::get<0>(tt)));
}

template <typename BinOp, typename E0, typename E1, typename T0, typename T1>
constexpr auto tuple_map(BinOp&& bop, std::tuple<E0, E1>& ee,
                         std::tuple<T0, T1>& tt)
      -> decltype(std::make_tuple(bop(std::get<0>(ee), std::get<0>(tt)),
                                  bop(std::get<1>(ee), std::get<1>(tt)))) {
  return std::make_tuple(bop(std::get<0>(ee), std::get<0>(tt)),
                         bop(std::get<1>(ee), std::get<1>(tt)));
}

template <typename BinOp, typename E0, typename E1, typename E2, typename T0,
          typename T1, typename T2>
constexpr auto tuple_map(BinOp&& bop, std::tuple<E0, E1, E2>& ee,
                         std::tuple<T0, T1, T2>& tt)
      -> decltype(std::make_tuple(bop(std::get<0>(ee), std::get<0>(tt)),
                                  bop(std::get<1>(ee), std::get<1>(tt)),
                                  bop(std::get<2>(ee), std::get<2>(tt)))) {
  return std::make_tuple(bop(std::get<0>(ee), std::get<0>(tt)),
                         bop(std::get<1>(ee), std::get<1>(tt)),
                         bop(std::get<2>(ee), std::get<2>(tt)));
}

template <typename BinOp, typename E0, typename E1, typename E2, typename E3,
          typename T0, typename T1, typename T2, typename T3>
constexpr auto tuple_map(BinOp&& bop, std::tuple<E0, E1, E2, E3>& ee,
                         std::tuple<T0, T1, T2, T3>& tt)
      -> decltype(std::make_tuple(bop(std::get<0>(ee), std::get<0>(tt)),
                                  bop(std::get<1>(ee), std::get<1>(tt)),
                                  bop(std::get<2>(ee), std::get<2>(tt)),
                                  bop(std::get<3>(ee), std::get<3>(tt)))) {
  return std::make_tuple(bop(std::get<0>(ee), std::get<0>(tt)),
                         bop(std::get<1>(ee), std::get<1>(tt)),
                         bop(std::get<2>(ee), std::get<2>(tt)),
                         bop(std::get<3>(ee), std::get<3>(tt)));
}

//
// Binary -- const references
//
template <typename BinOp, typename E0, typename T0>
constexpr auto tuple_map(BinOp&& bop, const std::tuple<E0>& ee,
                         const std::tuple<T0>& tt)
      -> decltype(std::make_tuple(bop(std::get<0>(ee), std::get<0>(tt)))) {
  return std::make_tuple(bop(std::get<0>(ee), std::get<0>(tt)));
}

template <typename BinOp, typename E0, typename E1, typename T0, typename T1>
constexpr auto tuple_map(BinOp&& bop, const std::tuple<E0, E1>& ee,
                         const std::tuple<T0, T1>& tt)
      -> decltype(std::make_tuple(bop(std::get<0>(ee), std::get<0>(tt)),
                                  bop(std::get<1>(ee), std::get<1>(tt)))) {
  return std::make_tuple(bop(std::get<0>(ee), std::get<0>(tt)),
                         bop(std::get<1>(ee), std::get<1>(tt)));
}

template <typename BinOp, typename E0, typename E1, typename E2, typename T0,
          typename T1, typename T2>
constexpr auto tuple_map(BinOp&& bop, const std::tuple<E0, E1, E2>& ee,
                         const std::tuple<T0, T1, T2>& tt)
      -> decltype(std::make_tuple(bop(std::get<0>(ee), std::get<0>(tt)),
                                  bop(std::get<1>(ee), std::get<1>(tt)),
                                  bop(std::get<2>(ee), std::get<2>(tt)))) {
  return std::make_tuple(bop(std::get<0>(ee), std::get<0>(tt)),
                         bop(std::get<1>(ee), std::get<1>(tt)),
                         bop(std::get<2>(ee), std::get<2>(tt)));
}

template <typename BinOp, typename E0, typename E1, typename E2, typename E3,
          typename T0, typename T1, typename T2, typename T3>
constexpr auto tuple_map(BinOp&& bop, const std::tuple<E0, E1, E2, E3>& ee,
                         const std::tuple<T0, T1, T2, T3>& tt)
      -> decltype(std::make_tuple(bop(std::get<0>(ee), std::get<0>(tt)),
                                  bop(std::get<1>(ee), std::get<1>(tt)),
                                  bop(std::get<2>(ee), std::get<2>(tt)),
                                  bop(std::get<3>(ee), std::get<3>(tt)))) {
  return std::make_tuple(bop(std::get<0>(ee), std::get<0>(tt)),
                         bop(std::get<1>(ee), std::get<1>(tt)),
                         bop(std::get<2>(ee), std::get<2>(tt)),
                         bop(std::get<3>(ee), std::get<3>(tt)));
}
//@}
#endif

}  // namespace krims
