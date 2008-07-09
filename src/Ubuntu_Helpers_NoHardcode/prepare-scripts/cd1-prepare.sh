#!/bin/bash
. /etc/lmce-build/builder.conf

rm -rf "${build_dir}/cd1-packages"
mkdir -p "${build_dir}/cd1-packages"

Src="deb http://debian.slimdevices.com/ stable main"
if [ ! -e /etc/apt/sources.list.pbackup ] ;then
	cp /etc/apt/sources.list /etc/apt/sources.list.pbackup
fi
if ! grep -qF "$Src" /etc/apt/sources.list; then
	echo "$Src" >> /etc/apt/sources.list
	apt-get update
fi

pushd "${build_dir}/cd1-packages"
	aptitude download `cat /etc/lmce-build/cd1-packages`
popd

