## ---------------------------------------------------------------------
##
## Copyright (C) 2016 by the krims authors
##
## This file is part of krims.
##
## krims is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published
## by the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## krims is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with krims. If not, see <http://www.gnu.org/licenses/>.
##
## ---------------------------------------------------------------------

# adds entries to these things
#
#       KRIMS_DEPENDENCIES			everyone needs these libraries
#       KRIMS_DEPENDENCIES_DEBUG		debug mode needs these extras
#       KRIMS_DEPENDENCIES_RELEASE		release mode needs these extras
#       KRIMS_DEPENDENCIES_TEST			tests need these extra libraries
#
#       KRIMS_DEFINITIONS			definitions for all compilation
#       KRIMS_DEFINITIONS_DEBUG			definitions for debug mode
#       KRIMS_DEFINITIONS_RELEASE		definitions for release mode
#

#
# Modules and macros
#
include(CheckCXXSourceCompiles)


####################
#-- C++ standard --#
####################
if (NOT CMAKE_CXX_STANDARD VERSION_LESS 14)
	message(STATUS "Detected C++14 support: Setting KRIMS_HAVE_CXX14")
	set(KRIMS_HAVE_CXX14 ON)
endif()
if (NOT CMAKE_CXX_STANDARD VERSION_LESS 17)
	message(STATUS "Detected C++17 support: Setting KRIMS_HAVE_CXX17")
	set(KRIMS_HAVE_CXX17 ON)
endif()

#############################
#-- Experimental features --#
#############################
option(KRIMS_ENABLE_EXPERIMENTAL "Enable experimental features in krims" OFF)
if(KRIMS_ENABLE_EXPERIMENTAL)
	# Enable addr2line translation
	message(STATUS "Enable experimental features for krims")
	LIST(APPEND KRIMS_DEFINITIONS "KRIMS_ENABLE_EXPERIMENTAL")
endif()

########################
#-- Exception system --#
########################
#
# Check whether stacktrace information is available for the exception
# system. We expect the interface of glibc.
# If it is than we also add the -rdynamic flag since this is required
# if one wants meaningful backtraces.
#
CHECK_CXX_SOURCE_COMPILES(
	"
	#include <execinfo.h>
	#include <stdlib.h>
	const int asize = 25;
	void * array[asize];
	int nBT = backtrace(array, asize);
	char ** bt_raw = backtrace_symbols(array, nBT);
	int main() { free(bt_raw); return 0; }
	"
	KRIMS_HAVE_GLIBC_STACKTRACE)

if(KRIMS_HAVE_GLIBC_STACKTRACE)
	# needed for meaningful stacktraces
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic")
endif()

#
# Check for demangling symbols from within the program.
# We expect the interface from the libstdc++
#
# The test example is taken from
#   http://gcc.gnu.org/onlinedocs/libstdc++/18_support/howto.html#6
#
if(KRIMS_HAVE_GLIBC_STACKTRACE)
	CHECK_CXX_SOURCE_COMPILES(
		"
		#include <exception>
		#include <iostream>
		#include <cxxabi.h>
		#include <cstdlib>

		struct empty { };

		template <typename T, int N>
		struct bar { };

		int     status;
		char   *realname;

		int main()
		{
		// exception classes not in <stdexcept>, thrown by the implementation
		// instead of the user
		std::bad_exception  e;
		realname = abi::__cxa_demangle(e.what(), 0, 0, &status);
		free(realname);


		// typeid
		bar<empty,17>          u;
		const std::type_info  &ti = typeid(u);

		realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
		free(realname);

		return 0;
		}
		"
		KRIMS_HAVE_LIBSTDCXX_DEMANGLER)
endif()

