#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh
. /usr/local/lmce-build/common/utils.sh

set -e
#set -x

DisplayMessage "*** STEP: Creating LINUXMCE CD 1"
trap 'Error "Undefined error in $0"' EXIT

iso_name="LinuxMCE-CD1.iso"

# Prepare the directory in whith the cd will be built
DisplayMessage "Preparing temp directory to create the iso"
temp_dir=$(mktemp -d)
mkdir -p ${temp_dir}/deb-cache

# Copy the debs that where created in local_mirror_dir on this cd
DisplayMessage "Copy packages from the local repository"
find "$local_mirror_dir" -follow -maxdepth 1 -type f -name '*.deb' -print0 | xargs -0 -i -t cp '{}' "${temp_dir}/deb-cache"

# Remove blacklisted packages from this cd
DisplayMessage "Remove package thare are on the repository but should not be on CD"
while read file; do
	rm -f "${temp_dir}/deb-cache/$file"_*.deb
done </etc/lmce-build/cd1-packages-blacklist

# Add some extra packages to this cd
DisplayMessage "Add extra packages that are not in repository but should be on CD"
for pack in "${build_dir}/cd1-packages/"*.deb ;do
	cp "$pack" "${temp_dir}/deb-cache"
done

# copy installer in the root of the disc
DisplayMessage "Copying installer to the root of the CD"
cp "${local_mirror_dir}"/mce-installer*.deb "${temp_dir}"

# Remove duplicate debs and keep the latest version only
DisplayMessage "Removing duplicate debs from the CD"
remove_duplicate_debs "${temp_dir}/deb-cache"

# Generate the iso
DisplayMessage "Generating the ISO"
mkisofs -f -joliet-long -r -o "${local_mirror_dir}/${iso_name}" "${temp_dir}"

DisplayMessage "Removing temporary dir"
## Clean the temporary dir
rm -rf "${temp_dir}"

trap - EXIT
