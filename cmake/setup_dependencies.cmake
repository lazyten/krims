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

# sets these things
#
#       KRIMS_DEPENDENCIES			everyone needs these libraries
#       KRIMS_DEPENDENCIES_DEBUG		debug mode needs these extras
#       KRIMS_DEPENDENCIES_RELEASE		release mode needs these extras
#       KRIMS_DEPENDENCIES_TEST		tests need these extra libraries
#
#       KRIMS_DEFINITIONS			definitions for all compilation
#       KRIMS_DEFINITIONS_DEBUG		definitions for debug mode
#       KRIMS_DEFINITIONS_RELEASE		definitions for release mode
#

####################
#-- Empty it all --#
####################
set(KRIMS_DEPENDENCIES "")
set(KRIMS_DEPENDENCIES_DEBUG "")
set(KRIMS_DEPENDENCIES_RELEASE "")
set(KRIMS_DEPENDENCIES_TEST "")
set(KRIMS_DEFINITIONS "")
set(KRIMS_DEFINITIONS_DEBUG "")
set(KRIMS_DEFINITIONS_RELEASE "")

########################################
#-- Link with some threading library --#
########################################
if (CMAKE_VERSION VERSION_GREATER 3.1.0)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	set(KRIMS_DEPENDENCIES ${KRIMS_DEPENDENCIES} Threads::Threads)
else()
	set(CMAKE_THREAD_PREFER_PTHREAD ON)
	find_package(Threads REQUIRED)
	set(KRIMS_DEPENDENCIES ${KRIMS_DEPENDENCIES} ${CMAKE_THREAD_LIBS_INIT})
endif()


############################
#-- rapidcheck and catch --#
############################
if (KRIMS_ENABLE_TESTS)
	# We need to setup rapidcheck and catch for the tests:
	include(cmake/findRapidcheck.cmake)
	set(KRIMS_DEPENDENCIES_TEST ${KRIMS_DEPENDENCIES_TEST} ${rapidcheck_TARGET})

	include(cmake/findCatch.cmake)
	set(KRIMS_DEPENDENCIES_TEST ${KRIMS_DEPENDENCIES_TEST} ${catch_TARGET})
endif()
