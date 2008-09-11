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
	case "$VideoDriver" in
		nv) PackageIsInstalled nvidia-glx && VideoDriver="nvidia" ;;
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
