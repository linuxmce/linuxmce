#!/bin/bash -e

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/env.sh

if [[ -f "$log_file" ]] ;then
	mv "$log_file" "$log_file.$(date '+%Y%m%d-%H%M%S')"
fi

"${build_scripts_dir}/checkout-svn.sh"
"${build_scripts_dir}/import-win32bins.sh"

"${build_scripts_dir}/build-replacements.sh"
"${build_scripts_dir}/build-makerelease.sh"
"${build_scripts_dir}/import-databases.sh"

"${build_scripts_dir}/build-maindebs.sh"
"${build_scripts_dir}/get-closed-source-debs.sh"

"${build_scripts_dir}/create-repo.sh"

#"${build_scripts_dir}/cd1-build.sh"
#"${build_scripts_dir}/cd2-build.sh"

if [ x"$do_not_build_sl_dvd" != x"yes" ] ; then

# Build the SingleLayer DVD
pushd /usr/local/lmce-build/vmware-install/
	/usr/local/lmce-build/vmware-install/install_in_vmware.sh "single-layer"
popd 
pushd /usr/local/lmce-build/build-dvd
	/usr/local/lmce-build/build-dvd/build-cd.sh "LinuxMCE-DVD-${arch}.iso"
popd

fi

if [ x"$do_not_build_dl_dvd" != x"yes" ] ; then

# Build the DoubleLayer DVD
pushd /usr/local/lmce-build/vmware-install/
	/usr/local/lmce-build/vmware-install/install_in_vmware.sh
popd 
pushd /usr/local/lmce-build/build-dvd
	/usr/local/lmce-build/build-dvd/build-cd.sh "LinuxMCE-DVD-DL-${arch}.iso" big
popd

fi
wget http://vt100.at/announce.php?text=in\ vitro\ fluffy\ is\ done -O /dev/null
