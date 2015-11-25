#!/bin/bash

# setup the interactor and the firstboot script
update-rc.d -f firstboot defaults >/dev/null

###########################################################
### Setup global variables
###########################################################

MD_Preseed () {
	update-rc.d -f kdm remove || :
	update-rc.d -f sddm remove || :
	update-rc.d -f lightdm remove || :
	update-rc.d -f NetworkManager remove || :

	echo '/bin/false' >"/etc/X11/default-display-manager"

	cat <<-EOF > /etc/apt/apt.conf.d/30pluto
		// Pluto apt conf add-on
		#APT::Cache-Limit "43554432";
		Dpkg::Options { "--force-confold"; };
		Acquire::http::timeout "10";
		Acquire::ftp::timeout "10";
		APT::Get::AllowUnauthenticated "true";
		EOF
}



###########################################################
### Main execution area
###########################################################

#Function execution
MD_Preseed

#Exit successfully
exit 0
