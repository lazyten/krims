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
#include "ExceptionBase.hh"
#include <iostream>
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

/** This macro is used for actual errors that should always abort the program.
 *
 * @note Active in DEBUG and RELEASE mode.
 * \note This macro is deprecated
 */
#define assert_abort(cond, exception)                                           \
  _Pragma(                                                                      \
        "GCC warning \"'assert_abort' macro is deprecated. Use 'assert_throw' " \
        "instead.\"") {                                                         \
    assert_throw(cond, exception);                                              \
  }

/** Assert a condition and if it fails (evaluates to false), generate an
 * exception (the 2nd argument).
 *
 * @note Active in DEBUG mode only.
 */
#ifdef DEBUG
#define assert_dbg(cond, exception)  \
  {                                  \
    if (!(cond)) {                   \
      assert_throw(cond, exception); \
    }                                \
  }
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
#define DefExceptionMsg(Exception, defaulttext)                  \
  class Exception : public ::krims::ExceptionBase {              \
   public:                                                       \
    virtual void print_extra(std::ostream& out) const noexcept { \
      out << defaulttext << std::endl;                           \
    }                                                            \
  }

/**
 * Define an exception with one parameter.
 *
 * @ingroup Exceptions
 */
#define DefException1(Exception1, type1, outsequence)            \
  class Exception1 : public ::krims::ExceptionBase {             \
   public:                                                       \
    Exception1(const type1 a1) : arg1(a1) {}                     \
    virtual ~Exception1() noexcept {}                            \
    virtual void print_extra(std::ostream& out) const noexcept { \
      out outsequence << std::endl;                              \
    }                                                            \
                                                                 \
   private:                                                      \
    const type1 arg1;                                            \
  }

/**
 * Define an exception class derived from ExceptionBase with two additional
 * parameters.
 */
#define DefException2(Exception2, type1, type2, outsequence)           \
  class Exception2 : public ::krims::ExceptionBase {                   \
   public:                                                             \
    Exception2(const type1 a1, const type2 a2) : arg1(a1), arg2(a2) {} \
    virtual ~Exception2() noexcept {}                                  \
    virtual void print_extra(std::ostream& out) const noexcept {       \
      out outsequence << std::endl;                                    \
    }                                                                  \
                                                                       \
   private:                                                            \
    const type1 arg1;                                                  \
    const type2 arg2;                                                  \
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
          : arg1(a1), arg2(a2), arg3(a3) {}                         \
    virtual ~Exception3() noexcept {}                               \
    virtual void print_extra(std::ostream& out) const noexcept {    \
      out outsequence << std::endl;                                 \
    }                                                               \
                                                                    \
   private:                                                         \
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
          : arg1(a1), arg2(a2), arg3(a3), arg4(a4) {}                          \
    virtual ~Exception4() noexcept {}                                          \
    virtual void print_extra(std::ostream& out) const noexcept {               \
      out outsequence << std::endl;                                            \
    }                                                                          \
                                                                               \
   private:                                                                    \
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
          : arg1(a1), arg2(a2), arg3(a3), arg4(a4), arg5(a5) {}                   \
    virtual ~Exception5() noexcept {}                                             \
    virtual void print_extra(std::ostream& out) const noexcept {                  \
      out outsequence << std::endl;                                               \
    }                                                                             \
                                                                                  \
   private:                                                                       \
    const type1 arg1;                                                             \
    const type2 arg2;                                                             \
    const type3 arg3;                                                             \
    const type4 arg4;                                                             \
    const type5 arg5;                                                             \
  }

}  // namespace krims
