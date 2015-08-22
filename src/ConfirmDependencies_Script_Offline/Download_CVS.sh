#!/bin/bash

. /usr/pluto/install/Common.sh

[ -z "$SRC_IMPL" ] && SRC_IMPL="/usr/pluto/src/$PKG_NAME"
mkdir -p "$SRC_IMPL" || exit $ERR_DOWNLOAD

# TODO: CVS pserver login
if ! cvs -d "$REPOS_SRC" checkout -d "/usr/pluto/src/$SRC_IMPL"; then
	echo "CVS source download failed"
	exit $ERR_DOWNLOAD
fi
# TODO: CVS pserver logout

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
