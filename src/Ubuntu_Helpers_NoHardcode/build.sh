#!/bin/bash -e

. /etc/lmce-build/builder.conf

if [[ -f "$log_file" ]] ;then
	mv "$log_file" "$log_file.$(date +%d%m%y-%s)"
fi

build_scripts_dir="/usr/local/lmce-build/build-scripts"

"${build_scripts_dir}/checkout-svn.sh"
"${build_scripts_dir}/import-win32bins.sh"

"${build_scripts_dir}/build-replacements.sh"
"${build_scripts_dir}/build-makerelease.sh"
"${build_scripts_dir}/import-databases.sh"

"${build_scripts_dir}/build-maindebs.sh"
"${build_scripts_dir}/get-closed-source-debs.sh"

"${build_scripts_dir}/create-repo.sh"

"${build_scripts_dir}/cd1-build.sh"
"${build_scripts_dir}/cd2-build.sh"

# Build the SingleLayer DVD
pushd /usr/local/lmce-build/vmware-install/
	/usr/local/lmce-build/vmware-install/install_in_vmware.sh "single-layer"
popd 
pushd /usr/local/lmce-build/build-dvd
	/usr/local/lmce-build/build-dvd/build-cd.sh "LinuxMCE-DVD-${arch}.iso"
popd

# Build the DoubleLayer DVD
pushd /usr/local/lmce-build/vmware-install/
	/usr/local/lmce-build/vmware-install/install_in_vmware.sh
popd 
pushd /usr/local/lmce-build/build-dvd
	/usr/local/lmce-build/build-dvd/build-cd.sh "LinuxMCE-DVD-DL-${arch}.iso" big
popd
