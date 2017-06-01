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

if (NOT ${PROJECT_NAME}_PROJECT_FEATURES_INIT)
set(${PROJECT_NAME}_ENABLED_FEATURES  "" CACHE INTERNAL
	"Enabled features of ${PROJECT_NAME}")
set(${PROJECT_NAME}_ALL_FEATURES "" CACHE INTERNAL "All features of ${PROJECT_NAME}")
set(${PROJECT_NAME}_PROJECT_FEATURES_INIT ON CACHE INTERNAL
	"ProjectFeatures is setup for ${PROJECT_NAME}")
endif()

function(enable_feature feature)
	# Enable a feature going by the keyword "feature"
	# sets
	#    PROJECT_HAVE_FEATURE to ON
	# and adds it to the feature lists as enabled
	#

	string(TOUPPER ${feature} UPPER_FEATURE)
	string(TOUPPER ${PROJECT_NAME} UPPER_PROJECT)

	set(${UPPER_PROJECT}_HAVE_${UPPER_FEATURE} ON CACHE INTERNAL "")

	list(FIND ${PROJECT_NAME}_ENABLED_FEATURES "${feature}" INDEX)
	if (INDEX EQUAL -1)
		list(APPEND ${PROJECT_NAME}_ENABLED_FEATURES ${feature})
		set(${PROJECT_NAME}_ENABLED_FEATURES
			"${${PROJECT_NAME}_ENABLED_FEATURES}"
			CACHE INTERNAL
			"Enabled features of ${PROJECT_NAME}"
		)
	endif()

	list(FIND ${PROJECT_NAME}_ALL_FEATURES "${feature}" INDEX)
	if (INDEX EQUAL -1)
		list(APPEND ${PROJECT_NAME}_ALL_FEATURES ${feature})
		set(${PROJECT_NAME}_ALL_FEATURES
			"${${PROJECT_NAME}_ALL_FEATURES}"
			CACHE INTERNAL
			"All features of ${PROJECT_NAME}"
		)
	endif()
endfunction()

function(disable_feature feature)
	# Disable a feature going by the keyword "feature"
	# sets
	#    PROJECT_HAVE_FEATURE to OFF
	# and adds it to the feature lists as enabled
	#
	string(TOUPPER ${feature} UPPER_FEATURE)
	string(TOUPPER ${PROJECT_NAME} UPPER_PROJECT)

	set(${UPPER_PROJECT}_HAVE_${UPPER_FEATURE} OFF CACHE INTERNAL "")
	list(FIND ${PROJECT_NAME}_ALL_FEATURES "${feature}" INDEX)
	if (INDEX EQUAL -1)
		list(APPEND ${PROJECT_NAME}_ALL_FEATURES ${feature})
		set(${PROJECT_NAME}_ALL_FEATURES
			"${${PROJECT_NAME}_ALL_FEATURES}"
			CACHE INTERNAL
			"All features of ${PROJECT_NAME}"
		)
	endif()
endfunction()

function(feature_list_cxx VARIABLE)
	# Parse the feature lists and return C++11 code
	# in the variable VARIABLE, which gives a list of all
	# enabled/disabled features by using the syntax
	#  {{"feature", true}, {"feature2", false}}

	set(${PROJECT_NAME}_ALL_FEATURES} "${${PROJECT_NAME}_ALL_FEATURES}")
	list(SORT ${PROJECT_NAME}_ALL_FEATURES)
	set(VAR "{")
	foreach(feature ${${PROJECT_NAME}_ALL_FEATURES})
		list(FIND ${PROJECT_NAME}_ENABLED_FEATURES "${feature}" INDEX)
		if(INDEX GREATER -1)
			# It exists!
			set(VAR "${VAR}{\"${feature}\", true}, ")
		else()
			set(VAR "${VAR}{\"${feature}\", false}, ")
		endif()
	endforeach()
	set(VAR "${VAR}}")
	set(${VARIABLE} "${VAR}" PARENT_SCOPE)
endfunction()
