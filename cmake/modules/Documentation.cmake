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

option(DOCUMENTATION_DOXYGEN_LATEX "Build the latex doxygen documentation" OFF)

function(setup_doxygen DOXYTARGET DOXYFILE DOXYDIR INSTALLDIR)
	# Run doxygen if the target ${DOXYTARGET} is built.
	# ${DOXYFILE} is the doxygen config to use and ${DOXYDIR}
	# is the directory where the doxygen documentation will end up according
	# to ${DOXYFILE}.
	#
	# The resulting pdf and html documentation will be installed to ${INSTALLDIR}
	# once the doc component is installed.

	find_package(Doxygen)
	if (NOT DOXYGEN_FOUND)
		message(FATAL_ERROR "Doxygen is needed for the setup_doxygen function of the Documentation package")
	endif()

	if(DOCUMENTATION_DOXYGEN_LATEX)
		set(LATEXFILE ${DOXYDIR}/latex/Makefile)
	endif()

	# Always run doxygen
	add_custom_command(
		DEPENDS "${DOXYFILE}"
		OUTPUT dummy ${DOXYDIR}/html/index.html ${LATEXFILE}
		COMMAND ${DOXYGEN_EXECUTABLE} "${DOXYFILE}"
		COMMENT "Creating ${PROJECT_NAME} source documentation with Doxygen"
		VERBATIM
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	)
	install(DIRECTORY ${DOXYDIR}/html
		DESTINATION ${INSTALLDIR}
		COMPONENT doc
	)
	set(DOXY_FILES ${DOXYDIR}/html/index.html ${DOXYDIR}/latex/Makefile)

	# Run Latex if we have it.
	find_package(LATEX COMPONENTS PDFLATEX)
	if (LATEX_FOUND AND DOCUMENTATION_DOXYGEN_LATEX)
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

function(setup_latex LATEXTARGET MAINFILE INSTALLDIR FEATURES)
	# Run latex on the mainfile if the target ${LATEXTARGET} is built.
	# ${MAINFILE} is the main tex file with the \begin{document}.
	#
	# The resulting pdf will be installed to ${INSTALLDIR}
	# once the doc component is installed.
	#
	# FEATURES is a list which indicates which latex features are needed.
	# Currently only "bibtex" and "" (i.e. plain latex) are understood.
	find_package(LATEX COMPONENTS PDFLATEX BIBTEX)
	if (NOT LATEX_FOUND)
		message(FATAL_ERROR "Latex is needed for the setup_latex call of the Documentation package")
	endif()

	get_filename_component(TEXNAME ${MAINFILE} NAME_WE)
	if (FEATURES EQUAL "bibtex")
		add_custom_command(
			DEPENDS ${TEXNAME}.tex
			OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${TEXNAME}.aux
			COMMAND ${PDFLATEX_COMPILER} -halt-on-error -output-directory ${CMAKE_CURRENT_BINARY_DIR} ${TEXNAME}.tex
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			VERBATIM
		)

		add_custom_command(
			DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${TEXNAME}.aux
			OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${TEXNAME}.bbl
			COMMAND ${BIBTEX_COMPILER} ${TEXNAME}.aux
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			VERBATIM
		)
		set(extradepends ${CMAKE_CURRENT_BINARY_DIR}/${TEXNAME}.bbl)
	endif()

	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${TEXNAME}.pdf
		DEPENDS ${TEXNAME}.tex ${extradepends}
		COMMAND ${PDFLATEX_COMPILER} -halt-on-error -output-directory ${CMAKE_CURRENT_BINARY_DIR} ${TEXNAME}.tex
		COMMAND ${PDFLATEX_COMPILER} -halt-on-error -output-directory ${CMAKE_CURRENT_BINARY_DIR} ${TEXNAME}.tex
		VERBATIM
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)

	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TEXNAME}.pdf DESTINATION ${INSTALLDIR} COMPONENT doc)
	add_custom_target(${LATEXTARGET} DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${TEXNAME}.pdf)
endfunction()

