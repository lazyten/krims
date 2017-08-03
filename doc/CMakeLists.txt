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

find_package(Doxygen)
find_package(LATEX)

if (NOT DOXYGEN_FOUND)
	message(FATAL_ERROR "Doxygen is needed to build the documentation.")
endif()

# Write actual doxygen file
configure_file("Doxyfile.in" "Doxyfile" @ONLY)

# Add command to build the documentation
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/codedoc/html/index.html
	       ${CMAKE_CURRENT_BINARY_DIR}/codedoc/latex/Makefile
	COMMAND ${DOXYGEN_EXECUTABLE} "Doxyfile"
	COMMENT "Creating source documentation with Doxygen"
	VERBATIM
)
set(DOCUMENTATION_FILES ${CMAKE_CURRENT_BINARY_DIR}/codedoc/html/index.html)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/codedoc/html
	DESTINATION share/doc/krims
	COMPONENT doc
)

# If latex was found also build the latex documentation
if (LATEX_FOUND)
	add_custom_command(
		OUTPUT   ${CMAKE_CURRENT_BINARY_DIR}/codedoc/latex/refman.pdf
		DEPENDS  ${CMAKE_CURRENT_BINARY_DIR}/codedoc/latex/Makefile
		COMMAND make
		COMMENT "Create pdf reference manual"
		VERBATIM
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/codedoc/latex"
	)
	set(DOCUMENTATION_FILES ${DOCUMENTATION_FILES} ${CMAKE_CURRENT_BINARY_DIR}/codedoc/latex/refman.pdf)

	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/codedoc/latex/refman.pdf
		DESTINATION share/doc/krims
		COMPONENT doc
	)
endif()

# Target to drive building the documentation
add_custom_target(doc-krims ALL DEPENDS ${DOCUMENTATION_FILES})
