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
#include <sstream>

namespace krims {

/** \brief Split the string s at every occurrence of the delimiter delim.
 *
 * Store the resulting substrings using the iterator out.
 *
 * \note Neither trailling, nor leading separator characters, nor multiple
 * separators are ignored and give rise to empty strings. In other words
 * ".a..b." will split to ["","a","","b",""] if passed to this function
 * with delim=='.'.
 *
 * \return The iterator after the last insertion has happened.
 */
template <typename String, typename Iterator>
Iterator split(const String& s, Iterator out,
               typename String::traits_type::char_type delim = '\n') {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) *(out++) = item;
  if (s.back() == delim) *(out++) = "";
  return out;
}

}  // namespace krims
