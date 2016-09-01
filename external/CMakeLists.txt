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

if (BUILD_EXTERNAL_RAPIDCHECK)
	#
	# Add subdirectory
	#

	# Check wether rapidcheck is there
	if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/rapidcheck/CMakeLists.txt")
		message(FATAL_ERROR "Could not find rapidcheck at expected place. Try a manual git checkout.")
	endif()

	# Set option such that rapidcheck tests are built.
	set(RC_ENABLE_TESTS ON CACHE BOOL "Build RapidCheck tests")

	# Change compiler flags (CMAKE_CXX_FLAGS) to make fresh build config
	set(CMAKE_CXX_FLAGS_STORED_TMP ${CMAKE_CXX_FLAGS})
	set(CMAKE_CXX_FLAGS "")
	#enable_if_compiles(CMAKE_CXX_FLAGS "-Wno-gnu-zero-variadic-macro-arguments")

	# Add the rapidcheck subdirectory and configure its built.
	add_subdirectory(rapidcheck)

	# undo the changes to the compiler flags:
	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_STORED_TMP})
	unset(CMAKE_CXX_FLAGS_STORED_TMP)

#	#
#	# Install rapidcheck
#	#
#
#	# Export definitions:
#	export(TARGETS rapidcheck
#		NAMESPACE Upstream::
#		FILE "${krims_BINARY_DIR}/rapidcheckTargets.cmake"
#	)
#
#	# And write a config file:
#	file(WRITE "${krims_BINARY_DIR}/rapidcheckConfig.cmake"
#		"include(\"\${CMAKE_CURRENT_LIST_DIR}/rapidcheckTargets.cmake\")"
#	)
#
#	if(KRIMS_INSTALL_RAPIDCHECK)
#		if(BUILD_SHARED_LIBS)
#			install(FILES "${rapidcheck_BINARY_DIR}/librapidcheck.so"
#				DESTINATION lib
#				COMPONENT library-rapidcheck
#			)
#		else()
#			install(FILES "${rapidcheck_BINARY_DIR}/librapidcheck.a"
#				DESTINATION lib
#				COMPONENT library-rapidcheck
#			)
#		endif()
#
#		# install the includes:
#		install(DIRECTORY "${rapidcheck_SOURCE_DIR}/include"
#			DESTINATION "."
#			COMPONENT devel-rapidcheck
#			FILES_MATCHING REGEX ".*\\.(h|hpp)$"
#		)
#
#		# TODO this does not work, since we cannot define the export group
#		#      for the rapidcheck target.
#		# and install them appropriately:
#		#install(EXPORT rapidcheck
#		#	FILE "rapidcheckTargets.cmake"
#		#	NAMESPACE Upstream::
#		#	DESTINATION "${PackageModuleLocation}/rapidcheck"
#		#)
#
#		message(STATUS "Enabled installation of external rapidcheck library.")
#	endif()
#
#	# For downstream compatibility:
#	add_library(common_rapidcheck ALIAS rapidcheck)
endif()
