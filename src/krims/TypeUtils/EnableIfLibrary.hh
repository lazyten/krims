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
#include <type_traits>
namespace krims {

/** Enables only if From* can be implicitly converted to To* */
template <typename From, typename To>
using enable_if_ptr_convertible_t =
      typename std::enable_if<std::is_convertible<From*, To*>::value>::type;

/** Enable the method only if the condition is satisfied and T and U are the
  * same type */
template <bool Cond, typename T, typename U>
using enable_if_cond_same_t =
      typename std::enable_if<Cond && std::is_same<T, U>::value>::type;

/** Enable the method only if the condition is satisfied and T can be
  * implicitly converted into U */
template <bool Cond, typename T, typename U>
using enable_if_cond_convertible_t =
      typename std::enable_if<Cond && std::is_convertible<T, U>::value>::type;

/** Convenience using statement */
template <bool Cond, typename T = void>
using enable_if_t = typename std::enable_if<Cond, T>::type;

}  // namespace krims
