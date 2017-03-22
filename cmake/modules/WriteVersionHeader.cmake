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

include(CMakeParseArguments)

function(WRITE_VERSION_HEADER _filename)
	# Write a header file which defines the macro contstants
	# ${NAME}_VERSION_MAJOR, ${NAME}_VERSION_MINOR and
	# ${NAME}_VERSION_PATCH according to the current 
	# project's version and name or by a name and version
	# supplied via the VERSION and NAME flags.


	set(options )
	set(oneValueArgs VERSION NAME)
	set(multiValueArgs )

	cmake_parse_arguments(WVH "${options}" "${oneValueArgs}" "${multiValueArgs}"  ${ARGN})

	if(WVH_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unknown keywords given to WRITE_VERSION_HEADER: \"${WVH_UNPARSED_ARGUMENTS}\"")
	endif()

	if("${WVH_VERSION}" STREQUAL "")
		if ("${PROJECT_VERSION}" STREQUAL "")
			message(FATAL_ERROR "No VERSION specified for WRITE_VERSION_HEADER()")
		else()
			set(WVH_VERSION "${PROJECT_VERSION}")
		endif()
	endif()

	if("${WVH_NAME}" STREQUAL "")
		if ("${PROJECT_NAME}" STREQUAL "")
			message(FATAL_ERROR "No NAME specified for WRITE_VERSION_HEADER()")
		else()
			set(WVH_NAME "${PROJECT_NAME}")
		endif()
	endif()

	# Split version into individual vars:
	string(REPLACE "." ";" VERSION_LIST ${WVH_VERSION})
	list(GET VERSION_LIST 0 WVH_MAJOR)
	list(GET VERSION_LIST 1 WVH_MINOR)
	list(GET VERSION_LIST 2 WVH_PATCH)

	# The content to write to disk:
	set(CONTENT_TO_WRITE
"#pragma once
namespace ${WVH_NAME} {
    namespace detail {
    static int constexpr __version_var_major{${WVH_MAJOR}};
    static int constexpr __version_var_minor{${WVH_MINOR}};
    static int constexpr __version_var_patch{${WVH_PATCH}};
    } // namespace detail
} // namespace ${WVH_NAME}
")

	# if we have a file already, check for changes first
	if(EXISTS "${_filename}")
		file(READ "${_filename}" ORIG_CONTENT)
		if ("${ORIG_CONTENT}" STREQUAL "${CONTENT_TO_WRITE}")
			# no need to write => timestamps stay unchanged
			return()
		endif()
	endif()

	# Now dump the content:
	file(WRITE "${_filename}" "${CONTENT_TO_WRITE}")
endfunction()
