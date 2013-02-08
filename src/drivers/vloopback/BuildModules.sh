#!/bin/bash

set -ex

. /etc/lmce-build/builder.conf

COMP_MODULES_DIR="$(pwd)/compiledModules"

mkdir -p "$COMP_MODULES_DIR"

function BuildModules() {
	export KVER="$1"
	echo "# Building for kernel $KVER"
	export KDIR="/lib/modules/$KVER/build"
	export KSRC="$KDIR"
	export KERNEL="$KVER"

	if [[ ! -d "$KDIR" ]] ;then
		echo "# Cannot find kernel sources of $KVER in $KDIR"
		exit 1
	fi

	## Build specific
	make clean
	make
	## Build specific

	mkdir -p "$COMP_MODULES_DIR/$KVER/misc"
	find '(' -type d -name 'compiledModules' -prune -false ')' -or -name '*.ko' -exec cp  '{}' "$COMP_MODULES_DIR/$KVER/misc" ';'
}

# The KERN variable is set in builder.conf
#KERN=$(uname -r)
KERN="$KVER"
BuildModules "$KERN"
echo "depmod \"$KERN\" 2>/dev/null || /bin/true" >> mkr_postinst.sh
chmod +x mkr_postinst.sh
