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

#include <krims/TypeUtils.hh>

namespace krims {
namespace tests {
// IsComplexNumber
static_assert(IsComplexNumber<std::complex<double>>::value, "Test failed");
static_assert(IsComplexNumber<std::complex<float>>::value, "Test failed");
static_assert(!IsComplexNumber<double>::value, "Test failed");
static_assert(!IsComplexNumber<float>::value, "Test failed");
static_assert(!IsComplexNumber<int>::value, "Test failed");

// RealTypeOf
static_assert(std::is_same<typename RealTypeOf<std::complex<double>>::type,
                           double>::value,
              "Test failed");
static_assert(std::is_same<typename RealTypeOf<std::complex<float>>::type,
                           float>::value,
              "Test failed");
static_assert(std::is_same<typename RealTypeOf<double>::type, double>::value,
              "Test failed");
static_assert(std::is_same<typename RealTypeOf<float>::type, float>::value,
              "Test failed");
static_assert(std::is_same<typename RealTypeOf<int>::type, int>::value,
              "Test failed");

}  // namespace tests
}  // namespace krims
