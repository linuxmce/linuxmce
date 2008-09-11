#!/bin/bash
. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh

function Checkout_Svn {
	DisplayMessage "**** STEP : SVN CHECKOUT"


	DisplayMessage "Removing old svn checkout dir"
	[[ -d "$svn_dir" ]] && mkdir -p "$svn_dir"
	rm -rf "${svn_dir}/trunk"
	
	local AllParts="src ubuntu web misc_utils installers config-pkgs"
	for svn_module in ${AllParts}; do
		mkdir -p "${svn_dir}/trunk/$svn_module"
		DisplayMessage "Checking out ${svn_url}/$svn_module"
		svn co "${svn_url}/${svn_module}"  "${svn_dir}/trunk/$svn_module" || Error "Failed to checkout ${svn_url}/$svn_module"
	done

	if [[ "$svn_private_url" != "" ]] && [[ "$svn_private_user" != "" ]] && [[ "$svn_private_pass" != "" ]] ;then
		pushd ${svn_dir}/trunk/src
			DisplayMessage "Checking out ${svn_private_url}"
			svn co --username "$svn_private_user" --password "$svn_private_pass" ${svn_private_url}/src/ZWave/          || Error "Failed to checkount ${svn_private_url}/src/ZWave"
			svn co --username "$svn_private_user" --password "$svn_private_pass" ${svn_private_url}/src/Fiire_Scripts/  || Error "Failed to checkount ${svn_private_url}/src/Fiire_Scripts"
			svn co --username "$svn_private_user" --password "$svn_private_pass" ${svn_private_url}/src/RFID_Interface/ || Error "Failed to checkount ${svn_private_url}/src/RFID_Interface"
		popd
	fi
}

Checkout_Svn 
