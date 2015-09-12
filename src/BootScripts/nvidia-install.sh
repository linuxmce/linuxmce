#!/bin/bash

. /usr/pluto/bin/Utils.sh
#######################################################################################################################
# BEGIN NVIDIA DRIVER HELPER FUNCTIONS
#######################################################################################################################
# TODO: Possibly separate these into their own file and include that file here in Utils.sh?

# These functions are to help determine which driver to install for the currently installed nVidia card,
# as well as determining if vdpau support is available on the installed card.

# Information was gleened from these sources.
# ftp://download.nvidia.com/XFree86/Linux-x86/190.42/README/appendix-a.html
# http://www.nvnews.net/vbulletin/showthread.php?s=2f34370027899f55166907809e838706&t=122606

# These will need redone for each LMCE release, depending on the drivers available to the base kubuntu installation
#######################################################################################################################

LogFile="/var/log/pluto/nvidia-install.log";

# DRIVER LISTS
# Make lists of all PCI_Id's supported under each driver version
Driver_Current_Supported="0410 0565 0568 05e0 05e1 05e2 05e3 05e6 05ea 05eb 0600 0601 0602 0603 0605 0607 0608 0609 060a 060b 060c 060f 0611 0612 0613 0614 0615 0617 0618 0621 0622 0626 0627 0628 062a 062c 062d 062e 0631 0632 0633 0637 0640 0641 0643 0646 0647 0649 064a 064c 0652 0653 0654 065b 067a 0680 06c0 06c4 06ca 06cd 0865 0874 0876 087d 087e 087f 0974 0a26 0a27 0a7a 0dc0 0dcd 0dce 0dd2 0dd6 0df4 0df5 0df6 0df7 0dfa 0dfc 0e30 0e31 0e3a 0e3b 0fce 0fd1 0fd2 0fd3 0fd4 0fd5 0fd8 0fe0 0ffb 0ffc 0fff 1040 1042 1048 104a 1050 1051 1054 1055 1056 1057 107d 1082 1088 1091 1094 1096 109a 109b 10c5 1180 1188 1189 118f 11a0 1200 1201 1203 1210 1211 1241 1244 1245 1247 1251 0565 0568 0621 0630 0633 067a 0680 0840 0869 08a0 08a2 08a3 0a32 0cac 0ca0 0ca5 0ca7 0dc5 0dc6 0dd1 0dd3 0dda 0de8 0de9 0dea 0ded 0dee 0def 0df9 0f00 0f01 0fc0 0fc1 0fc2 1049 1052 1058 1059 105a 1084 1086 1087 1089 108b 10cc 10c0 10cc 1205 1206 1207 1208 1212 1213 1243 1246 1248 1249 124b 124d 20dd 212a 212b 212c 0040 0041 0042 0043 0044 0045 0046 0047 0048 004e 0090 0091 0092 0093 0095 0098 0099 009d 00c0 00c1 00c2 00c3 00c8 00c9 00cc 00cd 00ce 00f1 00f2 00f3 00f4 00f5 00f6 00f8 00f9 0140 0141 0142 0143 0144 0145 0146 0147 0148 0149 014a 014c 014d 014e 014f 0160 0161 0162 0163 0164 0165 0166 0167 0168 0169 016a 0191 0193 0194 0197 019d 019e 01d0 01d1 01d2 01d3 01d6 01d7 01d8 01da 01db 01dc 01dd 01de 01df 0221 0222 0240 0241 0242 0244 0245 0247 0290 0291 0292 0293 0294 0295 0297 0298 0299 029a 029b 029c 029d 029e 029f 02e0 02e1 02e2 02e3 02e4 038b 0390 0391 0392 0393 0394 0395 0397 0398 0399 039c 039e 03d0 03d1 03d2 03d5 03d6 0400 0401 0402 0403 0404 0405 0406 0407 0408 0409 040a 040b 040c 040d 040e 040f 0420 0421 0422 0423 0424 0425 0426 0427 0428 0429 042a 042b 042c 042d 042e 042f 0531 0533 053a 053b 053e 0595 05e7 05ed 05f8 05f9 05fd 05fe 05ff 0604 0606 060d 0610 0619 061a 061b 061c 061d 061e 061f 0623 0625 062b 0635 0638 063a 0644 0645 0648 064b 0651 0656 0658 0659 065a 065c 068f 0697 06d1 06d2 06d8 06d9 06da 06dc 06dd 06de 06df 06e0 06e1 06e2 06e3 06e4 06e5 06e6 06e7 06e8 06e9 06ea 06eb 06ec 06ef 06f1 06f8 06f9 06fa 06fb 06fd 06ff 0743 0772 077a 07e0 07e1 07e2 07e3 07e5 082f 0843 0844 0845 0846 0847 0848 0849 084a 084b 084c 084d 084f 0860 0861 0862 0863 0864 0866 0867 0868 086a 086c 086d 086e 086f 0870 0871 0872 0873 087a 08a4 08a5 0914 0a20 0a22 0a23 0a28 0a29 0a2a 0a2b 0a2c 0a2d 0a34 0a35 0a38 0a3c 0a60 0a62 0a63 0a64 0a65 0a66 0a67 0a68 0a69 0a6a 0a6c 0a6e 0a6f 0a70 0a71 0a72 0a73 0a74 0a75 0a76 0a78 0a7c 0ca2 0ca3 0ca4 0ca8 0ca9 0caf 0cb0 0cb1 0cbc 0dc4 0dd8 0de0 0de1 0de2 0de4 0de5 0deb 0dec 0df0 0df1 0df2 0df3 0df8 0e22 0e23 0e24 1080 1081 10c3 10d8 212c"

