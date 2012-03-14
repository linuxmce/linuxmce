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
DRIVER_260_SUPPORTED="0040 0041 0042 0043 0044 0045 0046 0047 0048 0090 0091 0092 0093 0095 0098 0099 00c0 00c1 00c2 00c3 00c8 00c9 00f1 00f2 00f3 00f4 00f5 00f6 00f9 0140 0141 0142 0143 0144 0145 0146 0147 0148 0149 014f 0160 0161 0162 0163 0164 0166 0167 0168 0169 016a 0191 0193 0194 0197 01d0 01d1 01d2 01d3 01d6 01d7 01d8 01dd 01df 0221 0222 0240 0241 0242 0244 0247 0290 0291 0292 0293 0294 0295 0297 0298 02e0 02e1 02e2 02e3 02e4 038b 0390 0391 0392 0393 0394 0395 0397 0398 0399 03d0 03d1 03d2 03d5 03d6 0400 0401 0402 0403 0404 0405 0406 0407 0408 0409 0410 0420 0421 0422 0423 0424 0425 0426 0427 0428 042c 042e 0531 0533 053a 053b 053e 05e0 05e1 05e2 05e3 05e6 05e7 05e7 0595 05e7 068f 05e7 0697 05e7 0743 05ea 05eb 0600 0601 0602 0603 0604 0605 0606 0607 0608 0609 060a 060b 060c 060d 060f 0610 0611 0612 0613 0614 0615 0617 0618 0622 0623 0625 0626 0627 0628 062a 062b 062c 062d 062e 0631 0632 0635 0637 0640 0641 0643 0644 0645 0646 0647 0648 0649 064a 064b 064c 0651 0652 0653 0654 0656 065b 06c0 06c4 06ca 06cd 06d1 06d1 0772 06d2 06de 06de 077a 06de 082f 06de 0843 06de 0846 06df 06e0 06e1 06e2 06e3 06e4 06e5 06e6 06e7 06e8 06e9 06ec 06ef 06f1 07e0 07e1 07e2 07e3 07e5 0844 0845 0846 0847 0848 0849 084a 084b 084c 084d 084f 0860 0861 0862 0863 0864 0865 0866 0867 0868 086a 086c 086d 086e 086f 0870 0871 0872 0873 0874 0876 087a 087d 087e 087f 0a20 0a22 0a23 0a28 0a29 0a2a 0a2b 0a2d 0a34 0a35 0a60 0a62 0a63 0a64 0a65 0a66 0a67 0a68 0a69 0a6e 0a6f 0a70 0a71 0a72 0a73 0a74 0a75 0ca0 0ca2 0ca3 0ca4 0ca7 0ca8 0ca9 0cac 0caf 0cb0 0cb1 0dc4 0dd1 0de1 0dee 0df0 0df1 0df2 0df3 0e22 0e23 0e24 1080 1081 10c3 004e 009d 00cc 00cd 00ce 00f8 014a 014c 014d 014e 0165 019d 019e 01da 01db 01dc 01de 0245 0299 029a 029b 029c 029d 029e 029f 039c 039e 040a 040b 040c 040d 040e 040f 0429 042a 042b 042d 042f 05ed 05f8 05f9 05fd 05fe 05ff 0619 061a 061b 061c 061d 061e 061f 0638 063a 0658 0659 065a 065c 06d8 06d9 06da 06dc 06dd 06ea 06eb 06f8 06f9 06fa 06fb 06fd 06ff 0a2c 0a38 0a3c 0a6a 0a6c 0a78 0a7c 0cbc 0dd8 0dd8 0914 0df8 10d8"

