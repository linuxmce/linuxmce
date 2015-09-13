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

TranslateSerialPort()
{
	local SearchFor="$1"
	local PCI USB

	SerialPort=
	if [[ "$SearchFor" == pci* || "$SearchFor" == platform* ]]; then
		if [[ -d /sys/class/tty ]]; then
			pushd /sys/class/tty &>/dev/null
			for dev in ttyUSB*/device ttyACM*/device; do
				id=$(readlink -f "$dev" | sed -r 's,^.*(pci.*|platform.*)/usb[0-9]*/[0-9./-]*/[0-9]*-([0-9.]*):[0-9.]*(/ttyUSB[0-9]*)?$,\1+\2,g')
				if [[ "$id" == "$SearchFor" ]]; then
					SerialPort="/dev/$(dirname "$dev")"
					break;
				fi
			done
			popd &>/dev/null
		fi
	else
		SerialPort="$SearchFor"
	fi

	builtin echo "$SerialPort"
}

TranslateSoundCard()
{
	local SoundCard="$1" AlsaCard=
	local PCI USB
	local Cards Card Dev

	if [[ "$SoundCard" == *';'* ]]; then
		SoundCard="${SoundCard#*;}"
	fi

	if [[ -d /sys/class/sound && "$SoundCard" == pci* ]]; then
		if [[ "$SoundCard" == *+* ]]; then
			PCI="${SoundCard%+*}"
			USB="${SoundCard#*+}"
		else
			PCI="${SoundCard}"
			USB=
		fi
		pushd /sys/class/sound &>/dev/null
		Cards=$(find -name 'controlC*')
		for Card in $Cards; do
			Card="${Card#./}"
			Dev=$(readlink -f "$Card/device" | sed -r 's,/sound/.*$,,g')
			if [[ -n "$USB" ]]; then
				if [[ "$Dev" == *"$PCI"*usb*"$USB:"* ]]; then
					AlsaCard="${Card#controlC}"
					break
				fi
			else
				if [[ "$Dev" == *"$PCI" ]]; then
					AlsaCard="${Card#controlC}"
					break
				fi
			fi
		done
		popd &>/dev/null
	fi

	builtin echo "$AlsaCard"
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


GetDevice_Template()
{
	local PK_Device="$1"

	local R=$(RunSQL "SELECT FK_DeviceTemplate FROM Device WHERE PK_Device=$PK_Device")

	if [[ "$R" == NULL ]]; then
		R=""
	fi
	echo "$R"
}

GetDevice_Category()
{
	local PK_Device="$1"

	local R=$(RunSQL "SELECT FK_DeviceCategory FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE PK_Device=$PK_Device")

	if [[ "$R" == NULL ]]; then
		R=""
	fi
	echo "$R"
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
			WHERE (FK_Device_ControlledVia='$PK_Device_Parent' OR PK_Device='$PK_Device_Parent') AND FK_DeviceTemplate IN ($FK_DeviceTemplate)
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

# Colors and bolds messages
# '\E begin escape sequence
# [XX; is the text color
# XXm' is the background color
# \033 is the an escape
# [1m bold	[0m unbold
# "" around text	'' around color
# COLOR		FOREGROUND	BACKGROUND
# black		30		40
# red		31		41
# green		32		42
# yellow	33		43
# blue		34		44
# magenta	35		45
# cyan		36		46
# white		37		47

StatusMessage() {
	echo -e '\E[33;40m'"\033[1m  $* \033[0m"
}
ErrorMessage() {
	echo -e '\E[33;41m'"\033[1m  $* \033[0m"
}
NotifyMessage() {
	echo -e '\E[32;40m'"\033[1m  $* \033[0m"
}

confirmRoot() {
	testroot="`whoami`"
	if [ "$testroot" != "root" ]; then
		ErrorMessage "Need to be root to run. Exiting"
		exit 1
	fi
}

StartService() {
        ServiceDescription=$1
        ServiceCmd=$2
        ServiceBkg=$3

        if [[ -x $(echo $ServiceCmd | cut -d ' ' -f1) ]] ;then
                #if [[ -x /bin/plymouth ]]; then
                #       /bin/plymouth update --status="$ServiceDescription ... " || :
                #fi
                #echo -n "$ServiceDescription ... "

                log_daemon_msg "$ServiceDescription" "$ServiceCmd"
                if [[ "$ServiceBkg" == "&" ]] ;then
                        $ServiceCmd 1>/dev/null 2>/dev/null &
                else
                        $ServiceCmd 1>/dev/null 2>/dev/null
                fi
                err=$?
                log_end_msg $err

                #if [[ "$err" == "0" ]] ;then
                #       echo "ok"
                #else
                #       echo "fail"
                #fi
        fi

        return $err
}

VerifyExitCode() {
	local EXITCODE=$?
	if [ "$EXITCODE" != "0" ] ; then
		echo "An error (Exit code $EXITCODE) occured during the last action"
		echo "$1"
		exit 1
	fi
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

GeneratePassword() {
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

GeneratePasswordOf6Digits()
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


BlacklistConfFiles()
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

VDRInstalled() {
	# Return true if VDR is installed
	DEVICETEMPLATE_VDR_Plugin="1704"
	Q="SELECT PK_Device FROM Device Where FK_DeviceTemplate = $DEVICETEMPLATE_VDR_Plugin"
	VDRDevice=$(RunSQL "$Q")
	if [ "$VDRDevice" == "" ] ; then
		RETURNVALUE=1
	else
		RETURNVALUE=0
	fi
	return $RETURNVALUE
}


#####################################################################
# GPU/Driver fns
#####################################################################

FindVideoDriver() {
	#####################################################################
	# Switching our default to fbdev for interoperability
	# with KVM & nVidia no-test in AVWizard_Run.sh
	#####################################################################
	local vga_info="$1"
	local prop_driver="fbdev"

	# 1002=ATI, 1106=VIA, 10de=nVidia, 8086=Intel 1013=cirrus 80ee=VirtualBox
	chip_man=$(echo "$vga_info" | grep -Ewo '(\[1002|\[1106|\[10de|\[8086|\[1013|\[80ee)')

	case "$chip_man" in
		*10de)
			prop_driver="nvidia" ;;
		*1002)
			prop_driver="fglrx"
			if grep -Ei '((R.)([2-5])|(9|X|ES)(1|2?)([0-9])(5|0)0|Xpress)' <<< "$vga_info"; then
				prop_driver="radeon"
			fi
			if grep -Ei '(mach)' <<< "$vga_info"; then
				prop_driver="mach64"
			fi ;;

		*8086)
			prop_driver="intel"
			if grep "i740" <<< "$vga_info"; then
				prop_driver="i740"
			fi
			if grep "i128" <<< "$vga_info"; then
				prop_driver="i128"
			fi
			if ! PackageIsInstalled "i965-va-driver"; then
				apt-get -yf install i965-va-driver
				VerifyExitCode "Install Intel Graphics Accelerator"
			fi ;;

		*1106)
			prop_driver="openchrome" ;
			if grep -i "Savage" <<< "$vga_info"; then
				prop_driver="savage"
			fi
			#if echo "$vga_info"| grep -i "s3"; then
				#prop_driver="via"; fi
			if grep -i "virge" <<< "$vga_info"; then
				prop_driver="virge"
			fi ;;
		*1013)
			prop_driver="cirrus" ;;
		*80ee)
			prop_driver="vboxvideo" ;;
		*)
			prop_driver="fbdev" ;;
	esac

	echo "$prop_driver"
}

