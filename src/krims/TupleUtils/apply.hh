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
#include <functional>
#include <tuple>
#include <utility>

namespace krims {

#ifdef KRIMS_HAVE_CXX17
//
// C++17 is available
//

/** Call a function, taking the arguments from a tuple */
template <typename Func, typename Tuple>
constexpr decltype(auto) apply(Func&& f, Tuple&& t) {
  // Use implementation from standard library
  return std::apply(std::forward<Func>(f), std::forward<Tuple>(t));
  // TODO untested code
}

#elif defined KRIMS_HAVE_CXX14
//
// C++14 is available
//
namespace detail {
template <typename Func, std::size_t... Indices, typename Tuple>
constexpr decltype(auto) apply_impl(Func&& f, Tuple&& t,
                                    std::index_sequence<Indices...>) {
  // TODO: Note: This is a worse implementation than the std::apply of
  // c++17, since we do not use the std::invoke, i.e. we expect Func
  // to be a Functor or std::function
  return f(std::get<Indices>(std::forward<Tuple>(t))...);
}
}  // namespace detail

/** Call a functor or a std::function object, taking the arguments from a tuple
 */
template <typename Func, typename Tuple>
constexpr decltype(auto) apply(Func&& f, Tuple&& t) {
  // Construct index sequence from tuple size:
  auto idcs =
        std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>();

  // Call helper implementation above:
  return detail::apply_impl(std::forward<Func>(f), std::forward<Tuple>(t),
                            idcs);
}

#else
//
// Only C++11 is available
//

//
// Reference version
//
//@{
/** Call a functor or std::function object, taking the arguments from a
 * referenced tuple */
template <typename Func, typename E0>
constexpr typename std::result_of<Func(E0)>::type apply(Func&& f,
                                                        std::tuple<E0>& t) {
  return f(std::forward<E0>(std::get<0>(t)));
}

template <typename Func, typename E0, typename E1>
constexpr typename std::result_of<Func(E0, E1)>::type apply(
      Func&& f, std::tuple<E0, E1>& t) {
  return f(std::forward<E0>(std::get<0>(t)), std::forward<E1>(std::get<1>(t)));
}

template <typename Func, typename E0, typename E1, typename E2>
constexpr typename std::result_of<Func(E0, E1, E2)>::type apply(
      Func&& f, std::tuple<E0, E1, E2>& t) {
  return f(std::forward<E0>(std::get<0>(t)), std::forward<E1>(std::get<1>(t)),
           std::forward<E2>(std::get<2>(t)));
}

template <typename Func, typename E0, typename E1, typename E2, typename E3>
constexpr typename std::result_of<Func(E0, E1, E2, E3)>::type apply(
      Func&& f, std::tuple<E0, E1, E2, E3>& t) {
  return f(std::forward<E0>(std::get<0>(t)), std::forward<E1>(std::get<1>(t)),
           std::forward<E2>(std::get<2>(t)), std::forward<E3>(std::get<3>(t)));
}
//@}

//
// Const reference version
//
//@{
/** Call a functor or std::function object, taking the arguments from a const
 * referenced tuple */
template <typename Func, typename E0>
constexpr typename std::result_of<Func(E0)>::type apply(
      Func&& f, const std::tuple<E0>& t) {
  return f(std::forward<E0>(std::get<0>(t)));
}

template <typename Func, typename E0, typename E1>
constexpr typename std::result_of<Func(E0, E1)>::type apply(
      Func&& f, const std::tuple<E0, E1>& t) {
  return f(std::forward<E0>(std::get<0>(t)), std::forward<E1>(std::get<1>(t)));
}

template <typename Func, typename E0, typename E1, typename E2>
constexpr typename std::result_of<Func(E0, E1, E2)>::type apply(
      Func&& f, const std::tuple<E0, E1, E2>& t) {
  return f(std::forward<E0>(std::get<0>(t)), std::forward<E1>(std::get<1>(t)),
           std::forward<E2>(std::get<2>(t)));
}

template <typename Func, typename E0, typename E1, typename E2, typename E3>
constexpr typename std::result_of<Func(E0, E1, E2, E3)>::type apply(
      Func&& f, const std::tuple<E0, E1, E2, E3>& t) {
  return f(std::forward<E0>(std::get<0>(t)), std::forward<E1>(std::get<1>(t)),
           std::forward<E2>(std::get<2>(t)), std::forward<E3>(std::get<3>(t)));
}
//@}

//
// Generic stuff for both
//

/** Call a functor or std::function object with an empty tuple */
template <typename Func>
void apply(Func&& f, const std::tuple<>&) {
  f();
}

/** Call a functor or std::function object, taking the arguments from a tuple
 *
 * \note More than 4 tuple elements is only supported for c++14 and above.
 * */
template <typename Func, typename E0, typename E1, typename E2, typename E3,
          typename... Es>
void apply(Func&&, const std::tuple<E0, E1, E2, E3, Es...>&) {
  static_assert(sizeof...(Es) > 0,
                "If only c++11 is available apply is only implemented up "
                "to a tuple with 4 elements");
}
#endif
}  // namespace krims
