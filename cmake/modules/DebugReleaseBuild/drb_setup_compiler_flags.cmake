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

include(CheckCXXCompilerFlag)

macro(determine_supported_cxx_standards)
	# macro to check for the highest fully supported c++ standard.
	# Sets the cache variables:
	#       DRB_HAS_CXX11_SUPPORT
	#       DRB_HAS_CXX14_SUPPORT
	#       DRB_HAS_CXX17_SUPPORT
	# to ON or OFF respectively and also
	#       DRB_HIGHEST_CXX_SUPPORT
	# to the plain values (98,11,14,17).
	#
	# Note that the macro does not enforce any standard.
	#
	if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
		# All of gcc supports 98
		set(DRB_HIGHEST_CXX_SUPPORT 98)

		if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.8.1" OR
				CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "4.8.1")
			set(DRB_HAS_CXX11_SUPPORT ON CACHE INTERNAL "Did DRB detect c++11 support.")
			set(DRB_HIGHEST_CXX_SUPPORT 11 CACHE INTERNAL "The highest c++ standard supported.")
		endif()

		if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "5.0" OR
				CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "5.0")
			set(DRB_HAS_CXX14_SUPPORT ON CACHE INTERNAL "Did DRB detect c++14 support.")
			set(DRB_HIGHEST_CXX_SUPPORT 14 CACHE INTERNAL "The highest c++ standard supported.")
		endif()

		# TODO Extend once c++17 is ready
	elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		# All of clang supports 98
		set(DRB_HIGHEST_CXX_SUPPORT 98)

		if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "3.3" OR
				CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "3.3")
			set(DRB_HAS_CXX11_SUPPORT ON CACHE INTERNAL "Did DRB detect c++11 support.")
			set(DRB_HIGHEST_CXX_SUPPORT 11 CACHE INTERNAL "The highest c++ standard supported.")
		endif()

		if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "3.4" OR
				CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "3.4")
			set(DRB_HAS_CXX14_SUPPORT ON CACHE INTERNAL "Did DRB detect c++14 support.")
			set(DRB_HIGHEST_CXX_SUPPORT 14 CACHE INTERNAL "The highest c++ standard supported.")
		endif()

		# TODO Extend once c++17 is ready
	else()
		message(WARNING "Determining maximum supported C++ version is not supported for compiler ${CMAKE_CXX_COMPILER_ID} yet. \
Going with C++98.")
		set(DRB_HIGHEST_CXX_SUPPORT 98)
	endif()
endmacro(determine_supported_cxx_standards)

macro(use_cxx_standard STANDARD)
	# macro to enforce a particular c++ standard (only needed for cmake < 3.1)
	#
	if (CMAKE_VERSION VERSION_LESS "3.1")
		if (STANDARD VERSION_LESS "11")
			message(FATAL_ERROR "C++ Standard below 11 not supported in USE_CXX_STANDARD")
		endif()

		set (FLAG "c++${STANDARD}")
		if (STANDARD EQUAL "14")
			if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.5")
				# Clang <3.5 needs special treatment here, 
				# since c++14 flag is not yet know there
				set(FLAG "c++1y")
			endif()
		endif()

		# Check whether the flag works
		CHECK_CXX_COMPILER_FLAG("-std=${FLAG}" DRB_HAVE_STANDARD_FLAG)
		if (NOT DRB_HAVE_STANDARD_FLAG)
			message(FATAL_ERROR "C++ Standard ${STANDARD} is required but not supported.")
		endif()
		unset(DRB_HAVE_STANDARD_FLAG)

		# Add it to the list of flags.
		set (CMAKE_CXX_FLAGS "-std=${FLAG} ${CMAKE_CXX_FLAGS}")
		unset(FLAG)
	else()
		# set the standard and enforce it:
		set(CMAKE_CXX_STANDARD ${STANDARD})
		set(CMAKE_CXX_STANDARD_REQUIRED ON)

		# make sure we no not use the gnu++ extensions:
		# this adds c++n instead of gnu++n to the command line
		set(CMAKE_CXX_EXTENSIONS OFF)
	endif()
endmacro(use_cxx_standard)

macro(enable_if_compiles VARIABLE FLAG)
	# Checks whether a compiler supports a flag and if yes
	# adds it to the variable provided.
	#
	string(REGEX REPLACE "[^a-zA-Z0-9]" "" FLAG_CLEAN "${FLAG}")
	CHECK_CXX_COMPILER_FLAG("-Werror ${FLAG}" DRB_HAVE_FLAG_${FLAG_CLEAN})
	if (DRB_HAVE_FLAG_${FLAG_CLEAN})
		set(${VARIABLE} "${${VARIABLE}} ${FLAG}")
	endif()
	unset(FLAG_CLEAN)
endmacro(enable_if_compiles)

# TODO have something similar for the linker

macro(DRB_SETUP_COMPILER_FLAGS CXX_STANDARD)
	# Determine the compiler type and load corresponding flags.
	# Enforce a c++ standard above CXX_STANDARD. 
	# 
	# sets the following variables
	#       CMAKE_CXX_FLAGS				Flags for the c++ compiler, all builds
	#	CMAKE_CXX_FLAGS_DEBUG			Extra flags for the debug build
	#	CMAKE_CXX_FLAGS_RELEASE			Extra flags for the release build
	#
	#	CMAKE_SHARED_LINKER_FLAGS		Flags for the shared linker, all builds
	#	CMAKE_SHARED_LINKER_FLAGS_DEBUG		Extra flags for the debug build
	#	CMAKE_SHARED_LINKER_FLAGS_RELEASE	Extra flags for the release build
	#
	#	CMAKE_STATIC_LINKER_FLAGS		Flags for the static linker, all builds
	#	CMAKE_STATIC_LINKER_FLAGS_DEBUG		Extra flags for the debug build
	#	CMAKE_STATIC_LINKER_FLAGS_RELEASE	Extra flags for the release build
	#
	#	CMAKE_EXE_LINKER_FLAGS			Flags for linking executables, all builds
	#	CMAKE_EXE_LINKER_FLAGS_DEBUG		Extra flags for the debug build
	#	CMAKE_EXE_LINKER_FLAGS_RELEASE		Extra flags for the release build
	#
	#	DRB_HAS_CXX11_SUPPORT			Do we have full c++11 support?
	#	DRB_HAS_CXX14_SUPPORT			Do we have full c++14 support?
	#	DRB_HAS_CXX17_SUPPORT			Do we have full c++17 support?

	# check that drb has been initialised
	if(NOT DRB_INITIALISED)
		message(FATAL_ERROR "You have to call drb_init before any other DebugReleaseBuild function.")
	endif()

	# check what standards are supported and enforce the highest,
	# but at least the one provided by the user
	determine_supported_cxx_standards()
	if (DRB_HIGHEST_CXX_SUPPORT VERSION_GREATER CXX_STANDARD)
		use_cxx_standard(${DRB_HIGHEST_CXX_SUPPORT})
	else()
		use_cxx_standard(${CXX_STANDARD})
	endif()

	if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		# We have a known "standard" compiler
		include("${DRB_DIR}/compilers/standard.cmake")
	else()
		message(WARNING "Untested compiler: ${CMAKE_CXX_COMPILER_ID}, you are on your own.")
		message(WARNING "Currently we only support clang and gnu compilers.")

		# Try to fall back to the standard
		include("${DRB_DIR}/compilers/standard.cmake")
	endif()
endmacro(DRB_SETUP_COMPILER_FLAGS)
