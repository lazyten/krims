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
# The sources for the test executable
#
set(KRIMS_TESTS_SOURCES
	ExceptionTests.cc
	SubscriptionTests.cc
	main.cc
)

drb_add_executable(krims_tests
	DBGSUFFIX  "_DEBUG"
	RELSUFFIX  "_RELEASE"
	FILES ${KRIMS_TESTS_SOURCES}
)

drb_target_compile_definitions(ALL     krims_tests
	PRIVATE ${KRIMS_DEFINITIONS})
drb_target_compile_definitions(DEBUG   krims_tests
	PRIVATE ${KRIMS_DEFINITIONS_DEBUG})
drb_target_compile_definitions(RELEASE krims_tests
	PRIVATE ${KRIMS_DEFINITIONS_RELEASE})

drb_target_link_libraries(ALL     krims_tests
	${KRIMS_DEPENDENCIES} ${KRIMS_DEPENDENCIES_TEST})
drb_target_link_libraries(DEBUG   krims_tests
	${krims_DEBUG_TARGET} ${KRIMS_DEPENDENCIES_DEBUG})
drb_target_link_libraries(RELEASE krims_tests
	${krims_RELEASE_TARGET} ${KRIMS_DEPENDENCIES_RELEASE})

# Add the test executables
FOREACH(target ${krims_tests_TARGETS})
	add_test(NAME ${target} COMMAND ${target})
ENDFOREACH()
