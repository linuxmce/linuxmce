#!/bin/bash

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

. /usr/pluto/bin/Utils.sh
LogFile="/var/log/pluto/nvidia-install.log";

# DRIVER LISTS
# Make lists of all PCI_ID's supported under each driver version
DRIVER_195_SUPPORTED="0040 0041 0042 0043 0044 0045 0046 0047 0048 0090 0091 0092 0093 0095 0098 0099 00c0 00c1 00c2 00c3 00c8 00c9 00f1 00f2 00f3 00f4 00f5 00f6 00f9 0140 0141 0142 0143 0144 0145 0146 0147 0148 0149  014f 0160 0161 0162 0163 0164 0166 0167 0168 0169 016a 0191 0193 0194 0197 01d0 01d1 01d3 01d6 01d7 01d8 01dd 01df 0221 0222 0240 0241 0242 0244 0247 0290 0291 0292 0293 0294 0295 0297 0298 02e0 02e1 02e2  02e3 02e4 0390 0391 0392 0393 0394 0395 0397 0398 0399 03d0 03d1 03d2 03d5 0400 0401 0402 0403 0404 0405 0407 0408 0409 0420 0421 0422 0423 0424 0425 0426 0427 0428 042c 042e 0531 0533 053a 053b 053e 05e0  05e1 05e2 05e3 05e6 05eb 0600 0601 0602 0604 0605 0606 0608 0609 060a 060b 060c 060d 0610 0611 0612 0613 0614 0615 0617 0618 0622 0623 0625 0626 0627 0628 062a 062b 062c 0640 0641 0643 0644 0646 0647 0648  0649 064a 064b 064c 0652 0656 06e0 06e1 06e4 06e5 06e6 06e8 06e9 06ec 06ef 07e0 07e1 07e3 0844 0845 0847 0848 0849 084a 084b 084c 084d 084f 0862 0863 086c 0872 0873 087d 0a20 0a2a 0a34 0a60 0a74 0ca8 0ca9 004e 009d 00cc 00cd 00ce 00f8 014a 014c 014d 014e 0165 019d 019e 01da 01db 01dc 01de 0245 0299 029a 029b 029c 029d 029e 029f 039e 040a 040b 040c 040d 040e 040f 0429 042a 042b 042d 042f 05f9 05fd 05fe 05ff 061a 061c 06ad 061e 061f 0638 063a 0658 0659 065a 065c 06ea 06eb 06f8 06f9 06fa 06fd 087a"

DRIVER_173_SUPPORTED="00fa 00fb 00fc 00fd 00fe 0301 0302 0308 0309 0311 0312 0314 031a 031b 031c 0320 0321 0322 0323 0324 0325 0326 0327 0328 032a 032b 032c 032d 0330 0331 0332 0333 0334 0338 033f 0341 0342 0343 0344 0347 0348 034c 034e"

DRIVER_96_SUPPORTED="0110 0111 0112 0113 0170 0171 0172 0173 0174 0175 0176 0177 0178 0179 017a 017c 017d 0181 0182 0183 0185 0188 018a 018b 018c 01a0 01f0 0200 0201 0202 0203 0250 0251 0253 0258 0259 025b 0280 0281 0282 0286 0288 0289 028c"

DRIVER_71_SUPPORTED="0020 0028 0029 002c 002d 00a0 0100 0101 0103 0150 0151 0152 0153"



# VDPAU LISTS
# Make lists of all PCI_ID's that support each vdpau revision
VDPAU_REV_A_SUPPORTED="0400 0401 0402 0403 0404 0405 0405 0408 0409 0421 0422 0424 0425 0426 0427 0428 042c 042e 05e0 05e1 05e2 05e3 05e6 05eb 0600 0601 0602 0604 0605 0606 0608 0609 060a 060b 060c 060d 0610 0611 0612 0613 0614 0615 0617 0618 0622 0623 0625 0626 0627 0628 062a 062b 062c 0640 0641 0643 0644 0646 0647 0648 0649 064a 064b 064c 0652 0656 06E4 040a 040b 040c 040d 040e 040f 0429 042a 042b 042d 042f 05f9 05fd 05fe 05ff 061a 061c 016d 061e 061f 0638 063a 0658 0659 065a 065c"

VDPAU_REV_B_SUPPORTED="06e0 06e1  06e5 06e6 06e8 06e9 06ec 06ef 0844 0845 0847 0848 0849 084a 084b 084c 084d 0862 0863 086c 0872 0873 087d 06ea 06eb 06f8 06f9 06fa 06fd 087a"

VDPAU_REV_C_SUPPORTED="0a20 0a2a 0a34 0a60 0a74 0ca8 0ca9"


#######################################################################################################################


