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

# Check we have at least the required version:
cmake_minimum_required(VERSION 3.0.0)

# Set basedir for this module
set(DRB_DIR "${CMAKE_CURRENT_LIST_DIR}/DebugReleaseBuild"
	CACHE INTERNAL "Base directory of the DebugReleaseBuild module.")

# include other files:
include("${DRB_DIR}/drb_init.cmake")
include("${DRB_DIR}/drb_utils.cmake")
include("${DRB_DIR}/drb_setup_compiler_flags.cmake")
include("${DRB_DIR}/drb_targets.cmake")
