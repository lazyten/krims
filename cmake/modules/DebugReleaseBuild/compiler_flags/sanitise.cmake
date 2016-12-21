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

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# Cannot produce sanitisation analysis for any compiler but clang
	return()
endif()

#
# Options
#

option(DRB_SANITIZE_MEMORY_Release "Enable detection of unitialised memory reads in RELEASE build.")
option(DRB_SANITIZE_ADDRESS_Release "Enable detection of memory errors (Out-of-bounds, use-after-free, ...) in RELEASE build.")
option(DRB_SANITIZE_THREAD_Release "Enable detection of data races introduced by multi-threading in the RELEASE build.")

set(DRB_SANITIZE_OPTIONS DRB_SANITIZE_MEMORY_Release DRB_SANITIZE_THREAD_Release DRB_SANITIZE_ADDRESS_Release)

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
unset(DRB_HAVE_ANY_SANITIZE)

#
# Sanitize flags -- Release
#
if (CMAKE_BUILD_TYPE MATCHES "Release")
	# Reduce optimisation level to 1
	string(REGEX REPLACE "-O[02-9]" "-O1" RELEASE_FLAGS_REPLACED "${CMAKE_CXX_FLAGS_RELEASE}")
	set(CMAKE_CXX_FLAGS_RELEASE "${RELEASE_FLAGS_REPLACED}")
	unset(RELEASE_FLAGS_REPLACED)

	# Set flags for sanitizer:
	set(_FLAGSS "-g")

	if(DRB_SANITIZE_MEMORY_Release)
		set(_FLAGSS "${_FLAGSS} -fsanitize=memory")
		enable_if_compiles(_FLAGSS "-fsanitize-memory-track-origin")
		message(STATUS "Enabled memory sanitiser in Release build.")
	elseif(DRB_SANITIZE_THREAD_Release)
		set(_FLAGSS "${_FLAGSS} -fsanitize=thread")
		message(STATUS "Enabled thread sanitiser in Release build.")
	elseif(DRB_SANITIZE_ADDRESS_Release)
		set(_FLAGSS "${_FLAGSS} -fsanitize=address")
		message(STATUS "Enabled address sanitiser in Release build.")
	endif()

	# Disable some further optimisations
	enable_if_compiles(_FLAGSS "-fno-omit-frame-pointer")
	enable_if_compiles(_FLAGSS "-fno-optimize-sibling-calls")

	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${_FLAGSS}")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${_FLAGSS}")
	unset(_FLAGSS)
endif()
