# Krims
[![Build Status](https://travis-ci.org/linalgwrap/krims.svg?branch=master)](https://travis-ci.org/linalgwrap/krims)[![Licence](https://img.shields.io/badge/license-LGPL_v3.0-blue.svg)](LICENSE)

Library of common *Krimskrams* (German for "odds and ends").
It contains very basic stuff which provides a useful foundation for many other projects.

## Building
All compilers starting from ``clang-3.5`` and ``gcc-4.8`` should be able to build the code.
``C++11`` support is required and enables at least the basic functionality for everything below.
The full implementation is only available for ``C++14``-aware compilers.

The tests depend on some third-party libraries ([rapidcheck](https://github.com/emil-e/rapidcheck)
and [Catch](https://github.com/philsquared/Catch/)), which are automatically
downloaded if the cmake property ``AUTOCHECKOUT_MISSING_REPOS`` is set to ``ON``.

In order to build with tests (recommended) run
```
mkdir build && cd build
cmake -DAUTOCHECKOUT_MISSING_REPOS=ON ..
cmake --build .
ctest
```

In order to build without tests run
```
mkdir build && cd build
cmake -DKRIMS_ENABLE_TESTS=OFF ..
cmake --build .
ctest
```

## Short description
This section sketches the basic functionality of all individual
parts of the library.

### CMake module ``DebugReleaseBuild``
- Located at [cmake/modules/DebugReleaseBuild.cmake](cmake/modules/DebugReleaseBuild.cmake)
- Provides a way to simultaneously build a library in Debug and Release mode
- The idea is to have a Debug version including assertions and extensive error checking
  and a release version, which is highly optimised by the compiler
- Just by selecting which version to link with, one gets either the checking or the speed.
- Using this module a whole dependency tree of libraries can be build for
  Debug and Release simultaneously and hence with little afford one can switch even
  for a complex project.
- The module is also responsible for determining the compiler features and set suitable
  compiler flags for a built.

#### Usage
- Since krims uses this module, see its ``CMakeLists.txt`` for examples.
  Most notable take a look at [CMakeLists.txt](CMakeLists.txt) and
  [src/krims/CMakeLists.txt](src/krims/CMakeLists.txt).
- In short you need to call something like
```CMake
include(DebugReleaseBuild)
drb_init()

# Setup project
project(MyProject)

# Determine compiler flags
set (cxx_minimum_required 11)
drb_setup_compiler_flags(${cxx_minimum_required})

# Add library target
drb_add_library(mylib FILES source.hh)

# Modify compile definitions of release version
drb_target_compile_definitions(RELEASE mylib PUBLIC "EXAMPLE")

# call target_link_libraries on both versions
drb_target_link_libraries(ALL mylib dependency1 dependency2)
```

### CMake module ``WriteVersionHeader``
- Located at [cmake/modules/WriteVersionHeader.cmake](cmake/modules/WriteVersionHeader.cmake)
- The ``write_version_header`` cmake function exports the Project version number
  from CMake into a C++ header (this way one only needs to bump the version at one place)
- The syntax is ``write_version_header file [NAME namesp] [VERSION version_string]``
- The resulting data is put into the namespace ``namesp::detail``.
- If ``NAME`` or ``VERSION`` are not provided, the current project name and version are used.

### The exception system
- Available via the header ``<krims/ExceptionSystem.hh>``,
  which is located at [src/krims/ExceptionSystem.hh](src/krims/ExceptionSystem.hh)
- The idea is to allow for very easy error checking, by the means of assertions.
- Whenever an assertion fails the program is (by default) aborted.
  For tests this can be changed, such that an exception is thrown instead.
- The output on abort is very detailed, including the call stack,
  the failed condition and some message supplied by the programmer.
- Standard assertions (equality of size, is a number within a range)
  are predefined and only take a single line of code.
- Checking is usually done using the ``assert_dbg`` macro, which expands
  to an empty statement in Release mode.
- Helper macros to quickly define Exceptions are available.
  See [src/krims/ExceptionSystem/Exceptions.hh](src/krims/ExceptionSystem/Exceptions.hh)
  for predefined examples.

#### Example code
```cpp
double devide(double a, double b) {
	// If b is zero this is an error
	assert_dbg(b != 0.,ExcZero());

	// a and b should also be finite.
	assert_finite(a);
	assert_finite(b);

	return a/b;
}

double data[10];
double get(size_t i) {
	// if i is larger than 10 this is an error
	assert_greater(i,10);
	return data[i];
}
```

### Performing numeric-aware comparison
- Available via the header ``<krims/NumComp.hh>``,
  which is available at [src/krims/NumComp.hh](src/krims/NumComp.hh).
- This set of classes easily perform floating-point error-aware comparison
  of floating point types or ``std::complex<T>`` types.
- For example
```cpp
#include <krims/NumComp.hh>
using namespace krims;

int main() {
	if (10.0000000000001 == numcomp(10.)) {
		return 0;
	} else {
		return 1;
	}
}
```
  checks for the equality using some small tolerance.
- The tolerance can either be influenced relative to the
  numerical epsilon using the enum ``NumCompAccuracyLevel``
  or by supplying an absolute value, e.g.
```cpp
#include <krims/NumComp.hh>
using namespace krims;

int main() {
	if (10.0000000000001 == numcomp(10.).tolerance(NumCompAccuracyLevel::Sloppy) {
		return 0;
	} else {
		return 1;
	}
}
```
- By default ``true`` or ``false`` is returned.
  More information about why and how bad the comparison failed
  can be obtained if one sets the ``failure_action``
  to ``NumCompActionType::ThrowNormal`` or ``NumCompActionType::ThrowVerbose``
  like
```cpp
#include <krims/NumComp.hh>
using namespace krims;

int main() {
	if (10.0000000000001 == numcomp(10.).failure_action(NumCompActionType::ThrowNormal) {
		return 0;
	} else {
		return 1;
	}
}
```
