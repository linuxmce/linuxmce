#!/bin/bash

if [[ $SVNROOT == "" ]]; then
	if [[ $1 == "" ]]; then exit; fi
	SVNROOT="$1"
fi

package_dir=$SVNROOT/build/cd1-packages
package_list=$SVNROOT/src/Ubuntu_Helpers/cd1-packages-whitelist

rm -rf $package_dir
mkdir -p $package_dir

pushd $package_dir
	aptitude download `cat $package_list`
popd