DriverRank() {
	# We figure out, based on the driver, which is our favored GPU. 10 being most favored. This list reflects the liklihood of combinations
	# more than a head to head competition.
	local vga_info="$1"
	local driver_rank="1"
	local prop_driver=$(FindVideoDriver "$vga_info")
	case "$prop_driver" in
		cirrus) driver_rank="2" ;;
		vboxvideo) driver_rank="3" ;;
		i740|i128|mach64) driver_rank="4" ;;
		radeon) driver_rank="5" ;;
		intel) driver_rank="7" ;;
		fglrx) driver_rank="8" ;;
		nvidia)
			. /usr/pluto/bin/nvidia-install.sh
			current_nvidia=$(getInstalledNvidiaDriver)
			preferred_nvidia=$(getPreferredNvidiaDriver)
			case "$preferred_nvidia" in
				nvidia-173) driver_rank="6" ;;
				nvidia-current) driver_rank="10" ;;
			esac
	esac

	echo "$driver_rank"
}

CardDetail() {
	local vga_pci="$1"
	local card_detail=$(echo "$vga_pci" | cut -d':' -f3-)

	echo "$card_detail"
}

BestGPU() {
	local vga_pci=$(lspci -nn | grep -w 'VGA')

	# If there are more than one GPU determine the best to use.
	if [[ $(wc -l <<< "$vga_pci") -gt "1" ]]; then
		vga_1=$(echo "$vga_pci" | head -1)
		vga_2=$(echo "$vga_pci" | awk 'NR==2')

		# Run first GPU through the check
		driver_1=$(FindVideoDriver "$vga_1")
		rank_1=$(DriverRank "$driver_1")

		# Run second GPU through the gauntlet
		driver_2=$(FindVideoDriver "$vga_2")
		rank_2=$(DriverRank "$driver_2")

		# Choose the Highest number and complete
		if [[ "$rank_1" -gt "$rank_2" ]]; then
			vga_pci="$vga_1"
		else
			vga_pci="$vga_2"
		fi
	fi

	echo "$vga_pci"
}

