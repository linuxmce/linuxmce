#!/bin/bash

if [[ -n "$HEADER_Utils" ]]; then
	return 0
fi
HEADER_Utils=included

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

DEVICETEMPLATE_OnScreen_Orbiter=62
DEVICEDATA_Use_OpenGL_effects=172
DEVICEDATA_Use_alpha_blended_UI=169
DEVICEDATA_PK_UI=104

function AddBookmark {
	return 0
	Url="$1"
	Name="$2"

	[[ ! -f /home/public/bookmarks.html ]] || return 0

	# Generate an id for this bookmark
	ID=$(echo "$Url" | sha1sum)
	ID='rdf:#$'${ID:0:6}

	# Generate the bookmark entry
	Bookmark='   <DT><A HREF="'$Url'" ADD_DATE="1126869382" LAST_MODIFIED="1126869442" ID="'$ID'">'$Name'</A>'
	Bookmark=$(echo $Bookmark | sed 's/"/\\\"/g')

	for BookmarksFile in /home/public/bookmarks.html /home/user_*/bookmarks.html ;do
		# See if the bookmark is already there
		if grep -q "ID=\"$ID\"" $BookmarksFile ;then
			continue
		fi

		# Add the bookmark string to the file
		awk '
			BEGIN { HR=0 }
			HR==0 && /<HR>/ {print "'"$Bookmark"'"; HR=1}
			{print}
		' $BookmarksFile > $BookmarksFile.$$
		mv $BookmarksFile.$$ $BookmarksFile
	done

}

function DelBookmark {
	return 0
	Url="$1"
	Name="$2"

	[[ ! -f /home/public/bookmarks.html ]] || return 0

	# Generate an id for this bookmark
	ID=$(echo "$Url" | sha1sum)
	ID='rdf:#$'${ID:0:6}
	
	for BookmarksFile in /home/public/bookmarks.html /home/user_*/bookmarks.html ;do
		grep -v "ID=\"$ID\"" $BookmarksFile >> $BookmarksFile.$$
		mv $BookmarksFile.$$ $BookmarksFile
	done
}

UseAlternativeLibs() 
{
	export LD_LIBRARY_PATH=/opt/libsdl/lib:/opt/libxine/lib:/opt/libsdl1.2-1.2.7+1.2.8cvs20041007/lib:/opt/linphone-1.3.5/lib
}

function ListTemplates_Category {
	local FK_DeviceCategory_Parent="$1"
	local PK_DeviceTemplate_List=""
	local PK_DeviceCategory=""
	local PK_DeviceTemplate=""

	R=$(RunSQL "SELECT PK_DeviceCategory FROM DeviceCategory WHERE FK_DeviceCategory_Parent = $FK_DeviceCategory_Parent")
	for PK_DeviceCategory in $R ;do
		PK_DeviceTemplate_List="$PK_DeviceTemplate_List $(ListTemplates_Category "$PK_DeviceCategory")"
	done

	R=$(RunSQL "SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE FK_DeviceCategory=$FK_DeviceCategory_Parent")
	for PK_DeviceTemplate in $R ;do
		PK_DeviceTemplate_List="$PK_DeviceTemplate_List $PK_DeviceTemplate"
	done

	echo $PK_DeviceTemplate_List
}


