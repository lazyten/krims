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

#
# Setup build type for examples
#
if (CMAKE_BUILD_TYPE MATCHES "Release")
	set(EXAMPLES_BUILD_TYPE "RELEASE")
else()
	set(EXAMPLES_BUILD_TYPE "DEBUG")
endif()

#
# macro to setup an example target
#
macro(setup_example_target TARGET)
	set_target_properties(
		${TARGET}
		PROPERTIES
		#${CMAKE_EXE_LINKER_FLAGS} are added
		LINK_FLAGS "${CMAKE_EXE_LINKER_FLAGS_${EXAMPLES_BUILD_TYPE}}"
		LINKER_LANGUAGE "CXX"
		#${CMAKE_CXX_FLAGS} are added automaticaly
		COMPILE_FLAGS "${CMAKE_CXX_FLAGS_${EXAMPLES_BUILD_TYPE}}"
		COMPILE_DEFINITIONS "${EXAMPLES_BUILD_TYPE}"
	)

	# setup definitions.
	target_compile_definitions(${TARGET} PRIVATE "${KRIMS_DEFINITIONS};${KRIMS_DEFINITIONS_${EXAMPLES_BUILD_TYPE}}")

	# link it with the appropriate krims library target
	target_link_libraries(${TARGET}
		${krims_${EXAMPLES_BUILD_TYPE}_TARGET}
		${KRIMS_DEPENDENCIES}
		${KRIMS_DEPENDENCIES_${EXAMPLES_BUILD_TYPE}}
	)
endmacro()

#
# Include subdirectories:
#
add_subdirectory(ParameterMap_demo)
