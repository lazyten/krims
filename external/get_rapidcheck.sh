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

if ! pwd | grep -q 'krims/external$'; then
	echo "Need to be in krims/external directory."
	exit 1
fi

FROM="https://github.com/emil-e/rapidcheck"
INTERVAL="1 hour" # of updating the repo

# ---------------------------

CHECKFILE=.last_pull
if [ -f rapidcheck/CMakeLists.txt ]; then
	TMP=`mktemp`
	touch --date="-$INTERVAL" $TMP || exit 1
	if [ ! -f "$CHECKFILE" -o "$CHECKFILE" -ot $TMP ]; then
		echo "-- Updating rapidcheck from git"
		(
			cd rapidcheck
			git pull
		) || exit 1
		touch $CHECKFILE
	fi
	rm $TMP
	exit 0
else
	echo "-- Cloning rapidcheck from git"
	git clone --recursive "$FROM" rapidcheck || exit 1
	touch $CHECKFILE
	exit 0
fi
exit 1
