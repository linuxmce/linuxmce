#!/bin/bash
. /etc/lmce-build/builder.conf

set -e

echo ""
echo "********************************************************************************"
echo "*** Running: $0"
echo "********************************************************************************"

rm -rf "${build_dir}/cd1-packages"
mkdir -pv "${build_dir}/cd1-packages"

if [ ! -e /etc/apt/sources.list.pbackup ] ;then
	cp -v /etc/apt/sources.list /etc/apt/sources.list.pbackup
fi

slim_repo="deb http://debian.slimdevices.com/ stable main"
if ! grep -qF "$slim_repo" /etc/apt/sources.list; then
	echo "$slim_repo" >> /etc/apt/sources.list
	apt-get update
fi

avenard_repo="deb http://www.avenard.org/files/ubuntu-repos/ intrepid release testing"
if ! grep -qF "$avenard_repo" /etc/apt/sources.list; then
	echo "$avenard_repo" >> /etc/apt/sources.list
	apt-get update
fi

pushd "${build_dir}/cd1-packages"
	aptitude -y --force-yes download `cat /etc/lmce-build/cd1-packages`
popd

echo "*** Done: $0"

