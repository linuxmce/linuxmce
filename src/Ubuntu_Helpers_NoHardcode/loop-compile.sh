#!/bin/bash -ex

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/env.sh

while : ;do
	echo "$(date -R) Started" >> /var/log/loop-build.log	

	if [[ -f "$log_file" ]] ;then
		mv "$log_file" "$log_file.$(date +%d%m%y-%s)"
	fi

	# Start Build
	"${build_scripts_dir}/checkout-svn.sh"

	"${build_scripts_dir}/build-replacements.sh"
	"${build_scripts_dir}/build-makerelease.sh"
	"${build_scripts_dir}/import-databases.sh"

	"${build_scripts_dir}/build-maindebs-sim.sh"
done
