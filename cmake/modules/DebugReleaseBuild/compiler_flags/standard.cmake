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

# Compiler flags for the default compilers, i.e. gnu or clang


#######################
#--  Warning policy --#
#######################

# The warning policy we use here is based on a very valuable
# post by a clang developer on stack exchange
# https://programmers.stackexchange.com/questions/122608#124574

# Show high confidence warning
enable_if_compiles(CMAKE_CXX_FLAGS "-Wall")

# Show valuable extra warnings
enable_if_compiles(CMAKE_CXX_FLAGS "-Wextra")

# Turn on warnings about language extensions
enable_if_compiles(CMAKE_CXX_FLAGS "-pedantic")

# But silence some rather annoying warnings
enable_if_compiles(CMAKE_CXX_FLAGS "-Wno-unused-macros")
enable_if_compiles(CMAKE_CXX_FLAGS "-Wno-unused-parameter")

# Make warnings errors, such that we cannot ignore them
enable_if_compiles(CMAKE_CXX_FLAGS "-Werror")

#######################
#-- Bug workarounds --#
#######################
#include(${DRB_DIR}/workarounds/missing_gets_bug.cmake)

##############
#--  Debug --#
##############
#
# Extra stuff for debug:
#
if (CMAKE_BUILD_TYPE MATCHES "Debug")
	enable_if_compiles(CMAKE_CXX_FLAGS_DEBUG "-O0")
	enable_if_compiles(CMAKE_CXX_FLAGS_DEBUG "-Og")

	# Common linker flags for all of debug:
	set(COMMON_LINKER_FLAGS_DEBUG "${COMMON_LINKER_FLAGS_DEBUG} -g")
	set(COMMON_LINKER_FLAGS_DEBUG "${COMMON_LINKER_FLAGS_DEBUG} -ggdb")

	set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} ${COMMON_LINKER_FLAGS_DEBUG}")
	set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} ${COMMON_LINKER_FLAGS_DEBUG}")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${COMMON_LINKER_FLAGS_DEBUG}")

	unset(COMMON_LINKER_FLAGS_DEBUG)
endif()

################
#--  Release --#
################
#
# Extra stuff for release:
#
if (CMAKE_BUILD_TYPE MATCHES "Release")
	# nothing atm
endif()
