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

include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)
include(CheckCXXSourceRuns)

macro(determine_supported_cxx_standards)
        # macro to check for the highest fully supported c++ standard.
        # Sets the cache variable DRB_HIGHEST_CXX_SUPPORT to the plain
        # plain values (98,11,14,17).
        #
        # Note that the macro does not enforce any particular standard.
        #
        set(DRB_HIGHEST_CXX_SUPPORT 98 CACHE INTERNAL "The highest c++ standard supported.")
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
                # All of gcc supports 98
                set(DRB_HIGHEST_CXX_SUPPORT 98)

                if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.8.1")
                        set(DRB_HIGHEST_CXX_SUPPORT 11)
                endif()

                if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0")
                        set(DRB_HIGHEST_CXX_SUPPORT 14)
                endif()

                if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7")
                        set(DRB_HIGHEST_CXX_SUPPORT 17)
                endif()

                # TODO Extend once c++2a is ready
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
                # All of clang supports 98
                set(DRB_HIGHEST_CXX_SUPPORT 98)

                if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.3")
                        set(DRB_HIGHEST_CXX_SUPPORT 11)
                endif()

                if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.4")
                        set(DRB_HIGHEST_CXX_SUPPORT 14)
                endif()

                if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5")
                        set(DRB_HIGHEST_CXX_SUPPORT 17)
                endif()

                # TODO Extend once c++2a is ready
        else()
                message(WARNING "Determining maximum supported C++ version is not supported for compiler ${CMAKE_CXX_COMPILER_ID} yet. \
                Going with C++98.")
        endif()
endmacro(determine_supported_cxx_standards)

function(determine_supported_stdlib_cxx)
        # macro to check for the ideal standard library to use.
        #
        # For clang this would be libc++ if it is available, for 
        # gcc it would be libstdc++.
        #
        # The result is stored in cache under DRB_CXX_STANDARD_LIBRARY
        #
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
                set(CXX_STANDARD_LIBRARY "libstdc++")
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
                # Try to see whether clang and the system support llvm's libc++ library
                # as the standard c++ library
                set(CMAKE_REQUIRED_FLAGS_ORIG "${CMAKE_REQUIRED_FLAGS}")
                set(CMAKE_REQUIRED_FLAGS "-stdlib=libc++ -Werror -std=c++11")
                CHECK_CXX_SOURCE_RUNS("#include <vector>
                int main() { std::vector<int> v{0,}; return v[0]; }" DRB_HAVE_LLVM_LIBCXX)
                set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_ORIG}")
                unset(CMAKE_REQUIRED_FLAGS_ORIG)

                # Set the library to link with accordingly in the cache:
                if ("${CXX_STANDARD_LIBRARY}" STREQUAL "")
                        if(DRB_HAVE_LLVM_LIBCXX)
                                set(CXX_STANDARD_LIBRARY "libc++")
                        else()
                                set(CXX_STANDARD_LIBRARY "libstdc++")
                        endif()
                endif()
        else()
                message(WARNING "Compiler ${CMAKE_CXX_COMPILER_ID} not supported in determine_supported_stdlib_cxx")
        endif()

        set(DRB_CXX_STANDARD_LIBRARY "${CXX_STANDARD_LIBRARY}" CACHE STRING
                "Choose the standard library to link with.")
        message(STATUS "Using C++ standard library:  ${DRB_CXX_STANDARD_LIBRARY}")

        unset(CXX_STANDARD_LIBRARY)
endfunction()

function(cxx_standard_flag STANDARD VARIABLE)
        # Function which determines the compiler flag needed for the requested
        # standard STANDARD on the current compiler.
        #
        # The flag is written to the output variable ${VARIABLE}
        #
        if (STANDARD VERSION_LESS "11" OR "${STANDARD}" STREQUAL "98")
                message(FATAL_ERROR "C++ Standard below 11 not supported in cxx_standard_flag")
        endif()

        set (FLAG "c++${STANDARD}")
        if (STANDARD EQUAL "14")
                if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.5")
                        # Clang <3.5 needs special treatment here,
                        # since c++14 flag is not yet know there
                        set(FLAG "c++1y")
                endif()
        endif()
        if (STANDARD EQUAL "17")
                if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
                        # Gnu compilers currently need special treatment
                        # since c++17 flag is not yet know there
                        set(FLAG "c++1z")
                endif()
        endif()

        # Check whether the flag works
        CHECK_CXX_COMPILER_FLAG("-std=${FLAG}" DRB_HAVE_STANDARD_FLAG)
        if (NOT DRB_HAVE_STANDARD_FLAG)
                message(FATAL_ERROR "C++ Standard ${STANDARD} is requested but is not supported.")
        endif()
        unset(DRB_HAVE_STANDARD_FLAG)

        set(${VARIABLE} "-std=${FLAG}" PARENT_SCOPE)
endfunction(cxx_standard_flag)

