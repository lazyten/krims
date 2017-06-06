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

#include "ieee_convert.hh"
#include "krims/ExceptionSystem.hh"
#include <algorithm>
#include <cstdint>

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 6)
#define FLOAT128 __float128
#define FLOAT80 __float80
#endif

// Useful resources:
//     https://en.wikipedia.org/wiki/Double-precision_floating-point_format
//     https://en.wikipedia.org/wiki/Quadruple-precision_floating-point_format
//     https://en.wikipedia.org/wiki/Extended_precision#x86_Extended_Precision_Format

namespace krims {

void convert_b128_to_f80(const char* b128, char* b80) {
#if defined FLOAT128 and defined FLOAT80
  // gcc understands __float128 => use it
  const FLOAT128* in = reinterpret_cast<const FLOAT128*>(b128);
  FLOAT80* out       = reinterpret_cast<FLOAT80*>(b80);
  *out               = static_cast<FLOAT80>(*in);
#else
  // Address the mantissa of the output float in extended 80bit
  // precision (The first 63 bits) as an 64bit integer
  uint64_t& b80_mantissa = *reinterpret_cast<uint64_t*>(b80);

  // Address the sign and exponent of the b128 and b80
  // integers (coincidently in both cases the last 2 bytes
  // in exactly the same shape and meaning of the bytes)
  uint16_t& b80_exp        = *reinterpret_cast<uint16_t*>(b80 + 8);
  const uint16_t& b128_exp = *reinterpret_cast<const uint16_t*>(b128 + 14);
  b80_exp                  = b128_exp;

  // Copy the 64 most significant bits of the mantissa over
  //     (This means we need to start at bit 48 and go up to bit
  //      111 == 64 bits. Note that we assume little endian here.)
  std::copy(b128 + 6, b128 + 14, b80);

  // Get rid of the least significant bit
  //  ( Extended precision only has *63* mantissa bits)
  b80_mantissa >>= 1;

  // A number is +0 or -0 if all bits are zero, except possibly
  // the sign bit.
  const uint64_t& b128_part1 = *reinterpret_cast<const uint64_t*>(b128);
  const uint64_t& b128_part2 = *reinterpret_cast<const uint64_t*>(b128 + 8);
  const bool is_zero = b128_part1 == 0 && (b128_part2 & ~(uint64_t(1) << 63)) == 0;

  // Set the integer part bit
  //    (Due to the usual normalisation for IEEE floating points
  //     this bit is 1 except in the case where we want to represent
  //     an actual 0, then it is 0, too)
  if (!is_zero) b80_mantissa |= (uint64_t(1) << 63);

// TODO This code does not treat IEEE rounding in the requested way
//      Most notably it does *not* get the closest float, but a float
//      nearby.
//
// TODO What happens if number is b128 are too small to be represented
//      if b80??
#endif  // FLOAT128
}

void ieee_convert(std::pair<FloatingPointType, const char*> in,
                  std::pair<FloatingPointType, char*> out) {
  assert_implemented(in.first == FloatingPointType::BINARY128 &&
                     out.first == FloatingPointType::BINARY80);
  convert_b128_to_f80(in.second, out.second);
}

}  // namespace krims
