#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh
. /usr/local/lmce-build/common/utils.sh

set -e
#set -x

DisplayMessage "*** STEP: Creating LINUXMCE CD 2"
trap 'Error "Undefined error in $0"' EXIT

iso_name="LinuxMCE-CD2.iso"

# Copy some extra debs to the CD
DisplayMessage "Copy some extra debs to CD2"
rm -f "${build_dir}/cd2-packages/cachecd1-cache"/video-wizard-videos_*.deb
cp "${extras_dir}"/video-wizard-videos_*.deb "${build_dir}/cd2-packages/cachecd1-cache"
rm -f "${build_dir}/cd2-packages/cachecd1-cache"/tee-pluto_*.deb
cp "${replacements_dir}"/tee-pluto_*.deb "${build_dir}/cd2-packages/cachecd1-cache"

# Remove duplicate debs and keep the latest version only
DisplayMessage "Removing duplicate debs from the CD"
remove_duplicate_debs "${build_dir}/cd2-packages/cachecd1-cache"

# Build our new iso
DisplayMessage "Building the ISO"
pushd "${build_dir}"
	mkisofs -f -J -r -o "${local_mirror_dir}/${iso_name}" ./cd2-packages
popd

trap - EXIT
