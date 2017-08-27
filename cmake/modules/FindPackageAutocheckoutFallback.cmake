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

#
# Finds and sets up a package called "NAME" under the target names stored in
#      ${NAME}_DEBUG_TARGET     (Debug version)
#      ${NAME}_RELEASE_TARGET   (Release version)
# such that just linking against it as a dependency does everything
# automatically.
#
# In case the packages are not found and AUTOCHECKOUT_MISSING_LIBS is set to
# ON, the package is automatically checked out and built along with the current project.
# Otherwise a fatal error is produced.
#
# For this this module assumes that "bla" can be checked out with a script called
# ${PROJECT_SOURCE_DIR}/external/get_bla.sh
#
# if AUTOCHECKOUT_FORCED is set to on, sturmint is always checked out, even if it
# is found on the system.
#
# This module is closely related to the krims cmake module
# IncludeKrimsCmakeModule in the sense that the two use largely the same
# options and the mechanism of finding the krims module are extremly similar.
# This should be taken into account when updating one of the two modules.
#

#
# Options and properties required
#
option(AUTOCHECKOUT_MISSING_REPOS "Automatically checkout missing repositories" OFF)
option(AUTOCHECKOUT_FORCED
	"Force autocheckout of repositories, even if they are present on the system. \
Implies AUTOCHECKOUT_MISSING_REPOS set to ON" OFF)

#
# -------
#

if (NOT DRB_INITIALISED)
	message(FATAL_ERROR "The FindPackageAutocheckoutFallback module requires \
the DebugReleaseBuild module to be included and set up. \
Please include the module and call 'drb_init()' first.")
endif()


function(autocheckout_repo NAME MIN_VERSION)
	execute_process(
		COMMAND "sh" "get_${NAME}.sh"
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/external"
		RESULT_VARIABLE RES
	)
	if (NOT RES EQUAL 0)
		message(FATAL_ERROR "Getting ${NAME} failed with error: ${RES}")
	endif()

	# Proceed to configure the external repo
	add_subdirectory(${PROJECT_SOURCE_DIR}/external/${NAME})
	include_directories(${PROJECT_SOURCE_DIR}/external/${NAME}/src)
	include_directories(${PROJECT_BINARY_DIR}/external/${NAME}/src)

	# Extract version from CMakeLists.txt:
	file(STRINGS "${PROJECT_SOURCE_DIR}/external/${NAME}/CMakeLists.txt"
		VERSION_RAW
		REGEX "${NAME} VERSION [0-9.]+"
		LIMIT_COUNT 1)
	string(REGEX MATCH "[0-9.]+" GOT_VERSION "${VERSION_RAW}")

	# Compare against what is needed
	if("${GOT_VERSION}" VERSION_LESS "${MIN_VERSION}")
		message(FATAL_ERROR "Inconsistency in the repo: \
Version ${MIN_VERSION} of ${NAME} was requested, but only version ${GOT_VERSION} \
was found.")
	endif()

	set(${NAME}_DIR "${NAME}_DIR-AUTOCHECKOUT"
		CACHE STRING "Directory where ${NAME} was found.")
endfunction()

function(find_package_autocheckout_fallback NAME MIN_VERSION)
	# Options sanity test
	if (AUTOCHECKOUT_FORCED)
		set(AUTOCHECKOUT_MISSING_REPOS ON
			CACHE BOOL "Automatically checkout missing repositories" FORCE)
	endif()

	# Are we already done somewhere else?
	if (TARGET "${${NAME}_DEBUG_TARGET}"  OR TARGET "${${NAME}_RELEASE_TARGET}")
		message(STATUS "Found ${NAME} targets, assume ${NAME} already configured for build.")
		return()
	endif()

	if ("${${NAME}_DIR}" STREQUAL "${NAME}_DIR-AUTOCHECKOUT")
		# Last time we did an autocheckout ... we should stick with this
		# (This is done to make sure that we do not find the version we
		#  installed ourselves)
		autocheckout_repo(${NAME} ${MIN_VERSION})
		return()
	endif()

	if (NOT AUTOCHECKOUT_FORCED)
		find_package(${NAME} ${MIN_VERSION} QUIET CONFIG)
		mark_as_advanced(${NAME}_DIR)
	endif()

	if ("${${NAME}_DIR}" STREQUAL "${NAME}_DIR-NOTFOUND" OR AUTOCHECKOUT_FORCED)
		if (AUTOCHECKOUT_MISSING_REPOS)
			autocheckout_repo(${NAME} ${MIN_VERSION})
			return()
		endif()

		message(FATAL_ERROR "Could not find ${NAME} library.
Either provide the installation prefix of the ${NAME} library in the environment \
variable ${NAME}_DIR or enable autocheckout via -DAUTOCHECKOUT_MISSING_REPOS=ON.")
	endif()

	# Setup library targets
	set(${NAME}_DEBUG_TARGET   "Upstream::${NAME}.g"
		CACHE INTERNAL "Target name of debug version of ${NAME}")
	set(${NAME}_RELEASE_TARGET "Upstream::${NAME}"
		CACHE INTERNAL "Target name of release version of ${NAME}")

	# Check that all required targets are available.
	foreach(build ${DRB_BUILD_TYPES})
		if(NOT TARGET "${${NAME}_${build}_TARGET}")
			message(FATAL_ERROR "We could not find a ${build} version of ${NAME} at this location. \
Either disable building a ${build} version of ${CMAKE_PROJECT_NAME} or else \
rebuild ${NAME} with a ${build} version as well.")
		endif()
	endforeach()

	message(STATUS "Found ${NAME} config at ${${NAME}_CONFIG}")
endfunction()