Driver_173_Supported="00fa 00fb 00fc 00fd 00fe 0301 0302 0308 0309 0311 0312 0314 031a 031b 031c 0320 0321 0322 0323 0324 0325 0326 0327 0328 032a 032b 032c 032d 0330 0331 0332 0333 0334 0338 033f 0341 0342 0343 0344 0347 0348 034c 034e"

# Make lists of all PCI_Id's that support each vdpau revision
VDPAU_Rev_A_Supported="016d 0400 0401 0402 0403 0404 0405 0407 0408 0409 040a 040b 040c 040d 040e 040f 0410 0421 0422 0424 0425 0426 0427 0428 0429 042a 042b 042c 042d 042e 042f 0595 05e0 05e1 05e2 05e3 05e6 05e7 05ea 05eb 05ed 05f8 05f9 05fd 05fe 05ff 0600 0601 0602 0603 0604 0605 0606 0607 0608 0609 060a 060b 060c 060d 060f 0610 0611 0612 0613 0614 0615 0617 0618 0619 061a 061b 061c 061d 061e 061f 0621 0622 0623 0625 0626 0627 0628 062a 062b 062c 062d 062e 0630 0633 0638 063a 0640 0641 0643 0644 0645 0646 0647 0648 0649 064a 064b 064c 0651 0652 0653 0654 0655 0656 0658 0659 065a 065b 065c 068f 0697 06e4 0743 0ca0 0ca7 10c3"

VDPAU_Rev_B_Supported="06e0 06e1 06e2 06e5 06e6 06e8 06e9 06ea 06eb 06ec 06ef 06f1 06f8 06f9 06fa 06fb 06fd 06ff 0840 0844 0845 0846 0847 0848 0849 084a 084b 084c 084d 0860 0861 0862 0863 0864 0865 0866 0867 0869 086a 086c 086d 086e 086f 0870 0871 0872 0873 0874 0876 087a 087d 087e 087f 0a68 0a69 10C0"

