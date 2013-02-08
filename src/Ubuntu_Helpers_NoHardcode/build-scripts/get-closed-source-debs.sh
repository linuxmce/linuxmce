#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh

set -e
#set -x

DisplayMessage "*** STEP: Download closed source debs"
trap 'Error "Undefined error in $0"' EXIT

private_debs_dir="$build_dir/private_debs"

function Download_Debs() {
	mkdir -p "$private_debs_dir"
	pushd "$private_debs_dir"
		DisplayMessage "Getting latest ID"
		LatestID=$(2>/dev/null wget -O - "$closed_source_debs_url/latest" || :)

		if [[ "$LatestID" != "" ]] ;then
			DisplayMessage "Downloading latest closed source debs"
			wget -O  "$LatestID.tar" "$closed_source_debs_url/$LatestID.tar"
			rm -f latest.tar
			ln -s "$LatestID.tar" latest.tar
		fi
		
	popd
}

function Unpack_Debs() {
	DisplayMessage "Unpacking closed source debs"
	mkdir -p "$out_dir"/tmp
	pushd "$out_dir"/tmp
		tar -xvf "$private_debs_dir"/latest.tar
	popd
}


if [ x"$closed_source_debs_url" = x"" ] ; then
	DisplayMessage "Nothing to download, will use local debs."
else
	Download_Debs
	Unpack_Debs
fi

trap - EXIT
