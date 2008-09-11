#!/bin/bash

. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/Config_Ops.sh

DEVICECATEGORY_Video_Cards=125
DEVICECATEGORY_Media_Director=8
DEVICETEMPLATE_GeForce_or_TNT2=1736
DEVICETEMPLATE_Radeon_8500_or_newer=1721
DEVICETEMPLATE_Unichrome=1814

#<-mkr_b_via_b->
Install_VIA_pkgs()
{
	local Pkgs_VIA
	Pkgs_VIA=(via-alsa-2.6.16.20-pluto-2-686 via-xorg)

	echo "$(date -R) --> Finding installed VIA (ALSA, X) packages"
	VIA_inst="$(InstalledPackages "${Pkgs_VIA[@]}")"
	echo "$(date -R) <-- Finding installed VIA (ALSA, X) packages"

	if [[ -z "$VIA_inst" ]]; then
		apt-get -y -f install "${Pkgs_VIA[@]}"
	fi

	#cp -af /usr/pluto/via-xorg/* /
	depmod -a
	rmmod via || :
	modprobe via || :
}
#<-mkr_b_via_e->

# Clean up video driver packages
# When a video card is removed/replaced, remove its packages
# The new drivers for video cards are installed later in this script, not in this function
CleanupVideo()
{
	modprobe -r nvidia
	modprobe nvidia

	local MD
	MD=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Media_Director" "" parent)

	if [[ -z "$MD" ]]; then
		echo "$(date -R) --- CleanupVideo: this machine is not a hybrid or MD; no video card operations required"
		return 0
	fi

	echo "$(date -R) --> CleanupVideo"
	
	# Check for video card changes and update system accordingly
	# Package name lists are treated as prefixes
	local Pkgs_nVidia
	Pkgs_nVidia=(nvidia-driver nvidia-glx "nvidia-kernel-.*" pluto-nvidia-video-drivers)
	local Pkgs_ATI
	Pkgs_ATI=(xorg-driver-fglrx pluto-ati-video-drivers)
	local Pkgs_VIA
	Pkgs_VIA=(xserver-xorg-video-viaprop)
	local nV_inst ATI_inst VIA_inst
	local nV_dev ATI_dev VIA_dev
	local Pkg

	# Find installed packages
	echo "$(date -R) --> Finding installed packages (nVidia)"
	nV_inst="$(InstalledPackages "${Pkgs_nVidia[@]}")"
	echo "$(date -R) --- Installed packages (nVidia): " $nV_inst
	echo "$(date -R) <-- Finding installed packages (nVidia)"

	echo "$(date -R) --> Finding installed packages (ATI)"
	ATI_inst="$(InstalledPackages "${Pkgs_ATI[@]}")"
	echo "$(date -R) --- Installed packages (ATI): " $ATI_inst
	echo "$(date -R) <-- Finding installed packages (ATI)"

	echo "$(date -R) --> Finding installed packages (VIA)"
	VIA_inst="$(InstalledPackages "${Pkgs_VIA[@]}")"
	echo "$(date -R) --- Installed packages (VIA): " $VIA_inst
	echo "$(date -R) <-- Finding installed packages (VIA)"
	# END - Find installed packages

	SubComputer=$(FindDevice_Category $PK_Device 8)
	if [[ -z "$SubComputer" ]]; then
		SubComputer="$PK_Device"
	fi

	# Find desired video card
	echo "$(date -R) --> Retreiving desired video card (nVidia)"
	nV_dev="$(FindDevice_Template $SubComputer $DEVICETEMPLATE_GeForce_or_TNT2 'norecursion')"
	echo "$(date -R) <-- Retreiving desired video card (nVidia)"

	echo "$(date -R) --> Retreiving desired video card (ATI)"
	ATI_dev="$(FindDevice_Template $SubComputer $DEVICETEMPLATE_Radeon_8500_or_newer 'norecursion')"
	echo "$(date -R) <-- Retreiving desired video card (ATI)"

	echo "$(date -R) --> Retreiving desired video card (VIA)"
	VIA_dev="$(FindDevice_Template $SubComputer $DEVICETEMPLATE_Unichrome 'norecursion')"
	echo "$(date -R) <-- Retreiving desired video card (VIA)"
	# END - Find desired video card

	# Add proprietary video card Device and drivers if needed
	# TODO: allow user to express his/her will in using the open source driver if he/she so desires
	echo "$(date -R) --> Auto-create video card device"
	VideoDriver=$(GetVideoDriver)
	case "$VideoDriver" in
		nv|nvidia)
			if [[ -z "$nV_dev" ]]; then
				NewDeviceTemplate=$DEVICETEMPLATE_GeForce_or_TNT2
				nV_dev=$(/usr/pluto/bin/CreateDevice -d "$NewDeviceTemplate" -R "$PK_Device")
			fi
		;;
		ati|radeon|fglrx)
			if [[ -z "$ATI_dev" ]]; then
				NewDeviceTemplate=$DEVICETEMPLATE_Radeon_8500_or_newer
				ATI_dev=$(/usr/pluto/bin/CreateDevice -d "$NewDeviceTemplate" -R "$PK_Device")
			fi
		;;
#<-mkr_b_via_b->
		viaprop) # Proprietary VIA drivers
			if [[ -z "$VIA_dev" ]]; then
				Install_VIA_pkgs
				NewDeviceTemplate=$DEVICETEMPLATE_Unichrome
				VIA_dev=$(/usr/pluto/bin/CreateDevice -d "$NewDeviceTemplate" -R "$PK_Device")
			fi
		;;
#<-mkr_b_via_e->
	esac
	echo "$(date -R) <-- Auto-create video card device"

	echo "$(date -R) --> Performing package purges"
	if [[ -n "$nV_inst" && -z "$nV_dev" ]]; then
		apt-get -y remove --purge $nV_inst
	elif [[ -n "$ATI_inst" && -z "$ATI_dev" ]]; then
		apt-get -y remove --purge $ATI_inst
	elif [[ -n "$VIA_inst" && -z "$VIA_dev" ]]; then
		apt-get -y remove --purge $VIA_inst
	fi
	echo "$(date -R) <-- Performing package purges"

	echo "$(date -R) --> Configuring X"
	bash -x /usr/pluto/bin/Xconfigure.sh --update-video-driver | tee-pluto /var/log/pluto/Xconfigure.log
	echo "$(date -R) <-- Configuring X"

	echo "$(date -R) <-- CleanupVideo"
}
