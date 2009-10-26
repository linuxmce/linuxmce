#!/bin/bash

. /etc/lmce-build/builder.conf

set -e

echo ""
echo "********************************************************************************"
echo "*** Running: $0"
echo "********************************************************************************"

rm -rf "${build_dir}/cd2-packages"
mkdir -pv "${build_dir}/cd2-packages/cachecd1-cache"

pushd "${build_dir}/cd2-packages/cachecd1-cache"
	aptitude -y --force-yes download `cat /etc/lmce-build/cd2-packages`
popd

echo "*** Done: $0"

