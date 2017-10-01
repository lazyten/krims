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
# Module which sets up the environment for building swig interfaces
# and provides some functions to assist in building the modules.
# Implicitly requires the DebugReleaseBuild module to be loaded.
#


include(CMakeParseArguments)

if (NOT DRB_INITIALISED)
	message(FATAL_ERROR "The CppInterface module requires the DebugReleaseBuild \
module to be included and set up. Please include the module and call 'drb_init()'")
endif()

# Check for SWIG, at least 2.0.11
find_package(SWIG 2.0.11 REQUIRED)
include(${SWIG_USE_FILE})

# Setup build type for the interface
set(INTERFACE_BUILD_TYPE "Release" CACHE STRING
	"Build type to use for the interfaces to other languages (e.g. python). \
Switch this to Debug in order to use the Debug version of the backend library from \
e.g. the python interface."
)
if (NOT CMAKE_BUILD_TYPE MATCHES "Release")
	set(INTERFACE_BUILD_TYPE "Debug")
endif()
if (NOT ${INTERFACE_BUILD_TYPE} STREQUAL "Debug"
	AND NOT ${INTERFACE_BUILD_TYPE} STREQUAL "Release")
	message(FATAL_ERROR "INTERFACE_BUILD_TYPE needs to be either 'Release' or 'Debug'")
endif()
if (NOT CMAKE_BUILD_TYPE MATCHES "${INTERFACE_BUILD_TYPE}")
	message(FATAL_ERROR "INTERFACE_BUILD_TYPE cannot be something which is not built via CMAKE_BUILD_TYPE")
endif()


if (NOT BUILD_SHARED_LIBS)
	# We need even static library code to be position independent
	# as most modules are compiled and integrated with 3rd party
	# interpreters as shared modules
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()


function(cpp_interface_python3 MODULE_NAME)
	# SOURCES       The swig and cpp files to parse and compile into the final module
	# LINK_TARGETS  The targets to link the resulting module to.
	#               These are processed via DebugReleaseBuild, i.e. the correct
	#               target name for the build configuration matching the value of 
	#               INTERFACE_BUILD_TYPE (Debug or Release) is inserted automatically
	#
	# Generates the files
	#      ${CMAKE_CURRENT_BINARY_DIR}/_${MODULE_NAME}.so
	#      ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}.py
	# which are subsequently available for download
	#

	set(options )
	set(oneValueArgs)
	set(multiValueArgs SOURCES LINK_TARGETS)
	cmake_parse_arguments(CIP "${options}" "${oneValueArgs}" "${multiValueArgs}"  ${ARGN})

	if(CIP_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unknown keyword: \"${CIP_UNPARSED_ARGUMENTS}\"")
	endif()

	# Make sure all interface files are compiled as C++
	foreach(infile ${CIP_SOURCES})
		if(infile MATCHES "\\.i$")
			set_source_files_properties(${infile} PROPERTIES CPLUSPLUS ON)
		endif()
	endforeach()

	# Search for both interpreter and libraries, since both need to
	# be consistent with another
	set(Python_ADDITIONAL_VERSIONS 3)
	find_package(PythonInterp REQUIRED)
	find_package(PythonLibs   REQUIRED)
	include_directories(${PYTHON_INCLUDE_PATH})

	# Setup swig library/module
	set(CMAKE_SWIG_FLAGS "")
	if (CMAKE_VERSION VERSION_GREATER 3.8)
		swig_add_library(${MODULE_NAME} LANGUAGE python SOURCES ${CIP_SOURCES})
	else()
		# Older version of the call, deprecated from cmake 3.9
		swig_add_module(${MODULE_NAME} python ${CIP_SOURCES})
	endif()

	# Compiler flags for SWIG
	enable_if_compiles(SWIG_COMPILE_FLAGS -Wno-missing-field-initializers)
	enable_if_compiles(SWIG_COMPILE_FLAGS -Wno-pedantic)
	enable_if_compiles(SWIG_COMPILE_FLAGS -Wno-old-style-cast)
	enable_if_compiles(SWIG_COMPILE_FLAGS -Wno-sign-compare)
	enable_if_compiles(SWIG_COMPILE_FLAGS -Wno-maybe-uninitialized)
	enable_if_compiles(SWIG_COMPILE_FLAGS -Wno-deprecated)
	enable_if_compiles(SWIG_COMPILE_FLAGS -Wno-error)
	set_source_files_properties(
		${swig_generated_file_fullname}
		PROPERTIES COMPILE_FLAGS "${SWIG_COMPILE_FLAGS}"
	)

	# Determine build type:
	if (INTERFACE_BUILD_TYPE STREQUAL "Debug")
		set(BUILD_TYPE "DEBUG")
	else()
		set(BUILD_TYPE "RELEASE")
	endif()

	# This is needed such that the appropriate parts in included headers
	# are switched on in accordance with what DebugReleaseBuild would do
	# automatically as well.
	target_compile_definitions(${SWIG_MODULE_${MODULE_NAME}_REAL_NAME}
		PRIVATE ${BUILD_TYPE}=${BUILD_TYPE}
	)

	# Link with the appropriate build type targets
	set(link "")
	foreach(target ${CIP_LINK_TARGETS})
		set(link ${link} ${${target}_${BUILD_TYPE}_TARGET})
	endforeach()
	swig_link_libraries(${MODULE_NAME} ${PYTHON_LIBRARIES} ${link})
endfunction()

if (DRB_SANITIZE_ADDRESS_Release)
	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		set(HINTPATH
		"/usr/lib/clang/${CMAKE_CXX_COMPILER_VERSION}/lib/linux/libclang_rt.asan-x86_64.so")
	elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
		set(HINTPATH "/usr/lib/x86_64-linux-gnu/libasan.so.4")
	endif()

	message("   # Address sanitiser hint
   #
   For getting the address sanitiser to work from python try running
   python with LD_PRELOAD=<path to asan.so>, e.g. something like
   LD_PRELOAD=${HINTPATH}
   #")
endif()
