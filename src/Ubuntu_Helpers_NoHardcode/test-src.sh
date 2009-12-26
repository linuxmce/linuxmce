#!/bin/bash

# USE AT YOUR OWN RISK! DOES NOT PERFORM ANY ERROR OR VALIDITY CHECKING!
# Please improve upon it and add error/validity checkign if you want this!

# this is a bit of a cheap hack, but I really like the SQL_Ops library and the builder does have this file in the unprepped source branch

. /etc/lmce-build/builder.conf
. ${svn_dir}/${svn_branch_name}-last/src/BootScripts/SQL_Ops.sh

function Usage() {
	echo "test-src.sh"
	echo "Used to test a package from the unprepped source tree in ${svn_dir}/${svn_branch_name}."
	echo ""
	echo "Usage:"
	echo "test-src.sh <package PK_Package> <optional source PK_Package>"
	echo ""
	echo "Some packages don't have an associated source package, so the 2nd source package is optional."
	echo "If a package does have an associated source package, it must be passed in as well."
	echo ""
	echo "Examples:"
	echo "test-src.sh 220"
	echo "     builds pluto-website-admin package (which contains no source package)"
	echo "test-src 280 279"
	echo "     builds pluto-cm11a package (contains a source package)"

}

export PATH=$PATH:${svn_dir}/${svn_branch_name}/src/bin
echo "PATH=$PATH"
export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/${svn_branch_name}/src/lib"
echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"

if [ $# -eq 1 ] ;then
	if [ $1 == "--help" ] ;then
		Usage
		exit 1
	elif [ $1 = -h ] ;then
		Usage
		exit 1
	else
		Package=$1
	fi
elif [ $# -eq 2 ]  ;then
	Package=$1
	Package_Source=$2
else
	Usage
	exit 1
fi


#copy over source files
echo "Copying unprepped sources"
# get a list of source directories
if [ $Package_Source ] ;then
	echo "Package Source Provided!"
	Q="SELECT PK_Package_Directory, Path FROM Package_Directory WHERE FK_Package=$Package_Source AND InputPath IS NULL"
else
	echo "Package Source NOT Provided!"
	Q="SELECT PK_Package_Directory, InputPath FROM Package_Directory WHERE FK_Package=$Package AND FlipSource=1"
fi

UseDB pluto_main

R=$(RunSQL "$Q")

#for each directory, get the sources and copy them!
for thisR in $R ;do
	thisDirectory=$(Field 1 "$thisR")
	thisPath=$(Field 2 "$thisR")
	
	# Note:  I originally had another loop that selected all entries from Package_Directory_File
	#        and copied them over individually, but it would fail to build that way.
	#        So we will copy the entire directory over (same as what build.sh does)
	
	echo "  copying source files in $thisPath..."
	mkdir -p ${svn_dir}/${svn_branch_name}/$thisPath
	rm -rf ${svn_dir}/${svn_branch_name}/$thisPath/*
	cp -Rfp ${svn_dir}/${svn_branch_name}-last/$thisPath/* ${svn_dir}/${svn_branch_name}/$thisPath
	# Perform Search&Replace on the sources

	echo " Performing search'n'replace on the sources in this directory..."
	"${mkr_dir}/MakeRelease_PrepFiles" -p "${svn_dir}/${svn_branch_name}/$thisPath" \
		-e "*.prep,*.cpp,*.h,Makefile*,*.php,*.sh,*.pl,*.awk" \
		-c "/etc/lmce-build/${flavor}.conf" || Error "MakeRelease_PrepFiles failed"

done

echo " Cleaning MakeRelease debs output directory"
rm -rf "${out_dir}" || Error "Cannot clean MakeRelease debs output directory"
mkdir -p "${out_dir}" || Error "Cannot create MakeRelease debs output directory"

PKGS=$Package
if [ $Package_Source ] ;then
	PKGS=$PKGS,$Package_Source
fi

echo " Making Package by calling test-pkg.sh"
./test-pkg.sh $PKGS






