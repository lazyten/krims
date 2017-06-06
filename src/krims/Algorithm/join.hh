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
#include <iterator>
#include <sstream>

namespace krims {

/** \brief Join the values of the iterator range using a provided separator
 *
 * The values are converted to strings using stringstreams and the operator<<.
 * Between each two values the separator sequence is inserted. The final
 * element is not followed by the separator sequence.
 *
 * E.g. the vector { "a" , "b" } joined with separator "--" yields the string
 * "a--b".
 *
 * \param begin   The start of the range
 * \param end     The end of the range (as usual exclusive)
 * \param sep     The separator string to use. If this is absent, no separation
 *                is inserted.
 * */
template <typename String, typename Iterator>
String join(Iterator begin, const Iterator end, const String& sep) {
  using char_type      = typename String::value_type;
  using traits_type    = typename String::traits_type;
  using allocator_type = typename String::allocator_type;
  using stream_type    = std::basic_ostringstream<char_type, traits_type, allocator_type>;

  if (begin == end) return String{};

  stream_type res;
  res << *begin;
  ++begin;
  for (; begin != end; ++begin) {
    res << sep << *begin;
  }
  return res.str();
}

template <typename charT, typename Iterator>
std::basic_string<charT> join(Iterator begin, const Iterator end, const charT* sep) {
  return join(begin, end, std::basic_string<charT>(sep));
}

}  // namespace krims
