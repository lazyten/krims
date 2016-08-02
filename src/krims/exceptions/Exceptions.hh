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
#include "exception_defs.hh"
#include <complex>

namespace krims {
namespace exceptions {

//
// Numerics
//
/**
 * Exception denoting a division by zero.
 */
DefExceptionMsg(ExcDevideByZero, "Devision by zero encountered.");

/**
 * Exception denoting that a NaN or plus or minus infinity was encountered.
 * Use ExcDevideByZero to indicate division by zero and use ExcZero to denote
 * an unexpected zero
 *
 * The argument is of type std::complex<double>, so any real or complex scalar
 * value can be passed.
 */
DefException1(ExcNumberNotFinite, std::complex<double>,
              << "Encoutered a non-finite number, where this was not expected."
                 "(its value is"
              << arg1 << ").");

/**
* A number is zero, but it should not be here.
*/
DefExceptionMsg(ExcZero, "Encountered a zero, where this does not make sense.");

/** An overflow was detected. */
DefExceptionMsg(ExcOverflow, "Overflow detected.");

/** An underflow was detected. */
DefExceptionMsg(ExcUnderflow, "Underflow detected.");

//
// Range and size checking
//
/**
 * The sizes of two objects are assumed to be equal, but they were not.
 */
DefException2(ExcSizeMismatch, size_t, size_t, << "Size " << arg1
                                               << " not equal to " << arg2);

/**
 * Exception to indicate that a number is not within the expected range.
 * The constructor takes three <tt>size_t</tt> arguments
 *
 * <ol>
 * <li> the violating index
 * <li> the lower bound
 * <li> the upper bound plus one
 * </ol>
 */
template <typename T>
DefException3(ExcOutsideRange, T, T, T, << "Index " << arg1
                                        << " is not in the half-open interval ["
                                        << arg2 << "," << arg3 << ").");

/**
 * Exception to indicate that a number is larger than an upper bound.
 * The intention is that arg1 <= arg2 should have been satisfied.
 */
template <typename T>
DefException2(ExcTooLarge, T, T, << "Number " << arg1
                                 << " must be smaller or equal to " << arg2
                                 << ".");

/**
 * Exception to indicate that a number is larger or equal to an upper bound
 * The intention is that arg1 < arg2 should have been satisfied.
 */
template <typename T>
DefException2(ExcTooLargeOrEqual, T, T,
              << "Number " << arg1 << " must be smaller than " << arg2 << ".");
/**
 * Exception to indicate that two numbers, which should be exactly equal are
 * not.
 */
template <typename T>
DefException2(ExcNotEqual, T, T, << "Number " << arg1 << " must be equal to "
                                 << arg2 << ".");

//
// Program logic
//
/**
 * Exception denoting a function or functionality has not been implemented
 * either because the programmer originally thought this was too difficult
 * to do or because it was not needed.
 *
 * This should not be used to indicate that something is missing and should
 * be implemented if neccessary.
 */
DefExceptionMsg(ExcNotImplemented,
                "This functionality has not been implemented yet. "
                "Feel free to take a look and implement it.");

/**
 * This exception is used if some object is found uninitialized.
 */
DefExceptionMsg(ExcNotInitialised,
                "The object you attempt to use is not yet initialised.");

/**
 * The object is in a state not suitable for this operation.
 */
DefException1(ExcInvalidState, char *,
              << "The object you attempt to use is not in a valid state: "
              << arg1);

/**
 * The pointer is invalid (e.g. a nullptr) and should not be used.
 */
DefExceptionMsg(ExcInvalidPointer,
                "The pointer you attempt to use is not valid. Check you are "
                "not dereferencing a nullpointer or similar.");

/**
 * Internal error occurred inside a routine
 */
DefExceptionMsg(ExcInternalError,
                "An assertion inside an internal routine has failed. "
                "This is a bug and should not have happened.");

/**
 * The calling of this function was deliberately disabled for some reason (which
 * is given here).
 */
DefException1(ExcDisabled, char *,
              << "The method you attempt to call has been disabled: " << arg1);

/**
 * This is thrown if an iterator should be incremented, decremented
 * or used, but it is already at its final state.
 */
DefExceptionMsg(ExcIteratorPastEnd,
                "You are trying to use an iterator, which is pointing past "
                "the end of its range of valid elements. It is not valid to "
                "dereference or use an iterator in such a case.");

//
// IO and interaction with OS
//

/**
 * Generic IO exception, use <tt>ExcFileNotOpen</tt> to specifically indicate
 * that
 * opening a file for reading or writing has failed.
 */
DefExceptionMsg(ExcIO, "An input/output error has occurred.");
/**
 * An error occurred opening the named file.
 *
 * The constructor takes a single argument of type <tt>char*</tt> naming the
 * file.
 */
DefException1(ExcFileNotOpen, char *, << "Could not open file " << arg1);

}  // namespace exceptions
}  // namespace krims
