#!/bin/bash

. /usr/pluto/install/Common.sh

if [ -z "$MIN_VER$PARAMETER" ]; then
	NAME="$PKG_NAME"
elif [ -z "$MIN_VER" ]; then
	NAME="${PKG_NAME}$PARAMETER"
else
	NAME="${PKG_NAME}_$MIN_VER$PARAMETER"
fi

wget --dns-timeout=180 -t 0 -c -P /usr/pluto/download "$REPOS_SRC/$REPOS/$NAME" || exit $ERR_DOWNLOAD
