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

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang"
		AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "3.5.0")
	# Clang only supports this on 3.5 and up
	return()
endif()

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU"
		AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.9.0")
	# GCC only supports this on 4.9 and up
	return()
endif()

#
# Options
#

option(DRB_SANITIZE_ADDRESS_Release   "Enable detection of memory errors (Out-of-bounds, use-after-free, ...) in RELEASE build.")
option(DRB_SANITIZE_THREAD_Release    "Enable detection of data races introduced by multi-threading in the RELEASE build.")
option(DRB_SANITIZE_UNDEFINED_Release "Enable detection of undefined behaviour instructions (negative shifts, integer overflows) in the RELEASE build.")

set(DRB_SANITIZE_OPTIONS DRB_SANITIZE_THREAD_Release DRB_SANITIZE_ADDRESS_Release
	DRB_SANITIZE_UNDEFINED_Release)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# AFAIK only clang has this feature
	option(DRB_SANITIZE_MEMORY_Release "Enable detection of unitialised memory reads in RELEASE build.")
	set(DRB_SANITIZE_OPTIONS ${DRB_SANITIZE_OPTIONS} DRB_SANITIZE_MEMORY_Release)
endif()

#
# Consistency check
#
set(DRB_HAVE_ANY_SANITIZE OFF)
foreach (_opt ${DRB_SANITIZE_OPTIONS})
	if (${_opt})
		if (DRB_HAVE_ANY_SANITIZE)
			message(FATAL_ERROR "Only one of ${DRB_SANITIZE_OPTIONS} may be switched on at a time.")
		endif()
		set (DRB_HAVE_ANY_SANITIZE ON)
	endif()
endforeach()

#
# Sanitize flags -- Release
#
if (CMAKE_BUILD_TYPE MATCHES "Release" AND DRB_HAVE_ANY_SANITIZE)
	# Reduce optimisation level to 1
	string(REGEX REPLACE "-O[02-9]" "-O1" RELEASE_FLAGS_REPLACED "${CMAKE_CXX_FLAGS_RELEASE}")
	set(CMAKE_CXX_FLAGS_RELEASE "${RELEASE_FLAGS_REPLACED}")
	unset(RELEASE_FLAGS_REPLACED)

	# Set flags for sanitizer:
	set(_FLAGSS "-g")
	set(dummy "")

	if(DRB_SANITIZE_MEMORY_Release)
		set(_FLAGSS "${_FLAGSS} -fsanitize=memory")
		enable_if_all_compiles(_FLAGSS dummy "-fsanitize-memory-track-origin")
		message(STATUS "Enabled memory sanitiser in Release build.")
		message("For address symbolisation you might need to set the environment \
variables ASAN_SYMBOLIZER_PATH and ASAN_OPTIONS=symbolize=1")
	elseif(DRB_SANITIZE_THREAD_Release)
		set(_FLAGSS "${_FLAGSS} -fsanitize=thread")
		message(STATUS "Enabled thread sanitiser in Release build.")
	elseif(DRB_SANITIZE_ADDRESS_Release)
		set(_FLAGSS "${_FLAGSS} -fsanitize=address")
		message(STATUS "Enabled address sanitiser in Release build.")
	elseif(DRB_SANITIZE_UNDEFINED_Release)
		set(_FLAGSS "${_FLAGSS} -fsanitize=undefined")
		message(STATUS "Enabled undefined behaviour sanitiser in Release build.")
	endif()

	# Disable some further optimisations
	enable_if_all_compiles(_FLAGSS dummy "-fno-omit-frame-pointer")
	enable_if_all_compiles(_FLAGSS dummy "-fno-optimize-sibling-calls")

	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${_FLAGSS}")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${_FLAGSS}")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${_FLAGSS}")
	unset(_FLAGSS)
	unset(dummy)

endif()

#
# Cleanup:
#
unset(DRB_HAVE_ANY_SANITIZE)
