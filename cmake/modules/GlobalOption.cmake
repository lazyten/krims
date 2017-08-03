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

function(global_option NAME DOCSTRING VALUE)
	# Add a global option which goes by the name NAME
	# and add a project-local version which has the prefix ${PROJECT_NAME}
	#
	# Make sure that the local version is only ever stored if it is
	# different from the global one.
	string(TOUPPER "${PROJECT_NAME}" PROJECT_UPPER)

	option(${NAME} "${DOCSTRING} (default for all projects)" ${VALUE})

	if(DEFINED ${PROJECT_UPPER}_${NAME} AND NOT ${${PROJECT_UPPER}_${NAME}} EQUAL ${${NAME}})
		# local version defined and different
		option(${PROJECT_UPPER}_${NAME} "${DOCSTRING} for ${PROJECT_NAME}" ${${PROJECT_UPPER}_${NAME}})
	else()
		set(${PROJECT_UPPER}_${NAME} ${${NAME}} PARENT_SCOPE)
	endif()
endfunction()
