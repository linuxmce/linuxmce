#!/bin/sh


[ -z "$build_scripts_dir" ] && build_scripts_dir="/usr/local/lmce-build/build-scripts"
[ -z "$lmce_build_conf_dir" ] && lmce_build_conf_dir="/etc/lmce-build"


# Setup environment
if [ -z "$KVER" ]
then
	KVER=`grep linux-headers $lmce_build_conf_dir/build-packages | cut -b 15-99`
	KVER_SHORT=$(echo $KVER | cut -d '-' -f1)
	KVER_VERY_SHORT=$(echo $KVER | cut -d '.' -f1).$(echo $KVER | cut -d '.' -f2)
	KDIR=/usr/src/linux-source-$(echo $KVER | cut -d '-' -f1)
	export KVER KVER_SHORT KVER_VERY_SHORT KDIR
fi


