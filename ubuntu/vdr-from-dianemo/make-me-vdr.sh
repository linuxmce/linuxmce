#!/bin/bash
#
#
# Small script to compile all needed packages 
#
# 2009-06-14 	- First useful version of the script
# 		- Use of PATCHVARIANT variable to point to the lmce version
#		  of debian/.vdr-patches, debian/patches/00list, debia/abi-version
#		- Adding vdr-plugin-vompserver

function BuildPackages() {
	PackagesToBuild=$1
	echo $PATCHVARIANT
	read
	for Package in $PackagesToBuild
	do
		pushd  "$Package"
		PATCHVARIANT=$PATCHVARIANT dpkg-buildpackage
		cd ..
		popd
	done
	
}


PATCHVARIANT="lmce"
PACKAGES_FIRST_RUN="vdr-1.6.0 vdr-plugin-svdrpservice-0.0.4"

BuildPackages $PACKAGES_FIRST_RUN
#
# We need to have the svdrpservice-dev package to build the remote timer.
#
dpkg -i svdrpservice-dev
PACKAGES_SECOND_RUN="vdr-plugin-remotetimers-0.1.1~dia1 vdr-plugin-streamdev-0.3.3~cvs20080406.1321 vdr-plugin-xineliboutput-1.0.4 vdr-plugin-vompserver-0.3.0"
BuildPackages $PACKAGES_SECOND_RUN
#
# But we do not need to clutter the builder with this package.
#
dpkg -r svdrpservice-dev
