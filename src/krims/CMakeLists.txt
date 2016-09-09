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
# set the source files.
#
set(KRIMS_SOURCES
	version.cc
	ExceptionSystem/backtrace.cc
	ExceptionSystem/ExceptionBase.cc
	ExceptionSystem/exception_defs.cc
	ParameterMap.cc
)

# Dump the current version as an include file.
include(WriteVersionHeader)
write_version_header("${CMAKE_BINARY_DIR}/src/krims/version_defs.hh")

# Add the libraries for Debug and Release builds:
drb_add_library(krims
	DBGSUFFIX  ".g"
	FILES ${KRIMS_SOURCES}
)

drb_set_target_properties(
	ALL krims
	PROPERTIES
	VERSION "${PROJECT_VERSION}"
)

drb_target_compile_definitions(ALL     krims PUBLIC ${KRIMS_DEFINITIONS})
drb_target_compile_definitions(DEBUG   krims PUBLIC ${KRIMS_DEFINITIONS_DEBUG})
drb_target_compile_definitions(RELEASE krims PUBLIC ${KRIMS_DEFINITIONS_RELEASE})

drb_target_link_libraries(ALL     krims ${KRIMS_DEPENDENCIES})
drb_target_link_libraries(DEBUG   krims ${KRIMS_DEPENDENCIES_DEBUG})
drb_target_link_libraries(RELEASE krims ${KRIMS_DEPENDENCIES_RELEASE})

# set the installation property for these targets:
install(TARGETS ${krims_TARGETS}
	EXPORT KrimsTargets
	DESTINATION lib
	COMPONENT library
)

# install the includes:
install(DIRECTORY .
	DESTINATION "include/krims"
	COMPONENT devel
	FILES_MATCHING PATTERN "*.hh"
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/version_defs.hh"
	DESTINATION "include/krims"
	COMPONENT devel
)

# Export the target specifications for krims.
export(EXPORT KrimsTargets
	FILE "${krims_BINARY_DIR}/krimsTargets.cmake"
	NAMESPACE Upstream::
)

# and install them appropriately:
install(EXPORT KrimsTargets
	FILE "krimsTargets.cmake"
	NAMESPACE Upstream::
	DESTINATION "${PackageModuleLocation}/krims"
)

