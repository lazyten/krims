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
#include "tuple_for_each.hh"

namespace krims {

namespace detail {
/** Functor which checks whether a value satisfies the predicate Pred and if it
 * does calls the operation Op on it. It keeps an internal state such that
 * only for the first matching call (ie the first time Pred is satisfied)
 * anything
 * is done. Otherwise the function just returns false.
 *
 * operator() returns a bool which indicates whether it makes sense to check the
 * next value or not. In other words it returns true until Pred was true for the
 * first time, then it returns false.
 * */
template <typename Pred, typename Op>
class OpForFirst {
public:
  OpForFirst(Pred&& pred, Op&& op)
        : m_op(std::forward<Op>(op)), m_pred(std::forward<Pred>(pred)) {}

  /** See documentation of the class for more details */
  template <typename T>
  bool operator()(T&& t) {
    if (done) return false;

    if (m_pred(t)) {
      done = true;
      m_op(std::forward<T>(t));
      return false;
    }
    return true;
  }

private:
  bool done = false;
  Pred m_pred;
  Op m_op;
};
}  // namespace detail

#ifdef KRIMS_HAVE_CXX17
//
// C++17 is available
//

namespace detail {

template <typename Pred, typename Op, typename Tuple, std::size_t... Indices>
void tuple_for_first_impl(Pred&& pred, Op&& op, Tuple&& t) {
  typedef detail::OpForFirst<Pred, Op> op_t;
  op_t opforfirst(std::forward<Pred>(pred), std::forward<Op>(op));

  // Use binary left fold, results in
  //  opforfirst(get<0>(t) && opforfirst(get<1>(t)) && ...
  //  opforfirst(get<N>(t)))
  (opforfirst(std::get<Indices>(std::forward<Tuple>(t))) && ...);
  // TODO untested code
}
}  // namespace detail

/** \brief Apply the operation op to the first element
 *         in a tuple, which matches a predicate.
 *
 *         Does not apply the operation if no object
 *         matches the predicate
 */
template <typename Pred, typename Op, typename Tuple>
void tuple_for_first(Pred&& pred, Op&& op, Tuple&& t) {
  tuple_for_first_impl(std::forward<Pred>(pred), std::forward<Op>(op),
                       std::forward<Tuple>(tuple));
}

#else
//
// C++11 or C++14 is available
//

/** \brief Apply the operation op to the first element
 *         in a tuple, which matches a predicate.
 *
 *         Does not apply the operation if no object
 *         matches the predicate
 */
template <typename Pred, typename Op, typename Tuple>
constexpr void tuple_for_first(Pred&& pred, Op&& op, Tuple&& t) {
  typedef detail::OpForFirst<Pred, Op> op_t;
  op_t opforfirst(std::forward<Pred>(pred), std::forward<Op>(op));
  tuple_for_each(std::forward<op_t>(opforfirst), std::forward<Tuple>(t));
}
#endif

}  // namespace krims
