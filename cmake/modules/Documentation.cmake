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
	message(FATAL_ERROR "Doxygen is needed for the Documentation krims package")
endif()

function(setup_doxygen DOXYTARGET DOXYFILE DOXYDIR INSTALLDIR)
	# Run doxygen if the target ${DOXYTARGET} is built.
	# ${DOXYFILE} is the doxygen config to use and ${DOXYDIR}
	# is the directory where the doxygen documentation will end up according
	# to ${DOXYFILE}.
	#
	# The resulting pdf and html documentation will be installed to ${INSTALLDIR}
	# once the doc component is installed.

	# Always run doxygen
	add_custom_command(
		OUTPUT dummy ${DOXYDIR}/html/index.html ${DOXYDIR}/latex/Makefile
		COMMAND ${DOXYGEN_EXECUTABLE} "Doxyfile"
		COMMENT "Creating ${PROJECT_NAME} source documentation with Doxygen"
		VERBATIM
	)
	install(DIRECTORY ${DOXYDIR}/html
		DESTINATION ${INSTALLDIR}
		COMPONENT doc
	)
	set(DOXY_FILES ${DOXYDIR}/html/index.html ${DOXYDIR}/latex/Makefile)

	# Run Latex if we have it.
	if (LATEX_FOUND)
		add_custom_command(
			OUTPUT   ${DOXYDIR}/latex/refman.pdf
			DEPENDS  ${DOXYDIR}/latex/Makefile
			COMMAND make
			COMMENT "Create ${PROJECT_NAME} pdf reference manual"
			VERBATIM
			WORKING_DIRECTORY "${DOXYDIR}/latex"
		)
		install(FILES ${DOXYDIR}/latex/refman.pdf
			DESTINATION ${INSTALLDIR}
			COMPONENT doc
		)
		set(DOXY_FILES ${DOXY_FILES} ${DOXYDIR}/latex/refman.pdf)
	endif()

	add_custom_target(${DOXYTARGET} ALL DEPENDS ${DOXY_FILES})
endfunction()