FindDevice_Template()
{
	local PK_Device_Parent="${1//\'}" FK_DeviceTemplate="${2//\'}" NoRecursion="$3" IncludeParent="$4" All="$5"
	local Limit Found=0

	if [[ -z "$PK_Device_Parent" || -z "$FK_DeviceTemplate" ]]; then
		echo ""
		return 1
	fi

	if [[ -z "$All" ]]; then
		Limit="LIMIT 1"
	fi

	local i R Q
	if [[ "$PK_Device_Parent" == 0 ]]; then
		Q="
			SELECT PK_Device
			FROM Device
			WHERE FK_Device_ControlledVia IS NULL AND FK_DeviceTemplate IN ($FK_DeviceTemplate)
			$Limit
		"
	elif [[ -z "$IncludeParent" ]]; then
		Q="
			SELECT PK_Device
			FROM Device
			WHERE FK_Device_ControlledVia='$PK_Device_Parent' AND FK_DeviceTemplate IN ($FK_DeviceTemplate)
			$Limit
		"
	else
		Q="
			SELECT PK_Device
			FROM Device
			WHERE (FK_Device_ControlledVia='$PK_Device_Parent' OR PK_Device='$PK_Device_Parent') AND FK_DeviceTemplate IN ($FK_DeviceTemplate))
			$Limit
		"
	fi
	R="$(RunSQL "$Q")"

	if [[ -n "$R" ]]; then
		echo "$R"
	fi

	if [[ ( -z "$R" || -n "$All" ) && -z "$NoRecursion" ]]; then
		Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$PK_Device_Parent'"
		R="$(RunSQL "$Q")"
		for i in $R; do
			if FindDevice_Template "$i" "$FK_DeviceTemplate" "" "" "$All"; then
				if [[ -z "$All" ]]; then
					return 0
				else
					Found=1
				fi
			fi
		done
	else
		return 1
	fi

	[[ "$Found" -eq 1 ]]
	return $?
}

FindDevice_Category()
{
	local PK_Device_Parent="${1//\'}" FK_DeviceCategory="${2//\'}" NoRecursion="$3" IncludeParent="$4" All="$5"
	local Limit Found=0

	if [[ -z "$PK_Device_Parent" || -z "$FK_DeviceCategory" ]]; then
		echo ""
		return 1
	fi

	if [[ -z "$All" ]]; then
		Limit="LIMIT 1"
	fi

	local i R Q
	if [[ "$PK_Device_Parent" == 0 ]]; then
		Q="
			SELECT PK_Device
			FROM Device
			JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE FK_Device_ControlledVia IS NULL AND FK_DeviceCategory IN ($FK_DeviceCategory)
			$Limit
		"
	elif [[ -z "$IncludeParent" ]]; then
		Q="
			SELECT PK_Device
			FROM Device
			JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE FK_Device_ControlledVia='$PK_Device_Parent' AND FK_DeviceCategory IN ($FK_DeviceCategory)
			$Limit
		"
	else
		Q="
			SELECT PK_Device
			FROM Device
			JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE (FK_Device_ControlledVia='$PK_Device_Parent' OR PK_Device='$PK_Device_Parent') AND FK_DeviceCategory IN ($FK_DeviceCategory)
			$Limit
		"
	fi
	R="$(RunSQL "$Q")"

	if [[ -n "$R" ]]; then
		echo "$R"
	fi

	if [[ ( -z "$R" || -n "$All" ) && -z "$NoRecursion" ]]; then
		Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$PK_Device_Parent'"
		R="$(RunSQL "$Q")"
		for i in $R; do
			if FindDevice_Category "$i" "$FK_DeviceCategory" "" "" "$All"; then
				if [[ -z "$All" ]]; then
					return 0
				else
					Found=1
				fi
			fi
		done
	else
		return 1
	fi

	[[ "$Found" -eq 1 ]]
	return $?
}

# Get the device number for the parent of the given device
# Returns: number or empty string
GetDeviceParent()
{
	local PK_Device="$1"
	Q="
		SELECT FK_Device_ControlledVia
		FROM Device
		WHERE PK_Device='$PK_Device'
	"
	local R=$(RunSQL "$Q")
	
	if [[ "$R" == NULL ]]; then
		R=""
	fi
	echo "$R"
}