DRIVER_173_SUPPORTED="0040 0041 0042 0043 0044 0045 0046 0047 0048 0090 0091 0092 0093 0095 0098 0099 00c0 00c1 00c2 00c3 00c8 00c9 00f0 00f1 00f2 00f3 00f4 00f5 00f6 00f9 00fa 00fb 00fc 0140 0141 0142 0143 0144 0145 0146 0147 0148 0149 014f 0160 0161 0162 0163 0164 0166 0167 0168 0169 016a 0191 0193 0194 0197 01d0 01d1 01d3 01d6 01d7 01d8 01dd 01df 0211 0212 0215 0218 0221 0222 0240 0241 0242 0244 0247 0290 0291 0292 0293 0294 0295 0297 0298 0299 02e0 02e1 02e3 02e4 0301 0302 0311 0312 0314 031a 031b 0320 0321 0322 0323 0324 0325 0326 0327 0328 032c 032d 0330 0331 0332 0333 0334 0341 0342 0343 0344 0347 0348 0390 0391 0392 0393 0394 0395 0398 0399 03d0 03d1 03d2 03d5 0400 0401 0402 0403 0404 0405 0407 0408 0409 0420 0421 0422 0423 0424 0425 0426 0427 0428 042e 0531 0533 053a 053b 053e 0600 0602 0604 0606 0609 060c 060d 0610 0611 0612 0622 0647 0648 0649 064b 06e4 06e5 06e8 06e9 07e0 07e1 07e3 0844 0848 0849 084a 084b 084f 004e 009d 00cc 00cd 00ce 00f8 00fc 00fd 00fe 014a 014c 014d 014e 0165 019d 019e 01d7 01da 01db 01dc 01de 0245 029a 029b 029c 029d 029e 029f 0308 0309 031c 032a 032b 0338 033f 034c 034e 039e 040a 040b 040c 040d 040e 040f 0429 042a 042b 042d 042f 061a 061c"

DRIVER_96_SUPPORTED="0040 0041 0043 0044 0045 0046 0047 0048 004e 0090 0091 0092 0093 0098 0099 009d 00c0 00c1 00c2 00c3 00c8 00c9 00cc 00cd 00ce 00f0 00f1 00f2 00f3 00f4 00f5 00f6 00f8 00f9 00fa 00fb 00fc 00fd 00fe 00ff 0110 0111 0112 0113 0140 0141 0142 0143 0144 0145 0146 0148 0149 014a 014c 014e 014f 0160 0161 0164 0165 0166 0167 0168 0170 0171 0172 0173 0174 0175 0176 0177 0178 0179 017a 017c 017d 0181 0182 0183 0185 0188 018a 018b 018c 01a0 01d1 01d7 01d7 01d8 01da 01db 01dc 01de 01df 01f0 0200 0201 0202 0203 0211 0212 0215 0218 0240 0241 0242 0250 0251 0253 0258 0259 025b 0280 0281 0282 0286 0288 0289 028c 0290 0291 0298 0299 029a 029b 029c 029d 029e 029f 02e1 0301 0302 0308 0309 0311 0312 0314 031a 031b 031c 0320 0321 0322 0323 0324 0325 0326 0327 0328 032a 032b 032c 032d 0330 0331 0332 0333 0334 0338 033f 0341 0342 0343 0344 0347 0348 034c 034e 0391 0392 0398 039e 0020 0028 0029 002c 002d 00a0 0100 0101 0103 0150 0151 0152 0153"

DRIVER_71_SUPPORTED="0020 0028 0029 002c 002d 00a0 0100 0101 0103 0150 0151 0152 0153"



# VDPAU LISTS
# Make lists of all PCI_ID's that support each vdpau revision
VDPAU_REV_A_SUPPORTED="0400 0401 0402 0403 0404 0405 0407 0408 0409 0410 0421 0422 0424 0425 0426 0427 0428 042c 042e 05e0 05e1 05e2 05e3 05e6 05e7 05ea 05eb 0600 0601 0602 0603 0604 0605 0606 0607 0608 0609 060a 060b 060c 060d 060f 0610 0611 0612 0613 0614 0615 0617 0618 0622 0623 0625 0626 0627 0628 062a 062b 062c 062d 062e 0640 0641 0643 0644 0645 0646 0647 0648 0649 064a 064b 064c 0651 0652 0653 0654 0656 065b 06E4 0ca0 0ca7 040a 040b 040c 040d 040e 040f 0429 042a 042b 042d 042f 05ed 05f8 05f9 05fd 05fe 05ff 0619 061a 061b 061c 016d 061e 061f 0638 063a 0658 0659 065a 065c"

