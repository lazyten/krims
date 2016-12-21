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

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# Cannot produce coverage analysis for any compiler but clang or GNU
	return()
endif()

# TODO make this compatible with coveralls
#      See https://github.com/JoakimSoderberg/coveralls-cmake for details

option(DRB_COVERAGE_Debug "Compile with coverage testing in the Debug build." OFF)

if(DRB_COVERAGE_Debug AND CMAKE_BUILD_TYPE MATCHES "Debug")
	set(_FLAGSS "--coverage")
	enable_if_compiles(_FLAGSS "--coverage")

	# TODO This does only enable so-called DebugInfo-based coverage in clang.
	#      But clang has more to offer. See
	#      http://clang.llvm.org/docs/SourceBasedCodeCoverage.html
	#      http://clang.llvm.org/docs/SanitizerCoverage.html

	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${_FLAGSS}")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${_FLAGSS}")
	unset(_FLAGSS)

	message(STATUS "Enabled coverage analysis in the Debug build.")
endif()

# TODO
# Have a custom target to:
#    - Run the selected tests and generate the profile data
#    - Combine the profile data
#    - Produce some nice html or other output
