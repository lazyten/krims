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
#include <cstring>

// TODO move all functionality which has to do with the backtrace here
// also change it such that it operates on a pre-allocated piece of memory
// only and only uses c-like functions. This way we can even invoke it
// if we are catching a SIGSEV, see
// https://github.com/vmarkovtsev/DeathHandler
// for ideas.

namespace krims {

// TODO remove KRIMS_ENABLE_EXPERIMENTAL after some while
#if defined DEBUG && defined KRIMS_HAVE_GLIBC_STACKTRACE && \
      defined KRIMS_ENABLE_EXPERIMENTAL
// It only makes sense to define addr2line if we actually can make use of its
// functionality, i.e. if we have a glibc stacktrace and are in DEBUG mode
// (Debug symbols)

/** Get the line of a file from the incoming address string
 *
 * \param execname   Path to the executable
 * \param addr       Address string
 * \param maxlen     The maximal length any of the strings may have
 * \param codefile   The file name string which contains the address
 * \param number     The line number in codefile which contains the address
 *
 * \returns -1 on any error, else 0
 *
 * This function is inspired by
 * https://github.com/vmarkovtsev/DeathHandler/blob/master/death_handler.cc
 */
int addr2line(const char* execname, const char* addr, const size_t maxlen,
              char* codefile, char* number);
#define KRIMS_ADDR2LINE_AVAILABLE

#endif
}  // namespace krims