VDPAU_REV_B_SUPPORTED="06e0 06e1 06e2 06e5 06e6 06e8 06e9 06ec 06ef 06f1 0844 0845 0846 0847 0848 0849 084a 084b 084c 084d 0860 0861 0862 0863 0864 0865 0866 0867 086a 086c 086d 086e 086f 0870 0871 0872 0873 0874 0876 087a 087d 087e 087f 0a68 0a69 06ea 06eb 06f8 06f9 06fa 06fb 06fd 06ff"

VDPAU_REV_C_SUPPORTED="06c0 06c4 06ca 06cd 06d1 06d2 06de 06df 0a20 0a23 0a28 0a29 0a2a 0a2b 0a2d 0a34 0a35 0a60 0a62 0a63 0a64 0a65 0a66 0a6e 0a6f 0a70 0a71 0a72 0a73 0a74 0a75 0ca2 0ca3 0ca4 0ca8 0ca9 0caf 0cb0 0cb1 0dc4 0dd1 0de1 0dee 0df0 0df1 0df2 0df3 0e22 0e23 0e24 1080 1081 06d8 06d9 06da 06dd 0a2c 0a3c 0a6a 0a6c 0a78 0a7c 0cbc 0dd8 0df8 10d8"


#######################################################################################################################


# getPCI_ID()
# returns the last 4 digits of the PCI ID of the currently installed nVidia card.
getPCI_ID() {
        lspci -nn | grep -vi "non-vga" | grep -i vga | sed 's/.*://;s/\].*//'
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
        dpkg-query -l "nvidia-glx*"| grep "^ii" | awk '{print $2}'
}

# getPreferredNvidiaDriver()
# returns the preferred driver to install for the currently installed nVidia card.
getPreferredNvidiaDriver() {
        PCI_ID=$(getPCI_ID)
        DISTRO="$(lsb_release -c -s)"

        case " $DRIVER_260_SUPPORTED " in *" $PCI_ID "*)
                        echo "nvidia-glx-260"
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

        #Could not map PCI_ID to a correct nVidia device.
        echo "nvidia-glx-260"
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
                tmpfile=$( /bin/mktemp -t )
                apt-get install -y $preferred_driver 2> >(tee $tmpfile)
                if [[ $? > 0 ]] ; then
                        ERROR=$( cat $tmpfile )
                        INSTALLED="0"
                        echo ""
                        echo -e "\e[1;31mUnable to install $preferred_driver!\e[0m"
                        echo -e "\e[1;31mRefer to the above message, which is also logged to $LogFile.\e[0m"
                        echo ""
                        Log "$LogFile" "Unable to install driver:"
                        Log "$LogFile" "$ERROR"
                        beep -l 100 -f 500 -d 50 -r 3
                        sleep 10
                fi
                rm $tmpfile

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

        # Reboot only if a new driver was installed
        if [[ "$INSTALLED" == "1" ]];then
                # Give the user a warning message and beep, then reboot
                echo ""
                echo -e "\e[1;31mNvidia driver installation requires a reboot.\e[0m"
                echo -e "\e[1;31mPlease stand by while your system is rebooted.\e[0m"
                echo ""
				#need to force AVWizard to run, and remove the xorg.conf file created by the nvidia installer so it starts clean
				rm /etc/X11/xorg.conf*
				ConfSet "AVWizardOverride" "1"
                beep -l 100 -f 500 -d 50 -r 3
                sleep 2
                reboot
        fi
}
#######################################################################################################################
# END NVIDIA DRIVER HELPER FUNCTIONS
#######################################################################################################################
