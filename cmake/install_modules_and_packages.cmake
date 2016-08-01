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

# Installs the cmake modules and cmake package information this project
# provides
#
# Requires the variable PackageModuleLocation to be set.

# Installing cmake modules
install(DIRECTORY "${krims_SOURCE_DIR}/cmake/modules"
	DESTINATION ${PackageModuleLocation}
	COMPONENT devel
	FILES_MATCHING PATTERN "*.cmake"
)

# Write a basic version file for krims
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${krims_BINARY_DIR}/krimsConfigVersion.cmake"
	COMPATIBILITY AnyNewerVersion
)

# Adjust a configure file
configure_file(cmake/krimsConfig.cmake.in
	"${krims_BINARY_DIR}/krimsConfig.cmake"
	COPYONLY
)

# Set an export location:
install(FILES
	"${krims_BINARY_DIR}/krimsConfig.cmake"
	"${krims_BINARY_DIR}/krimsConfigVersion.cmake"
	DESTINATION "${PackageModuleLocation}/krims"
	COMPONENT devel
)

