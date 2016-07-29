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

cmake_minimum_required(VERSION 3.0.0)

# enable the new version feature introduced in cmake 3.0.0
cmake_policy(SET CMP0048 NEW)
project(krims VERSION 0.0.0)

# Global include directories for this project:
include_directories("${PROJECT_SOURCE_DIR}/src")  # for default sources
include_directories("${PROJECT_SOURCE_DIR}/src")  # for generated sources

# set the standard and enforce it:
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# make sure we no not use the gnu++ extensions:
# this adds c++n instead of gnu++n to the command line
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)

##########################################################################
# Finally install package:
include(cmake/install_package.cmake)
