//
// Copyright (C) 2016-17 by the krims authors
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
#include <algorithm>
#include <iterator>
#include <numeric>
#include <vector>
namespace krims {

/** Argsort returns the index sequence, which would sort the range of elements provided.
 *
 * Performs an indirect sorting of the range provided by the two iterators.
 * The vectors of indices returned gives the ordering of the iterator range,
 * which would give a sorted range. By the default less is used
 * as the comparator.
 *
 * E.g. It the range runs over the elements { "d", "a", "c", "b" }, then
 * the returned indices would be { 1, 3, 2, 0 } (if less is used as the
 * comparator).
 *
 **/
template <typename RandomAccessIterator, typename Compare>
std::vector<size_t> argsort(const RandomAccessIterator first,
                            const RandomAccessIterator last, Compare cmp) {
  // Initialise index array
  std::vector<size_t> indices(last - first);
  std::iota(std::begin(indices), std::end(indices), 0);

  std::sort(std::begin(indices), std::end(indices), [&first, &cmp](size_t i1, size_t i2) {
    return cmp(*(first + i1), *(first + i2));
  });

  return indices;
}

/** Sort a range using the argsort function with the default comparator (less). */
template <typename RandomAccessIterator>
std::vector<size_t> argsort(const RandomAccessIterator first,
                            const RandomAccessIterator last) {
  return argsort(first, last, std::less<decltype(*first)>{});
}
}  // krims
