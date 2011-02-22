#!/bin/bash
. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh

#set -x

#svn_branch_name=LinuxMCE-0810

function Checkout_Svn {
	DisplayMessage "**** STEP : SVN CHECKOUT"

	mkdir -p "$svn_dir"

	pushd ${svn_dir}
		LASTVERSION="1"
		DisplayMessage "Checking out ${svn_url}"
		svn co "${svn_url}" ${svn_branch_name} || Error "Failed to checkout ${svn_url}"
	popd

	if [ "$svn_private_url"  != "" -a \
	     "$svn_private_user" != "" -a \
	     "$svn_private_pass" != "" ] ; then
		pushd ${svn_dir}/${svn_branch_name}/src
			DisplayMessage "Checking out ${svn_private_url}"
			svn co --username "$svn_private_user" \
				--password "$svn_private_pass" \
				${svn_private_url}/src/ZWave/ \
				|| Error "Failed to checkount ${svn_private_url}/src/ZWave"
			svn co --username "$svn_private_user" \
				--password "$svn_private_pass" \
				${svn_private_url}/src/Fiire_Scripts/ \
				|| Error "Failed to checkount ${svn_private_url}/src/Fiire_Scripts"
			svn co --username "$svn_private_user" \
				--password "$svn_private_pass" \
				${svn_private_url}/src/RFID_Interface/ \
				|| Error "Failed to checkount ${svn_private_url}/src/RFID_Interface"
		popd
	fi

	cp -R ${svn_dir}/${svn_branch_name} ${svn_dir}/${svn_branch_name}-last
}

function Update_Svn {
	DisplayMessage "**** STEP : SVN UPDATE"

	pushd ${svn_dir}/${svn_branch_name}-last
		LASTVERSION=$(svn info src |grep ^Revision | cut -d" " -f2)
#		LASTVERSION=`svnversion`
		DisplayMessage "Updating SVN $LASTVERSION from ${svn_url} to latest"
		svn update ||  Error "Failed to update ${svn_url}"
	popd

	if [ "$svn_private_url"  != "" -a \
	     "$svn_private_user" != "" -a \
	     "$svn_private_pass" != "" ] ; then
		pushd ${svn_dir}/${svn_branch_name}/src
			DisplayMessage "Updating ${svn_private_url}"
			pushd ZWave
				svn update --username "$svn_private_user" \
				 --password "$svn_private_pass"  \
				|| Error "Failed to update ${svn_private_url}/src/ZWave"
			popd
			pushd Fiire_Scripts
				svn update --username "$svn_private_user"  \
				--password "$svn_private_pass" \
				|| Error "Failed to update ${svn_private_url}/src/Fiire_Scripts"
			popd
			pushd RFID_Interface
				svn update --username "$svn_private_user"  \
				--password "$svn_private_pass"  \
				|| Error "Failed to update ${svn_private_url}/src/RFID_Interface"
			popd
		popd
	fi
	rm -R ${svn_dir}/${svn_branch_name}
	DisplayMessage "Copying checkout to work"
	cp -R ${svn_dir}/${svn_branch_name}-last ${svn_dir}/${svn_branch_name}
}

mkdir -p "$svn_dir"

if [ -e ${svn_dir}/${svn_branch_name}-last ] ; then
	Update_Svn
else
	Checkout_Svn
fi

pushd ${svn_dir}/${svn_branch_name}
	VERSION=`svnversion`
popd

DisplayMessage "Old version was $LASTVERSION, new version is $VERSION"