# Delete a device and make its children top level
# Does exactly the same thing as CMD_Delete_Device in General_Info_Plugin
DeleteDevice()
{
	local PK_Device="$1"
	local Q R

	# Delete embedded devices
	Q="SELECT PK_Device FROM Device where FK_Device_RouteTo=$PK_Device"
	R=$(RunSQL "$Q")
	for Device in $R; do
		DeleteDevice "$Device"
	done

	local -a Queries
	Queries=(
		"UPDATE Device SET FK_Device_ControlledVia=NULL WHERE FK_Device_ControlledVia=$PK_Device"
		"DELETE FROM Device WHERE PK_Device=$PK_Device"
		"DELETE FROM CommandGroup_Command WHERE FK_Device=$PK_Device"
		"DELETE FROM Device_Command WHERE FK_Device=$PK_Device"
		"DELETE FROM Device_CommandGroup WHERE FK_Device=$PK_Device"
		"DELETE FROM Device_DeviceData WHERE FK_Device=$PK_Device"
		"DELETE FROM Device_DeviceGroup WHERE FK_Device=$PK_Device"
		"DELETE FROM Device_Device_Related WHERE FK_Device=$PK_Device OR FK_Device_Related=$PK_Device"
		"DELETE FROM Device_EntertainArea WHERE FK_Device=$PK_Device"
		"DELETE FROM Device_HouseMode WHERE FK_Device=$PK_Device"
		"DELETE FROM Device_Orbiter WHERE FK_Device=$PK_Device"
		"DELETE FROM Device_StartupScript WHERE FK_Device=$PK_Device"
		"DELETE FROM Device_Users WHERE FK_Device=$PK_Device"
		"DELETE FROM Package_Device WHERE FK_Device=$PK_Device"
		"DELETE FROM PaidLicense WHERE FK_Device=$PK_Device"
		"DELETE FROM Device_Device_Pipe WHERE FK_Device_From=$PK_Device OR FK_Device_To=$PK_Device"
		"DELETE FROM PaidLicense WHERE FK_Device=$PK_Device"
	)

	for Q in "${Queries[@]}"; do
		RunSQL "$Q"
	done
}

XineConfSet()
{
	local Setting="$1"
	local Value="$2"
	local XineConf="${3:-/etc/pluto/xine.conf}"
	
	[[ -z "$Setting" || -z "$Value" ]] && return 1
	
	if grep -qF "$Setting" "$XineConf"; then
		sed -i "s/^.*$Setting:.*$/$Setting:$Value/" "$XineConf"
	else
		echo "$Setting:$Value" >>"$XineConf"
	fi
}

DeviceIsDisabled()
{
	local PK_Device="$1"
	local R Q

	[[ -z "$PK_Device" ]] && return 1

	Q="SELECT COUNT(*) FROM Device WHERE PK_Device='$PK_Device' AND Disabled=0"
	R=$(RunSQL "$Q")

	if [[ -n "$R" && "$R" -eq 0 ]]; then
		return 0 # Device is disabled or does not exist at all
	else
		return 1 # Device exists and is enabled
	fi
}

