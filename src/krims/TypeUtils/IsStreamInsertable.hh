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
#include "VoidType.hh"
#include <iostream>

namespace krims {

//@{
/** \brief struct representing a type (std::true_type, std::false_type) which
 *  indicates whether T can be inserted into an ostream or not.
 */
template <typename T, typename = void>
struct IsStreamInsertable : public std::false_type {};

template <typename T>
struct IsStreamInsertable<T, VoidType<decltype(std::cout << std::declval<T>())>>
      : public std::true_type {};
//@}

}  // namespace krims