InstallVideoDriver() {
	local prop_driver="$1"

	case "$prop_driver" in

		# nVidia cards
		nvidia)
			if ! PackageIsInstalled nvidia-173 && ! PackageIsInstalled nvidia-current ; then
				VerifyExitCode "Install Pluto nVidia Driver"
				nv_pid=$(pidof nvidia-install.sh)
				if [[ -n $nv_pid ]] ; then
					StatusMessage "Installing nVidia driver this may take a few minutes"
					installCorrectNvidiaDriver
				else StartService "Installing nVidia driver this may take a few minutes" ". /usr/pluto/bin/nvidia-install.sh"
					installCorrectNvidiaDriver
				fi
				ConfSet "AVWizardOverride" "1"
			fi ;;
		nouveau)
			if ! PackageIsInstalled xserver-xorg-video-nouveau"$LTS_HES" ; then
				apt-get -yf install xserver-xorg-video-nouveau"$LTS_HES"
				VerifyExitCode "Install nouveau Driver"
			fi ;;

		# ATI cards
		radeon)
			if ! PackageIsInstalled xserver-xorg-video-radeon"$LTS_HES" ; then
				apt-get -yf install xserver-xorg-video-radeon"$LTS_HES"
				VerifyExitCode "Install radeon Driver"
			fi ;;
		fglrx)
			if ! PackageIsInstalled fglrx ; then
				apt-get -yf install fglrx
				VerifyExitCode "Install fglrx Driver"
				if -f /etc/X11/xorg.conf ; then
					a=1
						for i in xorg.conf; do
						new=$(printf "fglrx.xorg.backup%03d" ${a})
						cp /etc/X11/xorg.conf ${new}
						let a=a+1
					done

				fi
				ConfSet "AVWizardOverride" "1"
				reboot
			fi ;;
		mach64)
			if ! PackageIsInstalled xserver-xorg-video-mach64"$LTS_HES" ; then
				apt-get -yf install xserver-xorg-video-mach64"$LTS_HES"
				VerifyExitCode "Install mach64 Driver"
				ConfSet "AVWizardOverride" "1"
			fi ;;

		# Intel cards
		intel)
			if ! PackageIsInstalled xserver-xorg-video-intel"$LTS_HES" ; then
				apt-get -yf install xserver-xorg-video-intel"$LTS_HES"
				VerifyExitCode "Install Intel Driver"
			fi ;;
		i128)
			if ! PackageIsInstalled xserver-xorg-video-i128"$LTS_HES" ; then
				apt-get -yf install xserver-xorg-video-i128"$LTS_HES"
				VerifyExitCode "Install i128 Driver"
			fi ;;
		i740)
			if ! PackageIsInstalled xserver-xorg-video-i740"$LTS_HES" ; then
				apt-get -yf install xserver-xorg-video-i740"$LTS_HES"
				VerifyExitCode "Install i740 Driver"
			fi ;;

		# VIA cards
		openchrome)
			if ! PackageIsInstalled xserver-xorg-video-openchrome"$LTS_HES" ; then
				apt-get -yf install xserver-xorg-video-openchrome"$LTS_HES"
				VerifyExitCode "Install opencrhome Driver"
			fi ;;
		savage)
			if ! PackageIsInstalled xserver-xorg-video-savage"$LTS_HES" ; then
				apt-get -yf install xserver-xorg-video-savage"$LTS_HES"
				VerifyExitCode "Install VIA Savage Driver"
				ConfSet "AVWizardOverride" "1"
			fi ;;
		via)
			if ! PackageIsInstalled xserver-xorg-video-via"$LTS_HES" ; then
				apt-get -yf install xserver-xorg-video-via"$LTS_HES"
				VerifyExitCode "Install VIA S3 Driver"
				ConfSet "AVWizardOverride" "1"
			fi ;;
		virge)
			if ! PackageIsInstalled xserver-xorg-video-s3virge"$LTS_HES" ; then
				apt-get -yf install xserver-xorg-video-s3virge"$LTS_HES"
				VerifyExitCode "Install VIA S3 Virge Driver"
				ConfSet "AVWizardOverride" "1"
			fi ;;

		# VMWare
		cirrus)
			if ! PackageIsInstalled xserver-xorg-video-cirrus"$LTS_HES" ; then
				apt-get -yf install xserver-xorg-video-cirrus"$LTS_HES"
				VerifyExitCode "Install Cirrus Driver"
			fi ;;

		# VirtualBox
		vboxvideo)
			if ! PackageIsInstalled virtualbox-guest-x11 ; then
				apt-get -yf install virtualbox-guest-dkms
				VerifyExitCode "Install virtualbox Driver"
			fi ;;
	esac
}

CheckVideoDriver() {
	local vga_pci=$(BestGPU)

	InstallRoutine "$vga_pci"
}

