#!/bin/bash
#TODO:
# Implement a system that compiles the bins only when they're changed (similar to replacements)
. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh 

set -x 
set -e

function Build_MakeRelease {

	DisplayMessage "Precompiling pluto_main"
	pushd "${svn_dir}/${svn_branch_name}/src/pluto_main"
	SNR_CPPFLAGS="" make clean || Error "Failed to clean pluto_main to use for MakeRelease"
	SNR_CPPFLAGS="" make || Error "Failed to precompile pluto_main to use for MakeRelease"
	popd

	DisplayMessage "Precompiling PlutoUtils"
	pushd "${svn_dir}/${svn_branch_name}/src/PlutoUtils"
	SNR_CPPFLAGS="" make clean || Error "Failed to clean PlutoUtils to use for MakeRelease"
	SNR_CPPFLAGS="" make || Error "Failed to precompile PlutoUtils"
	popd

	DisplayMessage "Precompiling SerializeClass"
	pushd "${svn_dir}/${svn_branch_name}/src/SerializeClass"
	SNR_CPPFLAGS="" make clean || Error "Failed to clean SerializeClass to use for MakeRelease"
	SNR_CPPFLAGS="" make || Error "Failed to precompile SerializeClass"
	popd

	DisplayMessage "Precompiling LibDCE"
	pushd "${svn_dir}/${svn_branch_name}/src/DCE"
	SNR_CPPFLAGS="" make clean || Error "Failed to clean LibDCE to use for MakeRelease"
	SNR_CPPFLAGS="" make || Error "Failed to precompile LibDCE"
	popd

	DisplayMessage "Precompiling MakeRelease"
	pushd "${svn_dir}/${svn_branch_name}/src/MakeRelease"
	SNR_CPPFLAGS="" make -f Makefile.MakeRelease clean || Error "Failed to clean MakeRelease"
	SNR_CPPFLAGS="" make -f Makefile.MakeRelease || Error "Failed to precompile MakeRelease"
	popd

	DisplayMessage "Precompiling MakeRelease_PrepFiles"
	pushd "${svn_dir}/${svn_branch_name}/src/MakeRelease_PrepFiles"
	SNR_CPPFLAGS="" make clean || Error "Failed to clean MakeRelease_PrepFiles"
	SNR_CPPFLAGS="" make || Error "Failed to precompile MakeRelease_PrepFiles"
	popd

	DisplayMessage "Precompiling mysql_wrapper"
	pushd "${svn_dir}/${svn_branch_name}/src/mysql_wrapper"
	SNR_CPPFLAGS="" make clean || Error "Failed to clean mysql_wrapper"
	SNR_CPPFLAGS="" make || Error "Failed to precompile mysql_wrapper"
	popd

	DisplayMessage "Copy MakeRelease files to ${mkr_dir}"
	mkdir -p "${mkr_dir}"
	mkdir -p /usr/pluto/bin

	cp "${svn_dir}/${svn_branch_name}/src/bin/MakeRelease" "${mkr_dir}"
	cp "${svn_dir}/${svn_branch_name}/src/bin/MakeRelease_PrepFiles" "${mkr_dir}"
	cp "${svn_dir}/${svn_branch_name}/src/bin/mysql_wrapper" "${mkr_dir}"
	cp "${svn_dir}/${svn_branch_name}/src/lib/"*.so "${mkr_dir}"

	#TODO: Kill kirill cause i need to copy mysql_wrapper in /usr/pluto/bin
	mkdir -p /usr/pluto/bin
	cp "${svn_dir}/${svn_branch_name}/src/bin/mysql_wrapper" /usr/pluto/bin

}

DisplayMessage "**** STEP : PREPARING BUILD SYSTEM (MakeRelease)"
trap 'Error "Undefined error in $0"' EXIT

Build_MakeRelease

trap - EXIT