function(stdlib_cxx_flags STDLIB COMP_FLAGS LINK_FLAGS)
        # Function which determines the compiler flags and linker flags
        # needed in order to use the requested standard library for the
        # current compiler.
        #
        # The flags are added to the output variables ${COMP_FLAGS}
        # and ${LINK_FLAGS} respectively
        #
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
                if (NOT "${STDLIB}" STREQUAL "libstdc++")
                        message(FATAL_ERROR "For gcc only \"libstdc++\" \
                        is supported as the standard library")
                endif()

                # Set no flags, since libstdc++ is the default on gcc
                set(${VARIABLE} "" PARENT_SCOPE)
                return()
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
                # And also in the compiler/linker flags:
                if (STDLIB STREQUAL "libc++" OR STDLIB STREQUAL "libstdc++")
                        set(${COMP_FLAGS} "${${COMP_FLAGS}} -stdlib=${STDLIB}" PARENT_SCOPE)
                        set(${LINK_FLAGS} "${${LINK_FLAGS}} -stdlib=${STDLIB}" PARENT_SCOPE)
                else()
                        message(FATAL_ERROR "Unrecognised value \"${STDLIB}\" \
                        for CPP_STANDARD_LIBRARY. Only \"libc++\" or \"libstdc++\" are valid for clang")
                endif()
        else()
                message(WARNING "Compiler ${CMAKE_CXX_COMPILER_ID} not supported in stdlib_cxx_flags")
        endif()
endfunction()

macro(use_cxx_standard STANDARD)
        # macro to enforce a particular c++ standard (only needed for cmake < 3.1)
        #
        # sets the variable
        #	CMAKE_CXX_STANDARD
        # to the plain value STANDARD (98,11,14 or 17)
        #
        if (CMAKE_VERSION VERSION_LESS "3.1")
                # Determine the flag for the requested c++ standard.
                cxx_standard_flag("${STANDARD}" FLAG)
                set (CMAKE_CXX_FLAGS "${FLAG} ${CMAKE_CXX_FLAGS}")
                unset(FLAG)

                # set the CMAKE_CXX_STANDARD variable
                # (even though unused in this CMake version)
                set(CMAKE_CXX_STANDARD ${STANDARD}
                        CACHE STRING "The C++ standard we use for this build.")
        else()
                # set the standard and enforce it:
                set(CMAKE_CXX_STANDARD ${STANDARD})
                set(CMAKE_CXX_STANDARD_REQUIRED ON)

                # make sure we no not use the gnu++ extensions:
                # this adds c++n instead of gnu++n to the command line
                set(CMAKE_CXX_EXTENSIONS OFF)
        endif()
endmacro(use_cxx_standard)

macro(enable_if_compiles VARIABLE FLAG)
        # Checks whether a cxx compiler supports a flag and if yes
        # adds it to the variable provided.
        #
        string(REGEX REPLACE "[^a-zA-Z0-9]" "" FLAG_CLEAN "${FLAG}")
        CHECK_CXX_COMPILER_FLAG("-Werror ${FLAG}" DRB_HAVE_FLAG_${FLAG_CLEAN})
        if (DRB_HAVE_FLAG_${FLAG_CLEAN})
                set(${VARIABLE} "${${VARIABLE}} ${FLAG}")
        endif()
        unset(FLAG_CLEAN)
endmacro(enable_if_compiles)

macro(enable_if_cc_compiles VARIABLE FLAG)
        # Checks whether a plain c compiler supports a flag and if yes
        # adds it to the variable provided.
        #
        string(REGEX REPLACE "[^a-zA-Z0-9]" "" FLAG_CLEAN "${FLAG}")
        CHECK_C_COMPILER_FLAG("-Werror ${FLAG}" DRB_CC_HAVE_FLAG_${FLAG_CLEAN})
        if (DRB_CC_HAVE_FLAG_${FLAG_CLEAN})
                set(${VARIABLE} "${${VARIABLE}} ${FLAG}")
        endif()
        unset(FLAG_CLEAN)
endmacro(enable_if_cc_compiles)

macro(enable_if_all_compiles VARIABLE1 VARIABLE2 FLAG)
        # Checks whether the plain c compiler as well as the C++ compiler
        # supports a flag and if yes adds it to the variables provided.
        #
        string(REGEX REPLACE "[^a-zA-Z0-9]" "" FLAG_CLEAN "${FLAG}")
        CHECK_C_COMPILER_FLAG("-Werror ${FLAG}" DRB_CC_HAVE_FLAG_${FLAG_CLEAN})
        CHECK_CXX_COMPILER_FLAG("-Werror ${FLAG}" DRB_HAVE_FLAG_${FLAG_CLEAN})
        if (DRB_CC_HAVE_FLAG_${FLAG_CLEAN} AND DRB_HAVE_FLAG_${FLAG_CLEAN})
                set(${VARIABLE1} "${${VARIABLE1}} ${FLAG}")
                set(${VARIABLE2} "${${VARIABLE2}} ${FLAG}")
        endif()
        unset(FLAG_CLEAN)
