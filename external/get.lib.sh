#!/bin/sh
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

if [ -z "$FROM" ]; then
	echo "Please define FROM to a non-zero value"
	exit 1
fi
if [ -z "$INTERVAL" ]; then
	echo "Please define INTERVAL to a non-zero value"
	exit 1
fi
if [ -z "$CHECKFILE" ]; then
	echo "Please define CHECKFILE to a non-zero value"
	exit 1
fi
if [ -z "$WHAT" ]; then
	echo "Please define WHAT to a non-zero value"
	exit 1
fi

mkdir -p .last_pull
TIMEFILE=".last_pull/$WHAT"
if [ -f "$WHAT/$CHECKFILE" ]; then
	TMP=`mktemp`
	touch --date="-$INTERVAL" $TMP || exit 1
	if [ ! -f "$TIMEFILE" -o "$TIMEFILE" -ot $TMP ]; then
		echo "-- Updating  $WHAT  from git"
		(
			cd "$WHAT"
			git pull && \
			git submodule update --init --recursive
		) || exit 1
		touch "$TIMEFILE"
	fi
	rm $TMP
	exit 0
else
	echo "-- Cloning  $WHAT  from git"
	git clone --recursive "$FROM" "$WHAT" || exit 1
	touch "$TIMEFILE"
	exit 0
fi
exit 1