InstallRoutine() {
	local vga_pci="$1"
	local prop_driver=$(FindVideoDriver "$vga_pci")
	local card_detail=$(CardDetail "$vga_pci")
	# FIXME: is there a better way? a builtin somewhere?
	local online=$(ping -c 2 google.com)
	local offline_mismatch="false"
	local online_mismatch="false"

	if [[ -f /etc/X11/xorg.conf ]] && [[ $(wc -l <<< "$vga_pci") -lt "2" ]]; then
		# TODO figure out a better way to isolate the video driver in the xorg.conf list of "Driver" options
		cur_driver=$(grep "Driver" /etc/X11/xorg.conf | grep -Eo '(nvidia|nouveau|radeon|fglrx|savage|openchrome|via|virge|intel|i740|i128|mach64|cirrus|vboxvideo|fbdev)')
		if [[ "$prop_driver" != "$cur_driver" ]] && [[ -z $online ]]; then
			offline_mismatch="true"
		elif [[ "$prop_driver" != "$cur_driver" ]] && [[ -n $online ]]; then
			online_mismatch="true"
		fi

		if [[ "$prop_driver" == "$cur_driver" ]] && [[ "$cur_driver" == "nvidia" ]] && [[ -n "$online" ]]; then
			. /usr/pluto/bin/nvidia-install.sh
			current_nvidia=$(getInstalledNvidiaDriver)
			preferred_nvidia=$(getPreferredNvidiaDriver)
			if [[ "$current_nvidia" != "$preferred_nvidia" ]]; then
				online_mismatch="true"
			fi
		fi

		if [[ "$online_mismatch" == "false" ]] && [[ "$offline_mismatch" == "false" ]]; then
			Mismatch="false"
		else
			Mismatch="true"
		fi

		# Look at mismatches and handle appropriately
		if [[ "$cur_driver" == "$prop_driver" ]] && [[ "$Mismatch" == "false" ]]; then
			StatusMessage "Correct driver '$prop_driver' already loaded"
			return 0

		else
			# Remove fglrx via or nVidia drivers if they are installed, but do not match current requirements
			ErrorMessage "Video chipset change detected !!!"
			if [[ "$cur_driver" == "fglrx" ]]; then
				echo ""
				echo ""
				echo ""
				ErrorMessage "Purging fglrx driver due to multiple conflicts"
				apt-get -y remove --purge xorg-driver-fglrx fglrx* --force-yes
				apt-get -y install --reinstall libgl1-mesa-glx libgl1-mesa-dri fglrx-modaliases --force-yes
				dpkg-reconfigure xserver-xorg
				apt-get -y install --reinstall xserver-xorg-core --force-yes
				reboot
				exit 0
			elif [[ "$cur_driver" == "nvidia" ]]; then
				nv_pid=$(pidof nvidia-install.sh)
				if [[ -n $nv_pid ]] ; then
					StatusMessage "Installing nVidia driver this may take a few minutes"
					installCorrectNvidiaDriver
				else StartService "Installing nVidia driver this may take a few minutes" ". /usr/pluto/bin/nvidia-install.sh"
					installCorrectNvidiaDriver
				fi
				ConfSet "AVWizardOverride" "1"
				exit 0
			elif [[ "$cur_driver" == "via" ]]; then
				StatusMessage "Removing old VIA driver"
				apt-get -yf remove --purge xserver-xorg-video-via --force-yes
				InstallVideoDriver "$prop_driver"
				#reboot
				exit 0
			fi

			if [[ "$offline_mismatch" == "true" ]]; then
				case "$prop_driver" in
					nvidia)
						prop_driver="nouveau" ;;
					fglrx)
						prop_driver="radeon" ;;
					savage|via|virge)
						prop_driver="openchrome" ;;
				esac
			fi

			if [[ "$prop_driver" != "$cur_driver" ]]; then
				StatusMessage "Using video driver '$prop_driver' for $card_detail"
				InstallVideoDriver "$prop_driver"
			fi
		fi
	else
		# If there is no xorg.conf, install driver.
		if [[ -z "$online" ]]; then
			case "$prop_driver" in
				nvidia)
					prop_driver="nouveau" ;;
				fglrx)
					prop_driver="radeon" ;;
				savage|via|verge)
					prop_driver="openchrome" ;;
			esac
		fi
		StatusMessage "/etc/X11/xorg.conf is missing. Using video driver '$prop_driver' for $card_detail"
		InstallVideoDriver "$prop_driver"
	fi
	export Best_Video_Driver="$prop_driver"
}

GetVideoDriver() {
	if [[ -n "$ForceVESA" ]]; then
		echo fbdev
		return 0
	fi

	local vga_pci=$(BestGPU)
	local cur_driver=$(FindVideoDriver "$vga_pci")

	echo "$cur_driver"
}
