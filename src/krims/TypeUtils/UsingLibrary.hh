//
// Copyright (C) 2017 by the krims authors
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
#include "krims/config.hh"
#include <type_traits>

namespace krims {
#ifdef KRIMS_HAVE_CXX14

using std::conditional_t;
using std::common_type_t;
using std::decay_t;

using std::remove_reference_t;
using std::add_rvalue_reference_t;
using std::add_lvalue_reference_t;

using std::remove_cv_t;
using std::remove_const_t;
using std::remove_volatile_t;
using std::add_cv_t;
using std::add_const_t;
using std::add_volatile_t;

using std::make_signed_t;
using std::make_unsigned_t;

#else
#define USING_ALIAS(CLASS) \
  template <typename T>    \
  using CLASS##_t = typename std::CLASS<T>::type

template <bool B, class T, class F>
using conditional_t = typename std::conditional<B, T, F>::type;

template <typename... Ts>
using common_type_t = typename std::common_type<Ts...>::type;

USING_ALIAS(decay);

USING_ALIAS(remove_reference);
USING_ALIAS(add_rvalue_reference);
USING_ALIAS(add_lvalue_reference);

USING_ALIAS(remove_cv);
USING_ALIAS(remove_const);
USING_ALIAS(remove_volatile);
USING_ALIAS(add_cv);
USING_ALIAS(add_const);
USING_ALIAS(add_volatile);

USING_ALIAS(make_signed);
USING_ALIAS(make_unsigned);

#undef USING_ALIAS
#endif  // KRIMS_HAVE_CXX14
}  // namespace krims
