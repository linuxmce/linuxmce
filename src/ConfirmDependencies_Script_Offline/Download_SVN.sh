#!/bin/bash

. /usr/pluto/install/Common.sh

[ -z "$SRC_IMPL" ] && SRC_IMPL="/usr/pluto/sources/$PKG_NAME"
mkdir -p "$SRC_IMPL" || exit $ERR_DOWNLOAD
mkdir -p "/usr/pluto/sources/src/lib" "/usr/pluto/sources/src/bin"

#echo "$REPOS_SRC" | egrep '^svn(\+ssh)?://' &>/dev/null || REPOS_SRC="svn://$REPOS_SRC"
svn_param="--non-interactive --no-auth-cache"
[ -n "$USERNAME" ] && svn_param="$svn_param --username $USERNAME"
[ -n "$PASSWORD" ] && svn_param="$svn_param --password $PASSWORD"
if ! svn checkout $svn_param "$REPOS_SRC/$REPOS/$SRC_IMPL" "/usr/pluto/sources/$SRC_IMPL"; then
	echo "SVN source download failed: $REPOS_SRC/$REPOS/$SRC_IMPL"
	exit $ERR_DOWNLOAD
fi

mkdir -p /usr/pluto/install

#File=/usr/pluto/install/compile.sh
File=/dev/null

echo "
pushd \"/usr/pluto/src/$SRC_IMPL\" &>/dev/null
if ! make; then
	echo \"Source make failed\"
	exit $ERR_MAKE
fi
popd &>/dev/null
" >>"$File"
exit 0
