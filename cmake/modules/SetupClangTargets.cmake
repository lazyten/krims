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

# This module adds some targets which invoke clang checking tools with
# the appropriate files and options for this project.
#
# Available are things like formatting or checks for coding conventions
# and/or tiny bugs.

include(CMakeParseArguments)

#
# Find executables of clang tools
#
if (CMAKE_CXX_COMPILER_VERSION) 
	# Split version into list
	string(REPLACE "." ";" SPLIT_VERSION ${CMAKE_CXX_COMPILER_VERSION})

	# Get major and minor into ALTERNATE_SUFFIX
	list(GET SPLIT_VERSION 0 PART)
	set(ALTERNATE_SUFFIX "-${PART}")
	list(GET SPLIT_VERSION 1 PART)
	set(ALTERNATE_SUFFIX "${ALTERNATE_SUFFIX}.${PART}")

	unset(SPLIT_VERSION)
	unset(PART)
endif()

find_program(CLANG_TIDY
	NAMES clang-tidy clang-tidy${ALTERNATE_SUFFIX}
	DOC "Full path to clang-tidy"
)
find_program(CLANG_APPLY_REPLACEMENTS
	NAMES clang-apply-replacements clang-apply-replacements${ALTERNATE_SUFFIX}
	DOC "Full path to clang-apply-replacements"
)
find_program(CLANG_FORMAT
	NAMES clang-format clang-format${ALTERNATE_SUFFIX}
	DOC "Full path to clang-format"
)
unset(ALTERNATE_SUFFIX)

#
# ------------------------------------------------------------------
# Internal macros
#

macro(SCT_get_header_globs store dir)
	# Get the header file globs for globbing inside
	# the directory dir
	#
	# Note that this is an internal macro.
	unset(${store})
	set(HEADER_EXTENSIONS h hh hpp H h++)
	foreach(glob ${HEADER_EXTENSIONS})
		set(${store} ${${store}} "${dir}/*.${glob}")
	endforeach()
endmacro()

macro(SCT_get_source_globs store dir)
	# Get the source file globs for globbing inside
	# the directory dir
	#
	# Note that this is an internal macro.
	unset(${store})
	foreach(glob ${CMAKE_CXX_SOURCE_FILE_EXTENSIONS} ${CMAKE_C_SOURCE_FILE_EXTENSIONS})
		set(${store} ${${store}} "${dir}/*.${glob}")
	endforeach()
endmacro()

