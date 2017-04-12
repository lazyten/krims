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
#include "ExceptionBase.hh"
#include <sstream>
#include <string>

namespace krims {

namespace detail {
/** Wrapper to make sure, that an exception is really thrown by value */
template <typename Exception>
void throw_by_value(Exception e) {
  throw typename std::remove_cv<Exception>::type(e);
}
}  // namespace detail

//
// Assert Macros
//
/** Assert a condition and if it evaluates to false, throw the exception
 *  given as the second argument.
 *
 * @note Active in DEBUG and RELEASE mode.
 */
#define assert_throw(cond, exception)                                          \
  {                                                                            \
    if (!(cond)) {                                                             \
      auto __exc__cept = exception;                                            \
      __exc__cept.add_exc_data(__FILE__, __LINE__, __PRETTY_FUNCTION__, #cond, \
                               #exception);                                    \
      ::krims::detail::throw_by_value(__exc__cept);                            \
    }                                                                          \
  }

/** Assert a condition and if it fails (evaluates to false), throw the
 * exception (the 2nd argument).
 *
 * @note Active in DEBUG mode only.
 */
#ifdef DEBUG
#define assert_dbg(cond, exception) \
  { assert_throw(cond, exception); }
#else
#define assert_dbg(cond, exception) \
  {}
#endif

//
// DefException Macros
//
/**
 * Define an exception with a string parameter. If no string is supplied
 * on exception construction, the default string is printed when the exception
 * is raised.
 */
#define DefExceptionMsg(Exception, text)                    \
  class Exception : public ::krims::ExceptionBase {         \
   public:                                                  \
    Exception() { ::krims::ExceptionBase::m_extra = text; } \
  }

/**
 * Define an exception with one parameter.
 *
 * @ingroup Exceptions
 */
#define DefException1(Exception1, type1, outsequence) \
  class Exception1 : public ::krims::ExceptionBase {  \
   public:                                            \
    Exception1(const type1 a1) : arg1(a1) {           \
      std::ostringstream ss;                          \
      ss outsequence;                                 \
      ::krims::ExceptionBase::m_extra = ss.str();     \
    }                                                 \
    const type1 arg1;                                 \
  }

/**
 * Define an exception class derived from ExceptionBase with two additional
 * parameters.
 */
#define DefException2(Exception2, type1, type2, outsequence)          \
  class Exception2 : public ::krims::ExceptionBase {                  \
   public:                                                            \
    Exception2(const type1 a1, const type2 a2) : arg1(a1), arg2(a2) { \
      std::ostringstream ss;                                          \
      ss outsequence;                                                 \
      ::krims::ExceptionBase::m_extra = ss.str();                     \
    }                                                                 \
    const type1 arg1;                                                 \
    const type2 arg2;                                                 \
  }

/**
 * Declare an exception class derived from ExceptionBase with three additional
 * parameters.
 *
 * @ingroup Exceptions
 */
#define DefException3(Exception3, type1, type2, type3, outsequence) \
  class Exception3 : public ::krims::ExceptionBase {                \
   public:                                                          \
    Exception3(const type1 a1, const type2 a2, const type3 a3)      \
          : arg1(a1), arg2(a2), arg3(a3) {                          \
      std::ostringstream ss;                                        \
      ss outsequence;                                               \
      ::krims::ExceptionBase::m_extra = ss.str();                   \
    }                                                               \
    const type1 arg1;                                               \
    const type2 arg2;                                               \
    const type3 arg3;                                               \
  }

/**
 * Declare an exception class derived from ExceptionBase with four additional
 * parameters.
 *
 * @ingroup Exceptions
 */
#define DefException4(Exception4, type1, type2, type3, type4, outsequence)     \
  class Exception4 : public ::krims::ExceptionBase {                           \
   public:                                                                     \
    Exception4(const type1 a1, const type2 a2, const type3 a3, const type4 a4) \
          : arg1(a1), arg2(a2), arg3(a3), arg4(a4) {                           \
      std::ostringstream ss;                                                   \
      ss outsequence;                                                          \
      ::krims::ExceptionBase::m_extra = ss.str();                              \
    }                                                                          \
    const type1 arg1;                                                          \
    const type2 arg2;                                                          \
    const type3 arg3;                                                          \
    const type4 arg4;                                                          \
  }

/**
 * Declare an exception class derived from ExceptionBase with five additional
 * parameters.
 *
 * @ingroup Exceptions
 */
#define DefException5(Exception5, type1, type2, type3, type4, type5, outsequence) \
  class Exception5 : public ::krims::ExceptionBase {                              \
   public:                                                                        \
    Exception5(const type1 a1, const type2 a2, const type3 a3, const type4 a4,    \
               const type5 a5)                                                    \
          : arg1(a1), arg2(a2), arg3(a3), arg4(a4), arg5(a5) {                    \
      std::ostringstream ss;                                                      \
      ss outsequence;                                                             \
      ::krims::ExceptionBase::m_extra = ss.str();                                 \
    }                                                                             \
    const type1 arg1;                                                             \
    const type2 arg2;                                                             \
    const type3 arg3;                                                             \
    const type4 arg4;                                                             \
    const type5 arg5;                                                             \
  }

}  // namespace krims