# getPCI_ID()
# returns the last 4 digits of the PCI ID of the currently installed nVidia card.
getPCI_ID() {
	echo $(lspci -nn | grep -vi "non-vga" | grep -i vga | sed 's/.*://;s/\].*//'
)
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
	echo $(dpkg-query -l "nvidia-glx\*" | grep "^ii" | awk '{print $2}')
	#echo "$(dpkg -l | grep -i "ii .*" | grep -i 'nvidia-glx' | awk '{print $2}')" 
}

# getPreferredNvidiaDriver()
# returns the preferred driver to install for the currently installed nVidia card.
getPreferredNvidiaDriver() {
	PCI_ID=$(getPCI_ID)

	case " $DRIVER_195_SUPPORTED " in *" $PCI_ID "*) 
		echo "nvidia-glx-195"
		return 1
	esac
	
	case " $DRIVER_173_SUPPORTED " in *" $PCI_ID "*)
		echo "nvidia-glx-173"
		return 1
	esac

	case " $DRIVER_96_SUPPORTED " in *" $PCI_ID "*)
		echo "nvidia-glx-96"
		return 1
	esac

	case " $DRIVER_71_SUPPORTED " in *" $PCI_ID "*)
		echo "nvidia-glx-71"
		return 0
	esac

	#Could not map PCI_ID to a correct nVidia device. Return a default
	echo "nvidia-glx-195"
	return 0		


}

# getVDPAUSupport()
# Returns which vdpau revision (A,B or C) supported by the currently installed nVidia card.
# returns nothing if vdpau is NOT supported!
getVDPAUSupport() {
	PCI_ID=$(getPCI_ID)

	case " $VDPAU_REV_A_SUPPORTED " in *" $PCI_ID "*)
		echo "A"
		return 1
	esac
	case " $VDPAU_REV_B_SUPPORTED " in *" $PCI_ID "*)
		echo "B"
		return 1
	esac
	case " $VDPAU_REV_C_SUPPORTED " in *" $PCI_ID "*)
		echo "C"
		return
	esac
	
	# Could not map a PCI_ID to a supported vdpau revision
	return 0
}

# installCorrectNvidiaDriver()
# This function will install the correct nVidia driver for the currently installed card.
# if the first parameter is set to "reboot", it will also reboot the system after the driver is installed.
installCorrectNvidiaDriver() {
	#see if the caller wanted to reboot after installing packages
	param="$1"


	# first, lets see if there is even an nvidia card installed in the system
	# If there is no nVidia card even installed, lets get out of here.
	if ! getNvidiaInstalled;then
		return
	fi

	echo "*************************************************"
	echo "       Begin NVidia driver installation          "
	echo "*************************************************"
	Log "$LogFile" "== Begin NVidia driver installation ($(date)) =="
	Log "$LogFile" "Card Detected: $(lspci -nn | grep -vi non-vga | grep -i vga)"
	Log "$LogFile" "PCI_ID Detected: $(getPCI_ID)"

	# Now lets get the currently installed nvidia driver (if there is one)
	current_driver=$(getInstalledNvidiaDriver)
	if [[ "$current_driver" != "" ]];then
		echo "Detected installed driver $current_driver"
		Log "$LogFile" "Detected installed driver $current_driver"
	fi

	# Get the preferred driver to install
	preferred_driver=$(getPreferredNvidiaDriver)
	echo "LMCE prefers driver $preferred_driver"
	Log "$LogFile" "LMCE prefers driver $preferred_driver"

	# Install the driver if needed
	if [[ "$current_driver" != "$preferred_driver" ]]; then
		echo "installing NEW driver $preferred_driver!"
		Log "$LogFile" "installing NEW driver $preferred_driver!"
		INSTALLED="1"
		apt-get install -y $preferred_driver
	else
		echo "Preferred driver $preferred_driver already installed."
		Log "$LogFile" "Preferred driver $preferred_driver already installed."
		INSTALLED="0"
	fi

	echo "*************************************************"
	echo "       End NVidia driver installation          "
	echo "*************************************************"
	echo ""

	Log "$LogFile" "== End NVidia driver installation ($(date)) =="

	# Reboot only if requested and a new driver was installed
	if [[ "$param" == "reboot" && "$INSTALLED" == "1" ]];then
		# Give the user a warning message and beep, then reboot
		echo ""
		echo -e "\e[1;31mNvidia driver installation requires a reboot.\e[0m"
		echo -e "\e[1;31mPlease stand by while your system is rebooted.\e[0m"
		echo ""
		beep -l 100 -f 500 -d 50 -r 3
		sleep 5
		reboot
	fi
}
#######################################################################################################################
# END NVIDIA DRIVER HELPER FUNCTIONS
#######################################################################################################################


