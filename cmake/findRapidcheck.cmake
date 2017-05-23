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

# Finds and sets up rapidcheck under the target name stored in the variable
#     rapidcheck_TARGET
# such that just linking against it as a dependency does everything
# automatically.
#
# In case the rapidcheck library is not found and AUTOCHECKOUT_MISSING_LIBS is
# set to ON, rapidcheck is automatically checked out into the external
# subdirectory and configured for building.
#
# Otherwise a fatal error is produced.
#

#
# Options and properties required
#
option(AUTOCHECKOUT_MISSING_REPOS "Automatically checkout missing repositories" OFF)

#
# -------
#

set(rapidcheck_TARGET "rapidcheck")
if (TARGET "${rapidcheck_TARGET}")
	message(STATUS "Found target rapidcheck, assume rapidcheck already configured for build.")
	return()
endif()

# Try to find rapidcheck somewhere
find_package(rapidcheck QUIET CONFIG)
mark_as_advanced(rapidcheck_DIR)

string(TOUPPER "${PROJECT_NAME}" PROJECT_UPPER)
if ("${rapidcheck_DIR}" STREQUAL "rapidcheck_DIR-NOTFOUND")
	if (AUTOCHECKOUT_MISSING_REPOS)
		execute_process(
			COMMAND "sh" "get_rapidcheck.sh"
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/external"
			RESULT_VARIABLE RES
		)
		if (NOT RES EQUAL 0)
			message(FATAL_ERROR "Getting rapidcheck from git failed with error: ${RES}")
		endif()

		#
		# Proceed to configure rapidcheck
		#
		# Change compiler flags (CMAKE_CXX_FLAGS) to make fresh build config
		set(CMAKE_CXX_FLAGS_STORED_TMP ${CMAKE_CXX_FLAGS})
		set(CMAKE_CXX_FLAGS "")

		if (CMAKE_CXX_COMPILER_ID MATCHES "GNU"
				AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "6.0")
			# TODO Problems in rapidcheck's code
			#      Fix them some day
			enable_if_compiles(CMAKE_CXX_FLAGS "-Wno-error=shift-negative-value")
			enable_if_compiles(CMAKE_CXX_FLAGS_STORED_TMP "-Wno-error=shift-negative-value")
			enable_if_compiles(CMAKE_CXX_FLAGS "-Wno-error=misleading-indentation")
			enable_if_compiles(CMAKE_CXX_FLAGS_STORED_TMP "-Wno-error=misleading-indentation")
		elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
			enable_if_compiles(CMAKE_CXX_FLAGS "-stdlib=${DRB_CXX_STANDARD_LIBRARY}")
		endif()

		# Add the rapidcheck subdirectory and configure its built.
		add_subdirectory(${PROJECT_SOURCE_DIR}/external/rapidcheck)

		# undo the changes to the compiler flags:
		set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_STORED_TMP})
		unset(CMAKE_CXX_FLAGS_STORED_TMP)

		return()
	endif()

	message(FATAL_ERROR "Could not find rapidcheck library.
Either disable testing of ${PROJECT_NAME} by setting ${PROJECT_UPPER}_ENABLE_TESTS to OFF \
or enable autocheckout via '-DAUTOCHECKOUT_MISSING_REPOS=ON'.")
endif()

message(WARNING "This part of findRapidcheck has never been tested.")

# Add library
set(rapidcheck_TARGET "Upstream::rapidcheck")
message(STATUS "Found rapidcheck config at ${rapidcheck_CONFIG}")