VDPAU_Rev_C_Supported="0565 0568 067a 0680 06c0 06c4 06ca 06cd 06d1 06d2 06d8 06d9 06da 06dc 06dd 06de 06df 0772 082f 08a0 08a2 08a3 08a4 08a5 0914 0974 0a20 0a23 0a26 0a27 0a28 0a29 0a2a 0a2b 0a2c 0a2d 0a32 0a34 0a35 0a38 0a3c 0a60 0a62 0a63 0a64 0a65 0a66 0a6a 0a6c 0a6e 0a6f 0a70 0a71 0a72 0a73 0a74 0a75 0a76 0a78 0a7a 0a7c 0ca2 0ca3 0ca4 0ca5 0ca8 0ca9 0caf 0cb0 0cb1 0cbc 0dc0 0dc4 0dc5 0dc6 0dcd 0dce 0dd1 0dd2 0dd3 0dd6 0dd8 0dda 0de0 0de1 0de2 0de4 0de5 0de8 0de9 0dea 0deb 0dec 0ded 0dee 0def 0df0 0df1 0df2 0df3 0df4 0df5 0df6 0df7 0df8 0df9 0dfa 0dfc 0e22 0e23 0e24 0e30 0e31 0e3a 0e3b 0f00 0f01 0fc0 0fc1 0fc2 1049 1050 1052 1058 1059 105a 1080 1081 1082 1084 1086 1087 1088 1089 108b 1091 1094 1096 109a 109b 10c5 10cc 10d8 1200 1201 1203 1205 1206 1207 1208 1210 1211 1212 1213 1241 1243 1244 1245 1246 1247 1248 1249 124b 124d 1251 20dd 212a 212b 212c"

VDPAU_Rev_D_Supported="0fce 0fd1 0fd2 0fd3 0fd4 0fd5 0fd8 0fe0 0ffb 0ffc 0fff 1042 1048 104a 1051 1054 1055 1056 1057 107d 1180 1188 1189 118f 11a0"

Backports_Required="0565 0568 0621 0630 0633 067a 0680 0840 0869 08a0 08a2 08a3 0a20 0a22 0a23 0a28 0a29 0a2a 0a2b 0a2d 0a32 0a34 0a35 0a60 0a62 0a63 0a64 0a65 0a66 0a6a 0a6c 0a6e 0a6f 0a70 0a71 0a72 0a73 0a74 0a75 0a76 0a78 0a7a 0a7c 0ca2 0ca3 0ca4 0ca8 0ca9 0caf 0cb0 0cb1 0dc4 0de0 0de1 0de2 0de4 0de5 0df0 0df1 0df2 0df3 0e22 0e23 0e24 0ca5 0dc5 0dc6 0dd3 0dda 0de8 0de9 0dea 0ded 0def 0df9 0f00 0f01 0fc0 0fc1 0fc2 1049 1052 1058 1059 105a 1084 1086 1087 1089 108b 10c0 10cc 1205 1206 1207 1208 1212 1213 1243 1246 1248 1249 124b 124d 20dd 212a 212b 212c 10c3 1080 1081"

# GT_Series="0410 0565 0568 05e0 05e1 05e2 05e3 05e6 05ea 05eb 0600 0601 0602 0603 0605 0607 0608 0609 060a 060b 060c 060f 0611 0612 0613 0614 0615 0617 0618 0621 0622 0626 0627 0628 062a 062c 062d 062e 0631 0632 0633 0637 0640 0641 0643 0646 0647 0649 064a 064c 0652 0653 0654 065b 067a 0680 06c0 06c4 06ca 06cd 0865 0874 0876 087d 087e 087f 0a32 0ca0 0ca5 0ca7 0cac 0dc5 0dc6 0dd1 0dd3 0de8 0de9 0ded 0dee 0f00 0f01 0fc0 0fc1 0fc2 1049 1052 1084 1086 1087 1089 108b 10cc 1205 1206 1207 1208 1212 1213 1243 1246 1248 1249 124b 124d 20dd 212a 212b 212c"
#######################################################################################################################
# getPCI_Id()
# returns the last 4 digits of the PCI ID of the currently installed nVidia card.
getPCI_Id() {
	vga_pci=$(lspci -nn | grep -w 'VGA' | sed 's/.*://;s/\].*//')
	gpus=$(echo "$vga_pci" | sort -u | wc -l)
		if [[ "$gpus" -gt "1" ]]; then 
			vga_pci=$(echo "$vga_pci" | awk 'NR==2') 
		fi
	echo "$vga_pci"
}


# getNvidiaInstalled()
# returns true if an nvidia card is installed in the system, false otherwise
getNvidiaInstalled() {
	if ! lspci -nn | grep -iq "vga.*nvidia";then
		return 1
	else
		return 0
	fi
}

# getInstalledNvidiaDriver()
# echos the currently installed nVidia driver (if there is one)
getInstalledNvidiaDriver() {
	(dpkg-query -l "nvidia-173" "nvidia-current" | grep "^ii" | awk '{print $2}') 2>/dev/null
}

