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

# Compiler flags for the default compilers, i.e. gnu or clang


#######################
#--  Warning policy --#
#######################

# The warning policy we use here is based on a very valuable
# post by a clang developer on stack exchange
# https://programmers.stackexchange.com/questions/122608#124574
# and a list of recommended flags on
# https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md

#
# Warnings
#
# Show high confidence warning
enable_if_compiles(CMAKE_CXX_FLAGS "-Wall")

# Show valuable extra warnings
enable_if_compiles(CMAKE_CXX_FLAGS "-Wextra")

# Warn if virtual classes do not have a virtual destructor
enable_if_compiles(CMAKE_CXX_FLAGS "-Wnon-virtual-dtor")

# Warn if a virtual function is overloaded
enable_if_compiles(CMAKE_CXX_FLAGS "-Woverloaded-virtual")

# Warn about old c-style casts
enable_if_compiles(CMAKE_CXX_FLAGS "-Wold-style-cast")

# Warn about performance-critcal casts
enable_if_compiles(CMAKE_CXX_FLAGS "-Wcast-align")

# But silence some rather annoying warnings
enable_if_compiles(CMAKE_CXX_FLAGS "-Wno-unused-macros")
enable_if_compiles(CMAKE_CXX_FLAGS "-Wno-unused-parameter")

# Turn on warnings about language extensions
enable_if_compiles(CMAKE_CXX_FLAGS "-pedantic")

#
# Warnings as errors
#
# Make warnings errors, such that we cannot ignore them
enable_if_compiles(CMAKE_CXX_FLAGS "-Werror")

# Do not make overloaded virtuals errors:
# TODO maybe do?
enable_if_compiles(CMAKE_CXX_FLAGS "-Wno-error=overloaded-virtual")
enable_if_compiles(CMAKE_CXX_FLAGS "-Wno-error=deprecated-declarations")

#######################
#-- Bug workarounds --#
#######################
#include(${DRB_DIR}/workarounds/missing_gets_bug.cmake)

######################
#--  Linker flags  --#
######################
# Add all symbols in the symbol table, such that we can get meaningful stacktraces
# on application crash.
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

##############
#--  Debug --#
##############
#
# Extra stuff for debug:
#
if (CMAKE_BUILD_TYPE MATCHES "Debug")
	enable_if_compiles(CMAKE_CXX_FLAGS_DEBUG "-O0")
	enable_if_compiles(CMAKE_CXX_FLAGS_DEBUG "-Og")

	# Common linker flags for all of debug:
	set(COMMON_LINKER_FLAGS_DEBUG "${COMMON_LINKER_FLAGS_DEBUG} -g")
	set(COMMON_LINKER_FLAGS_DEBUG "${COMMON_LINKER_FLAGS_DEBUG} -ggdb")

	set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} ${COMMON_LINKER_FLAGS_DEBUG}")
	set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} ${COMMON_LINKER_FLAGS_DEBUG}")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${COMMON_LINKER_FLAGS_DEBUG}")

	unset(COMMON_LINKER_FLAGS_DEBUG)
endif()

################
#--  Release --#
################
#
# Extra stuff for release:
#
option(DRB_MACHINE_SPECIFIC_OPTIM_Release "Enable machine-specific optimisations in REALEASE build. Your build might not be transferable to other machines.")
if (CMAKE_BUILD_TYPE MATCHES "Release")
	if (DRB_MACHINE_SPECIFIC_OPTIM_Release)
		enable_if_compiles(CMAKE_CXX_FLAGS_RELEASE "-march=native")
		enable_if_compiles(CMAKE_CXX_FLAGS_RELEASE "-mtune=native")
		message(STATUS "Enabled machine-specific optimisations in Release build.")
	endif()
endif()

###################################
#--  Special section for clang  --#
###################################
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# Try to see whether clang and the system support llvm's libc++ library
	# as the standard c++ library
	set(CMAKE_REQUIRED_FLAGS_ORIG "${CMAKE_REQUIRED_FLAGS}")
	set(CMAKE_REQUIRED_FLAGS "-stdlib=libc++ -Werror -std=c++11")
	CHECK_CXX_SOURCE_COMPILES("#include <vector>
int main() { std::vector<int> v{0,}; return v[0]; }" DRB_HAVE_LLVM_LIBCXX)
	set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_ORIG}")
	unset(CMAKE_REQUIRED_FLAGS_ORIG)

	# Set the library to link with accordingly in the cache:
	if ("${DRB_CXX_STANDARD_LIBRARY}" STREQUAL "")
		if(DRB_HAVE_LLVM_LIBCXX)
			set(DRB_CXX_STANDARD_LIBRARY "libc++")
		else()
			set(DRB_CXX_STANDARD_LIBRARY "libstdc++")
		endif()
		set(DRB_CXX_STANDARD_LIBRARY "${DRB_CXX_STANDARD_LIBRARY}" CACHE STRING
			"Choose the standard library clang should link with")
	endif()

	# And alos in the compiler/linker flags:
	if (DRB_CXX_STANDARD_LIBRARY STREQUAL "libc++" OR
			DRB_CXX_STANDARD_LIBRARY STREQUAL "libstdc++")
		set(CMAKE_EXE_LINKER_FLAGS
			"${CMAKE_EXE_LINKER_FLAGS_DEBUG} -stdlib=${DRB_CXX_STANDARD_LIBRARY}")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=${DRB_CXX_STANDARD_LIBRARY}")
	else()
		message(FATAL_ERROR "Unrecognised value \"${DRB_CXX_STANDARD_LIBRARY}\" \
for CPP_STANDARD_LIBRARY. Only \"libc++\" or \"libstdc++\" are valid.")
	endif()
endif()
