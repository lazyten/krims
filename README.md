# Krims
[![Build Status](https://travis-ci.org/linalgwrap/krims.svg?branch=master)](https://travis-ci.org/linalgwrap/krims)
[![Coverage Status](https://coveralls.io/repos/github/linalgwrap/krims/badge.svg?branch=master)](https://coveralls.io/github/linalgwrap/krims)  
[![Licence](https://img.shields.io/badge/license-LGPL_v3.0-blue.svg)](LICENSE)

Library of common *Krimskrams* (German for "odds and ends").
It contains some very basic stuff which provides a useful foundation for many other projects.

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
```

## Short description
This section sketches the basic functionality of all individual
parts of the library.

- [CMake module ``DebugReleaseBuild``](#cmake-module-debugreleasebuild)
- [CMake module ``WriteVersionHeader``](#cmake-module-writeversionheader)
- [CMake module ``SetupClangTargets``](#cmake-module-setupclangtargets)
- [The exception system](#the-exception-system)
- [Performing floating point comparisons](#performing-floating-point-comparisons)
- [``Subscribable`` base class and ``SubscriptionPointer``](#subscribable-base-class-and-subscriptionpointer)
- [Useful type properties and type transformations](#useful-type-properties-and-type-transformations)
- [``GenMap``: A hierachical dictionary for managing data of arbitrary type.](#genmap-a-hierachical-dictionary-for-managing-data-of-arbitrary-type)
- [Iterator utils](#iterator-utils)
- [Circular buffer with maximum size](#circular-buffer-with-maximum-size)
- [Useful helper functions to deal with tuples](#useful-helper-functions-to-deal-with-tuples)

### CMake module ``DebugReleaseBuild``
- Located at [cmake/modules/DebugReleaseBuild.cmake](cmake/modules/DebugReleaseBuild.cmake)
- Provides a way to simultaneously build a library in Debug and Release mode
- The idea is to have a Debug version including assertions and extensive error checking
  and a release version, which is highly optimised by the compiler
- Just by selecting which version to link with, one gets either the checking or the speed.
- Using this module a whole dependency tree of libraries can be build for
  Debug and Release simultaneously and hence with little effort one can switch even
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
#    and require c++11 or higher
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

### CMake module ``SetupClangTargets``
- Add targets to easily run certain clang tools on the project
- Currently reformatting a project using ``clang-format`` and
  checking for common coding errors unsing ``clang-tidy`` is
  supported. Fixing errors from ``clang-tidy`` automatically
  (i.e. ``clang-tidy -fix``) is supported as well.
- For example: The code
```CMake
include(SetupClangTargets)
add_available_clang_targets_for(myproject
	DIRECTORIES src tests
)
```
  will setup the targets ``clang-format-myproject``,
  ``clang-tidy-myproject`` and ``clang-tidy-myproject-fix``.
- These targets will all work on the header and source files,
  which are located inside the directories ``src``
  and ``tests``.  
  Note, that these directories should have been
  added to the project using CMake's ``add_subdirectories``
  *already*. In other words it is advisable to call
  ``add_available_clang_targets_for`` somewhere at the
  bottom of the project's  ``CMakeLists.txt``.
- The format target ``clang-format-myproject``
  will reformat all files of the project according to the
  selected formatting guidelines in the appropriate
  ``.clang-format`` file.
- ``clang-tidy-myproject`` performs the configured
  ``clang-tidy``-checks and displays detected problems.
  If fixes are available to the problems, they can be
  applied with ``clang-tidy-myproject-fix`` automatically.
  Note that these two targets are only available on
  CMake above 3.5.0 or if ``ninja`` is used to perform
  the build.

### The exception system
- Available via the header [``<krims/ExceptionSystem.hh>``](src/krims/ExceptionSystem.hh).
- The idea is to allow for very easy error checking, by the means of assertions.
- Whenever an assertion fails the program is (by default) aborted.
  For tests this can be changed, such that an exception is thrown instead.
- The output on abort is very detailed, including the call stack,
  the failed condition and some message supplied by the programmer.
- Standard assertions (equality of size, is a number within a range)
  are predefined and only take a single line of code.
- Checking is usually done using the ``assert_dbg`` macro, which expands
  to an empty statement in Release mode.
- The whole system is suitable for multi-threaded applications.
- Helper macros to quickly define Exceptions are available.
  See [src/krims/ExceptionSystem/Exceptions.hh](src/krims/ExceptionSystem/Exceptions.hh)
  for predefined examples.
- See [examples/ExceptionSystem_demo/main.cc](examples/ExceptionSystem_demo/main.cc)
  for an example program, which triggers some exceptions.

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

### Performing floating point comparisons
- Available via the header [``<krims/NumComp.hh>``](src/krims/NumComp.hh).
- This set of classes easily perform error-tolerant comparison
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

### ``Subscribable`` base class and ``SubscriptionPointer``
- Provides a mechanism to transparently subscribe to objects,
  which are only available as references.
- Storing a reference to an object inside another class can be problematic,
  since it may well happen that the referenced object goes out of
  scope. If the class uses this reference thereafter
  a surprising error may result.
- This system tries to circumvent this problem by introducing a
  ``SubscriptionPointer`` which may subscribe to an object
  derived off the ``Subscribable`` base class.
  Each such subscription increases a reference count inside
  ``Subscribable``.
- If the ``Subscribable`` object, i.e. the object ``SubscriptionPointer``
  points to, is destroyed with a reference count greater
  zero, an Exception is raised via the krims exception system.
  In other words the reference counting only happens in the
  Debug version of the library.
- Note, that the classes are designed to be *thread-safe*.
- The implementation is provided it the headers
  [``<krims/Subscribable.hh>``](src/krims/Subscribable.hh)
  and
  [``krims/SubscriptionPointer.hh``](src/krims/SubscriptionPointer.hh).
- This class provides an alternative to the smart ``std::shared_ptr``
  of C++11. Especially in cases where large amounts of data
  (like big matrices) need to be accessed from various places
  in a code without being the owner of the data, this system is useful.
- The ``GenMap`` (see below) has full support
  for storing arbitrary subscribable objects by reference.

### Useful type properties and type transformations
- Some utility classes aiding with SFINAE or type conversion
  are available via the header [``<krims/TypeUtils.hh>``](src/krims/TypeUtils.hh)
- ``RealTypeOf`` extracts the real type of a complex number
  of is the identity to a normal float
- ``IsCheaplyCopyable`` determines whether data of this type
  is considered to be cheaply copyable.
  One can manually flag a class as cheaply copyable by deriving it off
  the marker interface [``CheaplyCopyable_i``](src/krims/TypeUtils/CheaplyCopyable_i.hh).

### ``GenMap``: A hierachical dictionary for managing data of arbitrary type.
- The GenMap allows to store and access data of an arbitrary type
  with the aid of ``std::string`` lookup keys.
- Data is automatically either stored by-value
  (for cheaply copyable types like floating point values,
  integers or strings), as a ``std::shared_ptr`` or as a ``SubscriptionPointer``.
- One can use ``std::initializer_list``s to easily construct or update GenMap,
  e.g.
```cpp
GenMap map{ {"key": 3}, {"key2" : "value2" } };
auto i = std::make_shared<int>(15);
map.update({"an integer", i});
// or equivalently:
map.update("an integer", i);
```
- If one is happy to copy the data inside the map, the function ``update_copy``
  is available, which effectively is a convenience function for making and storing
  a ``std::shared_ptr`` to the copy.
- The data can be retrieved as a pointer or by reference.
  A default value can be provided for use if the key does not exist:
```cpp
// Use default value 5 if key does not exist
map.at("nonexistent", 5)

// Use a default pointer to some other place
// if key does not exist
map.at_ptr("nonexistent", make_shared<int>(4));
```
  On retrieval of the value, the type needs to specified once again.
  If the type does not match the original type, an error is thrown
  in Debug mode.
```cpp
auto this_is_15 = map.at<int>("an integer");

// Error, will abort program in Debug mode
auto error = map.at<std::string>("an integer");
```
- The ``GenMap`` has a notion for hierarchical storage as well:
  Keys which contain a slash ``/`` are interpreted like a UNIX path.
  Using the ``submap`` function, one can navigate into a subpath,
  which offers the same interface as the original map.
  This way one can selectively shadow parts of the stored data
  and allow different parts of the program to transparently
  manage parameters or references to results of computations.
- Similar to ``std::map`` objects, a ``GenMap`` supports range-
  based ``for`` loops and iteration over the map as well as
  submaps, e.g.
```cpp
// Print all keys within the map
for (auto& kv : map) {
  std::cout << kv.key() << std::endl;
}

// Print a subtree, where we know that all 
// entries are integer values:
for (auto& kv : map.subtree("only_ints") {
  std::cout << kv.key() << " "
      << kv.value<int>() << std::endl;
}
```
- An example is located at [examples/GenMap_demo](examples/GenMap_demo).

### Iterator utils
- [``krims/IteratorUtils.hh``](src/krims/IteratorUtils.hh) includes classes for
  wrapping iterators.
- E.g. [``CircularIterator``](src/krims/IteratorUtils/CircularIterator.hh) implements
  an iterator to iterate over a range in a circular fashion. I.e. if one reaches
  the end of the range, the iterator detects this and wraps over to restart at the
  begining. The same in the other direction.
- The functions ``circular_begin`` and ``circular_end`` are available to construct
  suitable ``CircularIterator``s in order to iterate over pretty much any container
  circularly.
- Furthermore [``DereferenceIterator``](src/krims/IteratorUtils/DereferenceIterator.hh)
  provides a mechanism to easily iterate over a container with pointers to some objects,
  yielding directly the objects instead of the pointers.

### Circular buffer with maximum size
- [``<krims/CircularBuffer.hh>``](src/krims/CircularBuffer.hh) contains 
  the class ``CircularBuffer``, which provides a circular buffer implementation with
  a maximal size. It may contain less elements, but not more.
  If the maximal size is 5 and one pushes a 6th element to the buffer, the first
  is deleted automatically.
- ``CircularBuffer`` is very helpful to store e.g. a history of the ``N`` last steps
  in an iterative algorithm.

### Useful helper functions to deal with tuples
- The header [``<krims/TupleUtils.hh>``](src/krims/TupleUtils.hh)
  provides a number of utility functions
  which ease the use of ``std::tuple`` objects.
- The ``apply`` function allows to call a functor, lambda or ``std::function``
  object using the elements of a tuple as the parameters to the call
  In other words
```cpp
auto add_lambda = [] (double x, int y) { return x+y; };
auto tuple = std::make_tuple(3.1415,42);

double res = krims::apply(add_lambda, tuple);
// res is 45.1415
```
  calls the lambda ``add_lambda`` with the arguments ``3.1415`` and ``42``.
- ``tuple_for_each`` calls a functor, lambda or ``std::function``
  for each element of a tuple in turn, i.e. for a tuple with 6
  elements the function is called 6 times with one of the tuple elements
  as the argument.
  The function should therefore be generic in the types of the tuple elements.
- ``tuple_map`` is similar to ``tuple_for_each``:
  It applies a functor, lambda or ``std::function`` object
  to each tuple element and stores the returned values in a tuple, which
  is returned.
  In other words
```cpp
auto add3 = [] (double x) { return x+3; };
auto tuple = std::make_tuple(3.1415,42);
auto res = krims::tuple_for_each(add3,tuple);
// res is std::tuple<double,double>{6.1415,45.0}
```
  and
```cpp
auto add3 = [] (double x) { return x+3; };
auto tuple = std::make_tuple(3.1415,42);
auto res = std::make_tuple(add3(std::get<0>(tuple)), add3(std::get<1>(tuple)))
// res is std::tuple<double,double>{6.1415,45.0}
```
  are equivalent. A binary version which calls a function with 2 arguments
  on each pair of elements from two tuples also exists.
```cpp
auto add = [] (double x, double y) { return x+y; };
auto tuple1 = std::make_tuple(3, 5.6);
auto tuple2 = std::make_tuple(1.5, 2);
auto res = krims::tuple_map(add,tuple1,tuple2);
// res is std::tuple<double,double>{4.5,7.6}
```
- The code is implemented differently for the various ``C++``
  standards, making best use of the features the standard libraries
  as well as the language offers in these versions of the standard.
- For ``C++11`` only tuples with 4 elements or less are supported.
  From ``C++14`` onwards there is no restriction any more.