endmacro()

# TODO have something similar for the linker

macro(DRB_SETUP_COMPILER_FLAGS CXX_STANDARD)
        # Determine the compiler type and load corresponding flags.
        # Enforce a c++ standard above CXX_STANDARD.
        #
        # The macro takes DRB_MAXIMUM_CXX_STANDARD into account and does not go
        # beyond this c++ standard even though the compiler supports it.
        # If DRB_MAXIMUM_CXX_STANDARD < CXX_STANDARD an error is thrown.
        # This option is hence useful to define an upper CXX standard for testing
        # code with a new compiler but an old standard even though a higher
        # standard is supported.
        #
        # sets the following variables
        #	CMAKE_CXX_STANDARD			The precise c++ standard enforced.
        #       CMAKE_CXX_FLAGS				Flags for the c++ compiler, all builds
        #	CMAKE_CXX_FLAGS_DEBUG			Extra flags for the debug build
        #	CMAKE_CXX_FLAGS_RELEASE			Extra flags for the release build
        #
        #	CMAKE_SHARED_LINKER_FLAGS		Flags for the shared linker, all builds
        #	CMAKE_SHARED_LINKER_FLAGS_DEBUG		Extra flags for the debug build
        #	CMAKE_SHARED_LINKER_FLAGS_RELEASE	Extra flags for the release build
        #
        #	CMAKE_STATIC_LINKER_FLAGS		Flags for the static linker, all builds
        #	CMAKE_STATIC_LINKER_FLAGS_DEBUG		Extra flags for the debug build
        #	CMAKE_STATIC_LINKER_FLAGS_RELEASE	Extra flags for the release build
        #
        #	CMAKE_EXE_LINKER_FLAGS			Flags for linking executables, all builds
        #	CMAKE_EXE_LINKER_FLAGS_DEBUG		Extra flags for the debug build
        #	CMAKE_EXE_LINKER_FLAGS_RELEASE		Extra flags for the release build

        # check that drb has been initialised
        if(NOT DRB_INITIALISED)
                message(FATAL_ERROR "You have to call drb_init before any other DebugReleaseBuild function.")
        endif()

        # check what standards are supported
        determine_supported_cxx_standards()

        # If DRB_MAXIMUM_CXX_STANDARD is present and not set to "auto",
        # enforce a lower standard than supported:
        if(DEFINED DRB_MAXIMUM_CXX_STANDARD
                        AND NOT "${DRB_MAXIMUM_CXX_STANDARD}" STREQUAL "highest" )

                if (DRB_MAXIMUM_CXX_STANDARD VERSION_LESS CXX_STANDARD)
                        message(FATAL_ERROR "The maximal C++ standard \
                        DRB_MAXIMUM_CXX_STANDARD(==${DRB_MAXIMUM_CXX_STANDARD}) is below \
                        CXX_STANDARD(==${CXX_STANDARD}).")
                endif()
                if(DRB_MAXIMUM_CXX_STANDARD VERSION_LESS DRB_HIGHEST_CXX_SUPPORT)
                        set(DRB_HIGHEST_CXX_SUPPORT ${DRB_MAXIMUM_CXX_STANDARD})
                endif()
        endif()

        # Promote DRB_MAXIMUM_CXX_STANDARD to cache
        set(DRB_MAXIMUM_CXX_STANDARD "${DRB_HIGHEST_CXX_SUPPORT}" CACHE
                STRING "The maximal C++ standard DebugReleaseBuild makes use of. \
                Set to \"highest\" to let DRB use the highest available C++ standard (default).")

                # enforce the highest standard we are ok with:
                if (DRB_HIGHEST_CXX_SUPPORT VERSION_GREATER CXX_STANDARD)
                        use_cxx_standard(${DRB_HIGHEST_CXX_SUPPORT})
                else()
                        use_cxx_standard(${CXX_STANDARD})
                endif()

                # Check what standard library should be used
                determine_supported_stdlib_cxx()
                stdlib_cxx_flags(${DRB_CXX_STANDARD_LIBRARY} CMAKE_CXX_FLAGS CMAKE_EXE_LINKER_FLAGS)

                if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
                        # We have a known "standard" compiler
                        include("${DRB_DIR}/compiler_flags/standard.cmake")
                else()
                        message(WARNING "Untested compiler: ${CMAKE_CXX_COMPILER_ID}, you are on your own.")
                        message(WARNING "Currently we only support clang and gnu compilers.")

                        # Try to fall back to the standard
                        include("${DRB_DIR}/compiler_flags/standard.cmake")
                endif()

                # Setup compiler flags for special compile options (if desired and available)
                include("${DRB_DIR}/compiler_flags/coverage.cmake")
                include("${DRB_DIR}/compiler_flags/sanitise.cmake")
        endmacro(DRB_SETUP_COMPILER_FLAGS)