function(SCT_get_targets_recursive store dir)
	# Get all targets which are defined in this directory
	# or one of its subdirectories in depth-first traversal.

	get_property(SUBDIR DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
	foreach(subdir ${SUBDIR})
		SCT_get_targets_recursive(${store} ${subdir})
	endforeach()

	get_property(TARGETS DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
	set(${store} ${${store}} ${TARGETS} PARENT_SCOPE)
endfunction()

function(SCT_parse_add_clang_target_args)
	# Parse the arguments passed to a SetupClang function
	# and check basic sanity. Then create a list of 
	# header files and source files in the passed directories.
	#
	# After the macro execution these variables are set:
	#
	# HEADER_FILES:   List of all header files in the dirs
	# SOURCE_FILES:   List of all source files in the dirs
	#                 (takes the current language into account)
	# DIR_TARGETS:    List of all targets in the dirs
	#                 (only filled for cmake 3.7 and above, else empty)
	#
	# Note that this is an internal function.

	#
	# Parse arguments
	#
	set(options )
	set(oneValueArgs)
	set(multiValueArgs DIRECTORIES)
	cmake_parse_arguments(SCT "${options}" "${oneValueArgs}" "${multiValueArgs}"  ${ARGN})

	if(SCT_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unknown keyword: \"${SCT_UNPARSED_ARGUMENTS}\"")
	endif()

	#
	# Check sanity and work on args
	#
	if("${SCT_DIRECTORIES}" STREQUAL "")
		message(FATAL_ERROR "You need to specify the DIRECTORIES parameter with a list of subdirectories \
to process for source files and headers.")
	endif()

	#
	# Extract headers and sources
	#
	set(HEADER_FILES "")
	foreach(dir ${SCT_DIRECTORIES})
		SCT_get_header_globs(HEADER_GLOB ${dir})
		file(GLOB_RECURSE TMP ${HEADER_GLOB})
		set(HEADER_FILES ${HEADER_FILES} ${TMP})
	endforeach()
	list(REMOVE_DUPLICATES HEADER_FILES)

	set(SOURCE_FILES "")
	if (CMAKE_VERSION VERSION_LESS 3.7.0)
		# Older and less reliable method as we might get sources
		# which are not actually compiled as well!
		foreach(dir ${SCT_DIRECTORIES})
			SCT_get_source_globs(SOURCE_GLOB ${dir})
			file(GLOB_RECURSE TMP ${SOURCE_GLOB})
			set(SOURCE_FILES ${SOURCE_FILES} ${TMP})
		endforeach()
	else()
		# Better version for cmake >= 3.7.0 -> Works by first getting
		# a list of targets and then their source files explicitly.
		set(DIR_TARGETS "")
		foreach(dir ${SCT_DIRECTORIES})
			SCT_get_targets_recursive(TAR ${dir})
			set(DIR_TARGETS ${TARGETS} ${TAR})
		endforeach()

		foreach (target ${DIR_TARGETS})
			get_target_property(T_SOURCES ${target} SOURCES)
			get_target_property(T_DIR ${target} SOURCE_DIR)
			foreach(src ${T_SOURCES})
				set(SOURCE_FILES ${SOURCE_FILES} "${T_DIR}/${src}")
			endforeach()
		endforeach()
	endif()
	list(REMOVE_DUPLICATES SOURCE_FILES)

	# Export findings
	set(SOURCE_FILES "${SOURCE_FILES}" PARENT_SCOPE)
	set(HEADER_FILES "${HEADER_FILES}" PARENT_SCOPE)
	set(DIR_TARGETS "${DIR_TARGETS}" PARENT_SCOPE)
endfunction(SCT_parse_add_clang_target_args)

#
# ------------------------------------------------------------------
# External macros
#

function(add_clang_format_target TARGET_NAME)
	# Defines the target clang-format-${TARGET_NAME} which
	# formats all header and source files in a list of
	# directories using clang-format.
	#
	# Full syntax is
	#  add_clang_format_target TARGET_NAME DIRECTORIES dir1 [dir2 ... ]
	#
	# TARGET_NAME
	#    Name of the target, i.e. what will be used after
	#    clang-format-...
	#
	# DIRECTORIES
	#    List of directories to search for header and source files
	#    These should have been parsed by cmake already, so it is
	#    a good idea to run this command at the end of a CMakeLists.txt

	# Fills HEADER_FILES and SOURCE_FILES
	SCT_parse_add_clang_target_args(${ARGN})

	# More checks
	if (CLANG_FORMAT MATCHES "NOTFOUND")
		message(FATAL_ERROR "clang-format executable not found, so cannot setup clang format target.")
	endif()

	add_custom_target(clang-format-${TARGET_NAME}
		${CLANG_FORMAT} -i ${SOURCE_FILES} ${HEADER_FILES}
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		COMMENT "Format all code (headers and sources) files associated with ${TARGET_NAME}."
		VERBATIM
	)

	message(STATUS "Successfully set up target \"clang-format-${TARGET_NAME}\".")
endfunction()


function(add_clang_tidy_target TARGET_NAME)
	# Defines the targets clang-tidy-check-${TARGET_NAME} and
	# clang-tidy-fix-${TARGET_NAME}.
	#
	# clang-tidy-check-${TARGET_NAME} checks all source files whether
	# they agree with the coding conventions in this source tree.
	#
	# clang-tidy-fix-${TARGET_NAME} does the same but also applies
	# the fixes which can be applied automatically.
	#
	# Full syntax of this command is
	#   add_clang_tidy_target TARGET_NAME DIRECTORIES dir1 [dir2 ... ]
	#
	# TARGET_NAME
	#    Name of the target, i.e. what will be used after
	#    clang-format-...
	#
	# DIRECTORIES
	#    List of directories to search for source files.
	#    These should have been parsed by cmake already, so it is
	#    a good idea to run this command at the end of a CMakeLists.txt

	# Fills HEADER_FILES and SOURCE_FILES
	SCT_parse_add_clang_target_args(${ARGN})

	#
	# Checks
	#
	if (CMAKE_VERSION VERSION_LESS 3.5.0)
		message(FATAL_ERROR "Clang-tidy targets are not available if cmake version is below 3.5.0.")
	endif()
	cmake_minimum_required(VERSION 3.5.0)

	if (NOT CMAKE_EXPORT_COMPILE_COMMANDS)
		message(FATAL_ERROR "Clang-tidy is not available if \"CMAKE_EXPORT_COMPILE_COMMANDS\" \
is set to \"OFF\", since then no compile_commands.json file is produced.")
	endif()

	if (CLANG_TIDY MATCHES "NOTFOUND" OR CLANG_APPLY_REPLACEMENTS MATCHES "NOTFOUND")
		message(FATAL_ERROR "clang-tidy and/or clang-apply-replacements executable not found, \
so cannot setup clang tidy targets.")
	endif()

	set(FIXDIR ${PROJECT_BINARY_DIR}/fixes)
	set(FIXFILE ${FIXDIR}/clang-tidy-${TARGET_NAME}.yaml)
	set(APPLYFILE ${FIXDIR}/last-fixes-apply)

	# Command to determine what fixes should be done
	add_custom_command(OUTPUT ${FIXFILE}
		COMMAND
		mkdir -p "${FIXDIR}"
		COMMAND
		rm -f ${FIXFILE}
		COMMAND
		${CLANG_TIDY} -p "${PROJECT_BINARY_DIR}" -export-fixes=${FIXFILE} ${SOURCE_FILES}
		COMMAND
		touch ${FIXFILE}
		##
		DEPENDS ${SOURCE_FILES} ${PROJECT_SOURCE_DIR}/.clang-tidy
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		COMMENT "Detect problems with source files of ${TARGET_NAME} using clang-tidy."
		VERBATIM
	)

	# Command to apply the fixes
	add_custom_command(OUTPUT ${APPLYFILE}
		COMMAND
		${CLANG_APPLY_REPLACEMENTS} -format -style=file -style-config="${PROJECT_SOURCE_DIR}" "${FIXDIR}"
		COMMAND
		touch ${APPLYFILE}
		##
		DEPENDS ${FIXFILE}
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		COMMENT "Fix problems detected for ${TARGET_NAME}'s sources."
		VERBATIM
	)

	# Target to detect the fixes
	add_custom_target(clang-tidy-${TARGET_NAME}
		DEPENDS ${FIXFILE}
		##
		# Test whether the fixes file is empty or not.
		# If it is non-empty we return with a non-zero exit code
		COMMAND test ! -s ${FIXFILE}
		VERBATIM
	)

	# Target to apply the fixes
	add_custom_target(clang-tidy-${TARGET_NAME}-fix
		DEPENDS ${APPLYFILE}
	)

	message(STATUS "Successfully set up targets \"clang-tidy-${TARGET_NAME}\" and \"clang-tidy-${TARGET_NAME}-fix\".")
endfunction()

function(add_available_clang_targets_for)
	# Check the precondition of each of the clang tools
	# and if they are satisfied pass on all arguments
	# to the appropriate setup function.
	if (NOT CLANG_FORMAT MATCHES "NOTFOUND")
		add_clang_format_target(${ARGV})
	endif()

	if (NOT CMAKE_VERSION VERSION_LESS 3.5.0
			AND NOT CLANG_TIDY MATCHES "NOTFOUND"
			AND NOT CLANG_APPLY_REPLACEMENTS MATCHES "NOTFOUND")

		# Since exporting of compile commands is required,
		# set it to on.
		set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
		add_clang_tidy_target(${ARGV})
	endif()
endfunction(add_available_clang_targets_for)
