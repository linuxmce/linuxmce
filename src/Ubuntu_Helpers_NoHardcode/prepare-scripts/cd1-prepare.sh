#!/bin/bash
. /etc/lmce-build/builder.conf

rm -rf "${build_dir}/cd1-packages"
mkdir -p "${build_dir}/cd1-packages"

pushd "${build_dir}/cd1-packages"
	aptitude download `cat /etc/lmce-build/cd1-packages`
popd