# getPreferredNvidiaDriver()
# returns the preferred driver to install for the currently installed nVidia card.
getPreferredNvidiaDriver() {
	PCI_Id=$(getPCI_Id)
 
	case " $Driver_Current_Supported " in *" $PCI_Id "*)
		echo "nvidia-current"
		return 1
	esac

	case " $Driver_173_Supported " in *" $PCI_Id "*)
		echo "nvidia-173"
		return 1
	esac

	echo "nvidia-current"
	return 0
}

# Returns which vdpau revision (A,B or C) supported by the currently installed nVidia card.
# returns nothing if vdpau is NOT supported!
getVDPAUSupport() {
	PCI_Id=$(getPCI_Id)

	case " $VDPAU_Rev_A_Supported " in *" $PCI_Id "*)
		echo "A"
		return 1
	esac
	case " $VDPAU_Rev_B_Supported " in *" $PCI_Id "*)
		echo "B"
		return 1
	esac
	case " $VDPAU_Rev_C_Supported " in *" $PCI_Id "*)
		echo "C"
		return 1
	esac
	case " $VDPAU_Rev_D_Supported " in *" $PCI_Id "*)
		echo "D"
		return
	esac
	# Could not map a PCI_Id to a supported vdpau revision
	return 0
}

# This function will install the correct nVidia driver for the currently installed card.
# if the first parameter is set to "reboot", it will also reboot the system after the driver is installed.
installCorrectNvidiaDriver() {
	# first, lets see if there is even an nvidia card installed in the system
	# If there is no nVidia card even installed, lets get out of here.
	if ! getNvidiaInstalled; then
		return
	fi

	echo "*************************************************"
	echo -n "       " 
	StatusMessage "Begin nVidia driver installation"
	echo "*************************************************"
	Log "$LogFile" "== Begin NVidia driver installation ($(date)) =="
	Log "$LogFile" "Card Detected: $(lspci -nn | grep -vi 'non-vga' | grep -i 'vga')"
	Log "$LogFile" "PCI_Id Detected: $(getPCI_Id)"

	# Now lets get the currently installed nvidia driver (if there is one)
	current_driver=$(getInstalledNvidiaDriver)
	if echo "$current_driver" | grep "nvidia"; then
		echo "Detected installed driver $current_driver"
		Log "$LogFile" "Detected installed driver $current_driver"
	fi

	# Get the preferred driver to install
	preferred_driver=$(getPreferredNvidiaDriver)
	echo "LMCE prefers driver $preferred_driver"
	Log "$LogFile" "LMCE prefers driver $preferred_driver"

	# Install the driver if needed
	if [[ "$current_driver" != "$preferred_driver" ]]; then
		markUp="1"
	fi

	if [[ "$markUp" == "1" ]]; then
		echo "installing NEW driver $preferred_driver!"
		Log "$LogFile" "installing NEW driver $preferred_driver!"
		tmpfile=$( /bin/mktemp -t )

		if [[ -n "$current_driver" ]]; then
			apt-get remove -yf "$current_driver" --force-yes
		fi

		apt-get install -yf "$preferred_driver" 2> >(tee "$tmpfile")
		local param="reboot"


	else
		echo "Preferred driver $preferred_driver already installed."
		Log "$LogFile" "Preferred driver $preferred_driver already installed."
		Installed="0"
	fi

	echo "*************************************************"
	echo -n "       "
	NotifyMessage "End NVidia driver installation"
	echo "*************************************************"
	echo ""

	Log "$LogFile" "== End NVidia driver installation ($(date)) =="

	# Reboot only if requested and a new driver was installed
	if [[ "$param" == "reboot" ]];then
	#if [[ "$Installed" == "1" ]]; then
		# Give the user a warning message and beep, then reboot
		echo ""
		StatusMessage "Nvidia driver installation requires a reboot."
		NotifyMessage "Please stand by while your system is rebooted."
		echo ""
		ConfSet AVWizardOverride 1
		beep -l 100 -f 500 -d 50 -r 3
		sleep 2
		reboot
	fi
}
#######################################################################################################################
# END NVIDIA DRIVER HELPER FUNCTIONS
#######################################################################################################################
