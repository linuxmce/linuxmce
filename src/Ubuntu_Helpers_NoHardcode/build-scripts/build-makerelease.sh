#!/bin/bash
#TODO:
# Implement a system that compiles the bins only when they're changed (similar to replacements)
. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh 

#set -x 
set -e

function Precompile
{
	local pkg_name="$1"
	local pkg_dir="$2"

	local makefile_opt



	DisplayMessage "Precompiling $pkg_name"
	pushd "$pkg_dir"
		if [ -r "Makefile.bootstrap" ]
		then
			makefile_opt="-f Makefile.bootstrap"

		elif [ -r "Makefile.no-wrapper" ]
		then
			makefile_opt="-f Makefile.no-wrapper"

		elif [ -r "Makefile" ]
		then
			makefile_opt=""

		elif [ -r "Makefile.MakeRelease" ]
		then
			makefile_opt="-f Makefile.MakeRelease"

		elif [ -r "Makefile.prep" ]
		then
			makefile_opt="-f Makefile.prep"

		else
			Error "Could not find a Makefile for ${pkg_name} in ${pkg_dir}"
		fi

		echo "SNR_CPPFLAGS=\"\" make $makefile_opt clean"
		SNR_CPPFLAGS="" make $makefile_opt clean || Error "Failed to clean ${pkg_name} to use for MakeRelease"
		echo "SNR_CPPFLAGS=\"\" make $makefile_opt"
		SNR_CPPFLAGS="" make $makefile_opt || Error "Failed to precompile ${pkg_name} to use for MakeRelease"

	 popd


}

function Build_MakeRelease {

	Precompile pluto_main "${svn_dir}/${svn_branch_name}/src/pluto_main"
	Precompile PlutoUtils "${svn_dir}/${svn_branch_name}/src/PlutoUtils"
	Precompile SerializeClass "${svn_dir}/${svn_branch_name}/src/SerializeClass"
	Precompile LibDCE "${svn_dir}/${svn_branch_name}/src/DCE"
	Precompile MakeRelease "${svn_dir}/${svn_branch_name}/src/MakeRelease"
	Precompile MakeRelease_PrepFiles "${svn_dir}/${svn_branch_name}/src/MakeRelease_PrepFiles"

	DisplayMessage "Copy MakeRelease files to ${mkr_dir}"
	mkdir -pv "${mkr_dir}"

	cp -v "${svn_dir}/${svn_branch_name}/src/bin/MakeRelease" "${mkr_dir}"
	cp -v "${svn_dir}/${svn_branch_name}/src/bin/MakeRelease_PrepFiles" "${mkr_dir}"
	if [ -e "${svn_dir}/${svn_branch_name}/src/bin/mysql_wrapper" ]
	then
		cp -v "${svn_dir}/${svn_branch_name}/src/bin/mysql_wrapper" "${mkr_dir}"
	fi
	cp -v "${svn_dir}/${svn_branch_name}/src/lib/"*.so "${mkr_dir}"

#	#TODO: Kill kirill cause i need to copy mysql_wrapper in /usr/pluto/bin
#	mkdir -pv /usr/pluto/bin
#	cp -v "${svn_dir}/${svn_branch_name}/src/bin/mysql_wrapper" /usr/pluto/bin

}

DisplayMessage "**** STEP : PREPARING BUILD SYSTEM (MakeRelease)"
trap 'Error "Undefined error in $0"' EXIT

Build_MakeRelease

trap - EXIT
