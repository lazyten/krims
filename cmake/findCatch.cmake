# Finds and sets up catch under the target name stored in
#        catch_TARGET
# such that just linking against it as a dependency does everything automatically.
#
# In case the catch library is not found and AUTOCHECKOUT_MISSING_LIBS is set to
# on, catch is automatically checked out and built.
# Otherwise a fatal error is produced.
#

#
# Options and properties required
#
option(AUTOCHECKOUT_MISSING_LIBS "Automatically checkout missing libraries" OFF)
set_property(DIRECTORY PROPERTY EP_STEP_TARGETS configure build test)

# The name for the catch target.
# Note that just "catch" cannot be used since rapidcheck (which is checked out later)
# uses this.
set(catch_TARGET "catch_hdr")

#
# -------
#

if (TARGET "${catch_TARGET}")
	message(STATUS "Found target catch, assume catch already configured for build.")
	return()
endif()

# Try to find catch somewhere
find_path(catch_INCLUDE_DIR catch.hpp
	PATHS
	$ENV{catch_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/external/rapidcheck/ext/catch/include
	${PROJECT_SOURCE_DIR}/../catch/include
	${PROJECT_SOURCE_DIR}/../krims/modules/catch/include
	DOC "catch include directory"
)

string(TOUPPER "${PROJECT_NAME}" PROJECT_UPPER)
if ("${catch_INCLUDE_DIR}" STREQUAL "catch_INCLUDE_DIR-NOTFOUND")
	if (AUTOCHECKOUT_MISSING_LIBS)
		include(ExternalProject)
		ExternalProject_Add(catchFromGit
			PREFIX "${PROJECT_BINARY_DIR}/external/catch"
			GIT_REPOSITORY "https://github.com/philsquared/Catch"
			#
			# No configure or build neccessary
			CONFIGURE_COMMAND ""
			BUILD_COMMAND ""
			TEST_COMMAND ""
			INSTALL_COMMAND ""
			# INSTALL_DIR
			# TEST_COMMAND
		)

		# Setup catch target
		add_library(${catch_TARGET} INTERFACE)
		add_dependencies(${catch_TARGET} catchFromGit)
		target_include_directories(${catch_TARGET} INTERFACE "${PROJECT_BINARY_DIR}/external/catch/src/catchFromGit/include")

		# Print message and return
		message(STATUS "Using catch from git repository.")
		return()
	endif()

	message(FATAL_ERROR "Could not find catch include directory. 
Either disable testing of ${PROJECT_NAME} by setting ${PROJECT_UPPER}_ENABLE_TESTS to OFF \
or enable autocheckout via -DAUTOCHECKOUT_MISSING_LIBS=ON \
or provide a hint where the catch include file can be found via \
the environment variable catch_INCLUDE_DIR.")
endif()

# Add library
add_library(${catch_TARGET} INTERFACE IMPORTED)
message(STATUS "Found catch at ${catch_INCLUDE_DIR}/catch.hpp")
include_directories(${catch_INCLUDE_DIR})
