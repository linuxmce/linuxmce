#!/bin/bash

# Usage:
#	with no params - scan LinuxMCE media library for DVD images and process them
#	with foldername passed - scan folder for DVD images and process them

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

export DVDCSS_CACHE="$DVDKeysCache"

CheckKeysForDVD() {
    echo -n `date`"    Checking keys for $1 ... "
    if [ -f "$1.keys.tar.gz" ]; then
	echo -e "keys are already present, skipping\n"
	return
    fi
    
    echo "not present, trying to retrieve them from DVD image"

    KeysPath=`/usr/pluto/bin/dvdcssHelper.sh -f "$1" | grep "CACHE_DIR:" | sed 's/CACHE_DIR://'`
    if [ -z "$KeysPath" ]; then
	echo -e "Error retrieving keys: cannot find DVDCSS keys dir for this DVD image\n"
	return
    fi

    DumpStatus=`/usr/pluto/bin/dvdcssHelper.sh -d "$1"`
    if [ "$DumpStatus" != "KEY_ERROR_COUNT:0" ]; then
	echo "While retrieving keys: not all keys can be retrieved from DVD image, trying to fetch them from real DVD copy of keys"
	RealKeyCandidateMask=`echo $KeysPath | sed 's/0000000000$//' | sed 's/0000000000\/$//'`
	RealKeyCandidate=`ls -d $RealKeyCandidateMask* | grep -m 1 -v "0000000000$"`
	if [ -z "$RealKeyCandidate" ]; then 
		echo -e "Error retrieving keys: copying from real DVD copy failed\n"
		return
	else
		cp -f $RealKeyCandidate/* $KeysPath/
		echo "Copied keys from real DVD copy"
	fi
    fi

    KeysFolder=`basename $KeysPath`
    KeysCacheFolder=`dirname $KeysPath`
    RealDVDKeysFolderMask=`echo $KeysFolder | sed 's/0000000000$//'`

    pushd $KeysCacheFolder > /dev/null
    tar zcf "$1.keys.tar.gz.in-progress" $RealDVDKeysFolderMask*
    popd > /dev/null
    mv "$1.keys.tar.gz.in-progress" "$1.keys.tar.gz"
    echo -e "Keys retrieved OK\n"    
}


if [ ! -f "/usr/lib/libdvdcss.so.2" ]; then
	echo "Error: no libdvdcss installed. Please install it and re-run this script"
	exit 1
fi

echo `date`"   Started"

if [[ -z "$1" ]]; then
	echo "Scanning LinuxMCE media library for DVD images"
	UseDB "pluto_media"
	Query="SELECT CONCAT_WS(\"/\", Path, Filename) FROM File WHERE Filename LIKE \"%.dvd\" AND Missing=0"
	Result=$(RunSQL "$Query")

	set -o noglob # prevent shell expansion in for's list
	for Row in $Result; do
        	Item=$(Field 1 "$Row")
		set +o noglob
		Item=$(echo "$Item" | tr  $'\x1' ' ')
        	CheckKeysForDVD "$Item"
	done
else
	echo "Scanning folder $1 for DVD images"
	IFS=$'\n'

	for Item in `find "$1" -name \*.dvd`; do
		CheckKeysForDVD "$Item"
	done
fi

echo -e `date`"   Finished\n"
