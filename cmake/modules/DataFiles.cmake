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
# including this module will set the following variables inside internal cache
#   ${PROJECT_NAME}_DATA_DOWNLOAD_DIR
#       The directory to which static data will be downloaded
#   ${PROJECT_NAME}_DATA_INSTALL_DIR
#       The directory to which static data will be installed


#
# Options
#
# The directory to which data is downloaded during the build process.
set(DATA_DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/data" CACHE PATH
	"Directory to which static data files are downloaded during the build process.")

#
# ---------------------------------------------------------------------
#

set(${PROJECT_NAME}_DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/data"
	CACHE INTERNAL "The directory where files for the project ${PROJECT_NAME} are installed")
set(${PROJECT_NAME}_DATA_DOWNLOAD_DIR "${DATA_DOWNLOAD_DIR}/${PROJECT_NAME}"
	CACHE INTERNAL "The directory where files for the project ${PROJECT_NAME}\
	are downloaded to during the build process")

function(data_download_target TARGET URL HASHFULL)
	# Add a target which downloads a static data tar.gz file
	# available under the remote location URL. The downloaded
	# content is verified using the hash HASHFULL, which should
	# be provided in the form ALGO=value, e.g. SHA256=abcde123456 ...
	#
	# The first argument is the name of the target to be created
	# The second argument is the url of the tarball and the third
	# is the hash.
	#
	# The target is given the "ALL" tag, such that it will
	# be executed if the "all" target is built.
	#

	# The hash without the ALGO= prefix
	string(REGEX REPLACE "^[A-Za-z0-9]+=" "" hash ${HASHFULL})

	# Get the extension of the tarball and the remote host
	get_filename_component(extension "${URL}" EXT)
	string(REGEX MATCH "/[^/]+/" tmp ${URL})
	string(REGEX MATCH "[^/]+" remote_host ${tmp})

	# The location of the unpacked tarball:
	set(downdir     ${${PROJECT_NAME}_DATA_DOWNLOAD_DIR})
	set(stampdir    ${downdir}/.stamp)
	set(downfile    ${stampdir}/${hash}${extension})
	set(stampfile   ${stampdir}/${hash}.stamp)
	set(downscript  ${PROJECT_BINARY_DIR}/DownloadDataFiles.cmake)

	# Dump the script which will eventually download the data:
	file(WRITE ${downscript}
"# Make dir and download:
file(MAKE_DIRECTORY ${stampdir})
file(DOWNLOAD ${URL} ${downfile} EXPECTED_HASH ${HASHFULL} SHOW_PROGRESS)

# Unpack the file
message(STATUS \"Unpacking ${PROJECT_NAME} data tarball into ${downdir}\")
execute_process(
	COMMAND ${CMAKE_COMMAND} -E tar xzf ${downfile}
	COMMAND ${CMAKE_COMMAND} -E touch ${stampfile}
	WORKING_DIRECTORY ${downdir}
)"	)

	# Add custom command and target:
	add_custom_command(
		OUTPUT ${stampfile}
		COMMAND
		${CMAKE_COMMAND} -P ${downscript}
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
		COMMENT "Obtaining ${PROJECT_NAME} data from ${remote_host}"
	)
	add_custom_target(${TARGET} ALL DEPENDS ${stampfile})
endfunction(data_download_target)

function(install_data)
	# Install all data from the download dir to the data install dir
	#
	install(
		# Note the trailling /. in the next line
		# This makes sure that the content of the directory
		# (and not the dir itself) is copied to the destination.
		DIRECTORY ${${PROJECT_NAME}_DATA_DOWNLOAD_DIR}/.
		DESTINATION ${${PROJECT_NAME}_DATA_INSTALL_DIR}
		COMPONENT data
		REGEX "/\\.stamp/?" EXCLUDE
	)
endfunction()
