# Finds and sets up rapidcheck under the target name stored in
#     rapidcheck_TARGET
# such that just linking against it as a dependency does everything 
# automatically.
#
# In case the rapidcheck library is not found and AUTOCHECKOUT_MISSING_LIBS is set to
# on, rapidcheck is automatically checked out and built.
# Otherwise a fatal error is produced.
#

#
# Options and properties required
#
option(AUTOCHECKOUT_MISSING_LIBS "Automatically checkout missing libraries" OFF)
set(BUILD_EXTERNAL_RAPIDCHECK bool off
	cache internal
	"Should the rapidcheck folder in the external project dir be build.")

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
string(TOUPPER "${PROJECT_NAME}" PROJECT_UPPER)
if ("${rapidcheck_DIR}" STREQUAL "rapidcheck_DIR-NOTFOUND")
	if (AUTOCHECKOUT_MISSING_LIBS)
		execute_process(
			COMMAND "sh" "get_rapidcheck.sh"
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/external"
			RESULT_VARIABLE RES
		)
		if (NOT RES EQUAL 0)
			message(FATAL_ERROR "Getting rapidcheck from git failed with error: ${RES}")
		endif()

		# Print message and return
		set(BUILD_EXTERNAL_RAPIDCHECK on)
		return()
	endif()

	message(FATAL_ERROR "Could not find rapidcheck library.
Either disable testing of ${PROJECT_NAME} by setting ${PROJECT_UPPER}_ENABLE_TESTS to OFF \
or enable autocheckout via -DAUTOCHECKOUT_MISSING_LIBS=ON.")
endif()

message(WARNING "This part of findRapidcheck has never been tested.")

# Add library
set(rapidcheck_TARGET "Upstream::rapidcheck")
message(STATUS "Found rapidcheck config at ${rapidcheck_CONFIG}")
