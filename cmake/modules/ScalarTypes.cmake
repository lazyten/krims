## ---------------------------------------------------------------------
##
## Copyright (C) 2017 by the krims authors
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

# This module provides functionality in order to manage the scalar types
# which are used to perform numeric calculations.
# It automatically determines required headers and searches for required
# dependencies if non-standard scalar types are chosen.

#
# Options
#
set(TYPES_REAL      "double" CACHE STRING "The default real type.")
set(TYPES_COMPLEX   "std::complex<\@TYPES_REAL\@>" CACHE STRING
	"The default complex type. @VAR@ variables are substituted."
)

#
# ---------------------------------------------------------------------
#

include(CMakeParseArguments)
function(setup_scalar_types)
	# Setup the scalar types for this project. Reads the current values
	# of the cache variables TYPES_REAL and TYPES_COMPLEX and determines
	# the headers and includes for these types. The determined values
	# are written to the following variables:
	#     SCALAR_TYPES_HEADERS        The extra #include <header> statements needed
	#     SCALAR_TYPES_REAL           The full identifier for the real type
	#     SCALAR_TYPES_COMPLEX        The full identifier for the complex type
	#     SCALAR_TYPES_LIBRARIES      The extra library targets one needs to link with

	set(options )
	set(oneValueArgs FORCE_REAL FORCE_COMPLEX)
	set(multiValueArgs )
	cmake_parse_arguments(TYPES "${options}" "${oneValueArgs}" "${multiValueArgs}"  ${ARGN})

	if(TYPES_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unknown keyword: \"${SCT_UNPARSED_ARGUMENTS}\"")
	endif()

	# Copy types from global cache or use the forced types:
	set(REAL "${TYPES_REAL}")
	set(COMPLEX "${TYPES_COMPLEX}")
	if(NOT "${TYPES_FORCE_REAL}" STREQUAL "")
		set(REAL ${TYPES_FORCE_REAL})
	endif()
	if(NOT "${TYPES_FORCE_COMPLEX}" STREQUAL "")
		set(COMPLEX ${TYPES_FORCE_COMPLEX})
	endif()

	# @configure the type strings:
	string(CONFIGURE "${REAL}"    TMP_REAL    @ONLY)
	string(CONFIGURE "${COMPLEX}" TMP_COMPLEX @ONLY)

	# Deduce extra headers, include dirs and libraries to use
	set(TMP_HEADERS "")
	SET(TMP_LIBRARIES "")
	if (TMP_COMPLEX MATCHES "std::complex")
		set(TMP_HEADERS "${TMP_HEADERS}#include <complex>\n")
	endif()

	# Set internal cache variables
	set(SCALAR_TYPES_COMPLEX     "${TMP_COMPLEX}"     PARENT_SCOPE)
	set(SCALAR_TYPES_REAL        "${TMP_REAL}"        PARENT_SCOPE)
	set(SCALAR_TYPES_HEADERS     "${TMP_HEADERS}"     PARENT_SCOPE)
	set(SCALAR_TYPES_LIBRARIES   "${TMP_LIBRARIES}"   PARENT_SCOPE)
endfunction()
