#!/bin/bash
#
#
# Small script to compile all needed packages 
#
function BuildPackages(PackagesToBuild) {
	for Package in $(PackagesToBuild)
		pushd $Package
		dpkg-buildpackage
		cd ..
		popd
	done
}

PACKAGES_FIRST_RUN="vdr-1.6.0 vdr-plugin-svdrpservice-0.0.4"
BuildPackages($PACKAGES_FIRST_RUN)
#
# We need to have the svdrpservice-dev package to build the remote timer.
#
dpkg -i svdrpservice-dev
PACKAGES_SECOND_RUN="vdr-plugin-remotetimers-0.1.1~dia1 vdr-plugin-streamdev-0.3.3~cvs20080406.1321 vdr-plugin-xineliboutput-1.0.4"
BuildPackages($PACKAGES_SECOND_RUN)
#
# But we do not need to clutter the builder with this package.
#
dpkg -r svdrpservice-dev