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
#include <complex>
#include <type_traits>

namespace krims {

//@{
/** \brief helper struct to extract the underlying real type from a
 *         potentially complex scalar type.
 *
 * The real type is accessible via the member type "type".
 *  */
template <typename Scalar>
struct RealTypeOf {
  static_assert(std::is_arithmetic<Scalar>::value,
                "RealTypeOf can only operate oni arithmetic types.");
  typedef Scalar type;
};

template <typename Scalar>
struct RealTypeOf<std::complex<Scalar>> {
  typedef Scalar type;
};
//@}

template <typename T>
using RealTypeOfType = typename RealTypeOf<T>::type;
}
