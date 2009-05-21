#!/bin/bash

set -ex

COMP_MODULES_DIR="$(pwd)/compiledModules"
. /etc/lmce-build/builder.conf

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
	pushd driver
	make
	popd
	## Build specific

	mkdir -p "$COMP_MODULES_DIR/$KVER/misc"
	find '(' -type d -name 'compiledModules' -prune -false ')' -or -name '*.ko' -exec cp  '{}' "$COMP_MODULES_DIR/$KVER/misc" ';'
}

MakeRelease_Kernel=$KVER
if [[ "$MakeRelease_Kernel" == "" ]] ;then
	MakeRelease_Kernel=$(uname -r)
fi

echo "#!/bin/bash" > mkr_postinst.sh
for KERN in ${MakeRelease_Kernel} ;do
	BuildModules "$KERN"
	echo "depmod \"$KERN\" 2>/dev/null || /bin/true" >> mkr_postinst.sh
done
echo "modprobe pvrusb2 || :" >> mkr_postinst.sh
chmod +x mkr_postinst.sh
