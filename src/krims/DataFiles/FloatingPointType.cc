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

#include "FloatingPointType.hh"
#include "krims/ExceptionSystem.hh"
#include <limits>
#include <type_traits>

namespace krims {

template <typename T, bool flt = std::is_floating_point<T>::value>
struct GuessType {
  FloatingPointType operator()() { return FloatingPointType::INVALID; }
};

template <>
struct GuessType<double, true> {
  FloatingPointType operator()() { return FloatingPointType::BINARY64; }
};

template <>
struct GuessType<float, true> {
  FloatingPointType operator()() { return FloatingPointType::BINARY32; }
};

template <>
struct GuessType<long double, true> {
  FloatingPointType operator()() {
    typedef std::numeric_limits<long double> lim;

    if (lim::min_exponent == -16381 && lim::max_exponent == 16384 && lim::digits == 64) {
      return FloatingPointType::BINARY80;
    } else {
      assert_implemented(false);
      return FloatingPointType::INVALID;
    }
  }
};

template <typename T>
FloatingPointType floating_point_type_of() {
  return GuessType<T>{}();
}

#define INSTANTIATE(type) template FloatingPointType floating_point_type_of<type>()
INSTANTIATE(long double);
INSTANTIATE(double);
INSTANTIATE(float);

INSTANTIATE(int8_t);
INSTANTIATE(uint8_t);
INSTANTIATE(int16_t);
INSTANTIATE(uint16_t);
INSTANTIATE(int32_t);
INSTANTIATE(uint32_t);
INSTANTIATE(int64_t);
INSTANTIATE(uint64_t);
#undef INSTANTIATE

}  // namespace krims
