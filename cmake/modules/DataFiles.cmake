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

# Module to manage the downloading and finding of files contaning
# static data required to run a program.
#
# including this module will set the following variables:
#   PROJECT_DATA_DOWNLOAD_DIR     The directory to which static data should
#                                 be downloaded
#   DATA_INSTALL_DIR              The directory to which static data should
#                                 be installed


#
# Options
#
# The directory to which data is downloaded during the build process.
set(DATA_DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/data" CACHE INTERNAL
	"Directory to which static data files are downloaded during the build process.")

#
# ---------------------------------------------------------------------
#

set(DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}")
set(PROJECT_DATA_DOWNLOAD_DIR "${DATA_DOWNLOAD_DIR}/${PROJECT_NAME}")

# TODO Helpful functions for downloading and installing data files.
