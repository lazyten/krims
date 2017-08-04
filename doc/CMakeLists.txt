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

include(Documentation)

# Exclude patterns for the actual Doxyfile generated with configure_file
set(DOXY_EXCLUDE_PATTERNS "")
configure_file("Doxyfile.in" "Doxyfile" @ONLY)

setup_doxygen(doc-krims-doxygen "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
	      "${CMAKE_CURRENT_BINARY_DIR}/codedoc"
	      "share/doc/krims")

add_custom_target(doc-krims ALL)
add_dependencies(doc-krims doc-krims-doxygen)
