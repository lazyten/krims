#!/bin/sh
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