FindInArray()
{
	local Value="$1" Array i
	shift
	Array=("$@")
	
	for ((i = 0; i < ${#Array[*]}; i++)); do
		if [[ "${Array[$i]}" == "$Value" ]]; then
			echo "$i"
			return 0
		fi
	done
	return 1
}

FindInArray_Prefix()
{
	local Value="$1" Array i
	shift
	Array=("$@")

	for ((i = 0; i < ${#Array[*]}; i++)); do
		if [[ "${Array[$i]}" == "$Value"* ]]; then
			echo "$i"
			return 0
		fi
	done
	return 1
}

# Verify that the given string is a valid IPv4
ValidIP()
{
	local IP="$1"

	local IPn="[0-9]{1,3}"
	local IPpattern="^($IPn)\\.($IPn)\\.($IPn)\\.($IPn)$"
	local i Number

	# Bash 3.1 requires $IPpattern to be quoted, otherwise it gives a syntax error
	# Bash 3.2 requires $IPpattern to be unquoted, otherwise it fails to match the pattern
	# Ubuntu has Bash 3.2
	if [[ ! "$IP" =~ $IPpattern ]]; then
		return 1
	fi

	for ((i = 1; i <= 4; i++)); do
		Number="${BASH_REMATCH[$i]}"
		if ((Number < 0 || Number > 255)); then
			return 1
		fi
	done

	return 0
}

GetVideoDriver()
{
	if [[ -n "$ForceVESA" ]]; then
		echo vesa
		return 0
	fi
	
	local VideoDriver
#<-mkr_b_via_b->
	VideoDriver=viaprop
#<-mkr_b_via_e->
#<-mkr_B_via_b->
	VideoDriver=$(lshwd | grep ' VGA ' | head -1 | sed 's/^.*(\([^()]*\)).*$/\1/')
	# If we only detect a vesa based system, lets see if we can find out what else it might be
	if [[ "$VideoDriver" == "vesa" ]]; then
	{
		DisplayNote=$(lshwd|grep "Display controller"| cut -f 2 -d \|)
		# This should work beautifully for the ASUS eee Box
		if [[ "$DisplayNote" == "Mobile Integrated Graphics Controller (vesa)" ]]; then
		{
			VideoDriver="intel"
		}
		fi
	}		
	fi
	case "$VideoDriver" in
		nv) 
			if PackageIsInstalled nvidia-glx || PackageIsInstalled nvidia-glx-new || PackageIsInstalled nvidia-glx-71 || PackageIsInstalled nvidia-glx-96 || PackageIsInstalled nvidia-glx-173 || PackageIsInstalled nvidia-glx-180 || PackageIsInstalled nvidia-glx-190 || PackageIsInstalled nvidia-glx-195 ;then
			       	VideoDriver="nvidia" 
			fi
		;;
		radeon|ati) PackageIsInstalled xorg-driver-fglrx && VideoDriver="fglrx" ;;
		i810) VideoDriver="intel" ;;
		"") VideoDriver="vesa" ;; # just-in-case default
	esac
#<-mkr_B_via_e->
	echo "$VideoDriver"
}

ReloadDevicesOnThisMachine()
{
	for Dev in $(cat /usr/pluto/locks/pluto_spawned_local_devices.txt); do
		/usr/pluto/bin/MessageSend "$DCERouter" 0 "$Dev" 7 1 163 "Utils.sh ReloadDevicesOnThisMachine"
	done
}

OpenGLeffects()
{
	local Q

	Q="
		SELECT IK_DeviceData
		FROM Device
		LEFT JOIN Device AS Parent ON Parent.PK_Device=Device.FK_Device_ControlledVia
		JOIN Device_DeviceData ON Device.PK_Device=FK_Device
		WHERE
			Device.FK_DeviceTemplate='$DEVICETEMPLATE_OnScreen_Orbiter'
			AND (
				Device.FK_Device_ControlledVia='$PK_Device'
				OR Parent.FK_Device_ControlledVia='$PK_Device'
			)
		AND FK_DeviceData='$DEVICEDATA_Use_OpenGL_effects'
	"
	RunSQL "$Q"
}

AlphaBlendingEnabled()
{
	local Q

	Q="
		SELECT IK_DeviceData
		FROM Device
		LEFT JOIN Device AS Parent ON Parent.PK_Device=Device.FK_Device_ControlledVia
		JOIN Device_DeviceData ON Device.PK_Device=FK_Device
		WHERE
			Device.FK_DeviceTemplate='$DEVICETEMPLATE_OnScreen_Orbiter'
			AND (
				Device.FK_Device_ControlledVia='$PK_Device'
				OR Parent.FK_Device_ControlledVia='$PK_Device'
			)
		AND FK_DeviceData='$DEVICEDATA_Use_alpha_blended_UI'
	"
	RunSQL "$Q"
}

GetDeviceData()
{
	local Q
	local Device="$1" DeviceData="$2"

	Q="
		SELECT IK_DeviceData
		FROM Device_DeviceData
		WHERE FK_Device='$Device' AND FK_DeviceData='$DeviceData'
	"
	Field 1 "$(RunSQL "$Q")"
}

SetDeviceData()
{
	local Q
	local Device="$1" DeviceData="$2" Value="$3"

	Q="
		UPDATE Device_DeviceData
		SET IK_DeviceData='$Value'
		WHERE FK_Device=$Device AND FK_DeviceData=$DeviceData
	"
	RunSQL "$Q"
}

UI_SetOptions()
{
	local OrbiterDev="$1"
	local OpenGLeffects="$2"
	local AlphaBlending="$3"
	local UI_Version="$4"

	# disable OpenGL effects
	Q="
		REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
		VALUES('$OrbiterDev', '$DEVICEDATA_Use_OpenGL_effects', '$OpenGLeffects')
	"
	RunSQL "$Q"
	# disable alpha blending
	Q="
		REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
		VALUES('$OrbiterDev', '$DEVICEDATA_Use_alpha_blended_UI', '$AlphaBlending')
	"
	RunSQL "$Q"
	# select UI
	Q="
		REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
		VALUES('$OrbiterDev', '$DEVICEDATA_PK_UI', '$UI_Version')
	"
	RunSQL "$Q"
}

function GeneratePassword() {
	#TODO: Replace with this --->  </dev/urandom tr -dc A-Za-z0-9_ | head -c8
        local -a alpha1=(Q W E R T Y U I O P A S D F G H J K L Z X C V B N M)
        local -a alpha2=(q w e r t y u i o p a s d f g h j k l z x c v b n m)
        local -a alpha3=(1 2 3 4 5 6 7 8 9 0)

        local pass=""
        pass=$pass"${alpha1[$(($RANDOM%26))]}${alpha1[$(($RANDOM%26))]}${alpha1[$(($RANDOM%26))]}"
        pass=$pass"${alpha2[$(($RANDOM%26))]}${alpha2[$(($RANDOM%26))]}${alpha2[$(($RANDOM%26))]}"
        pass=$pass"${alpha3[$(($RANDOM%10))]}${alpha3[$(($RANDOM%10))]}"

        local fromwhere=$(( $RANDOM % 3 + 1 ))
        [[ $fromwhere == 1 ]] && pass=$pass"${alpha1[$(($RANDOM%26))]}"
        [[ $fromwhere == 2 ]] && pass=$pass"${alpha2[$(($RANDOM%26))]}"
        [[ $fromwhere == 3 ]] && pass=$pass"${alpha3[$(($RANDOM%10))]}"


        for i in `seq 1 100` ;do
                local split=$(( $RANDOM % ${#pass} + 1 ))
                pass1=${pass:$split}
                pass2=${pass:0:$split}
                pass="${pass1}${pass2}"
        done

        echo $pass
}

function GeneratePasswordOf6Digits()
{
	local pass
	for ((i = 0; i < 6; i++)); do
		pass="${pass}$((RANDOM % 10))"
	done
	echo "$pass"
}

Log()
{
	local File="$1"; shift
	local Msg="$*"

	echo "LOG: $Msg"
	echo "$Msg" >>"$File"
}


function BlacklistConfFiles()
{
	local file=$1

	if [[ -e /etc/confblacklist ]] ;then
		while read line; do
			if [[ "$line" == "$file" ]] ;then
				return 0
			fi
		done < /etc/confblacklist
	fi

	return 1				
}






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
	echo "$(dpkg-query -l nvidia-glx*|grep "^ii"|awk '{print $2}')"
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

	echo "*************************************************"
	echo "       Begin NVidia driver installation          "
	echo "*************************************************"

	# first, lets see if there is even an nvidia card installed in the system
	# If there is no nVidia card even installed, lets get out of here.
	if ! getNvidiaInstalled;then
		echo "No nVidia card detected in the system. Exiting..."
		exit 1
	fi

	# Now lets get the currently installed nvidia driver (if there is one)
	current_driver=$(getInstalledNvidiaDriver)
	if [[ "$current_driver" -ne "" ]];then
		echo "Detected installed driver $current_driver"
	fi

	# Get the preferred driver to install
	preferred_driver=$(getPreferredNvidiaDriver)
	echo "LMCE prefers driver $preferred_driver"

	# Install the driver if needed
	if [[ "$current_driver" -ne "$preferred_driver" ]]; then
		echo "installing NEW driver $preferred_driver!"
		apt-get install $preferred_driver
		if [[ "$param" == "reboot" ]];then
			# Give the user a warning message and beep, then reboot
			echo ""
			echo -e "\e[1;31mNvidia driver installation requires a reboot.\e[0m"
			echo -e "\e[1;31mPlease stand by while your system is rebooted.\e[0m"
			echo ""
			beep -l 100 -f 500 -d 50 -r 3
			sleep 5
			reboot
		fi
	else
		echo "Preferred driver already installed"
	fi

	echo "*************************************************"
	echo "       End NVidia driver installation          "
	echo "*************************************************"
}
#######################################################################################################################
# END NVIDIA DRIVER HELPER FUNCTIONS
#######################################################################################################################


