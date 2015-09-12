#!/bin/bash

function XorgConfLogging() {
	local message="$1"
	local xorgLog="${ConfigFile}.log"
	local xorgLines=$(cat ${ConfigFile} | wc -l)
	local myPid=$$

	echo "$myPid $(date -R) $message [$xorgLines]"	>> $xorgLog
}

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/X-Common.sh

ConfigFile="/etc/X11/xorg.conf"
Output="VGA"
vga_pci=$(lspci | grep ' VGA ')
gpus=$(echo "$vga_pci" | wc -l)
bus_id=$(echo "$vga_pci" | awk 'NR==2' | while IFS=':. ' read -r tok1 tok2 tok3 rest; do printf '%2s %2s %s\n' "$((16#$tok1))":"$((16#$tok2))":"$((16#$tok3))" | sed -e 's/ //g'; done)

DEVICECATEGORY_Video_Cards=125
DEVICEDATA_Setup_Script=189
DEVICEDATA_Connector=68
DEVICEDATA_TV_Standard=229

#nVidia modeline, driver bug workaround
Modeline_640x480_60='"640x480" 25.18 640 656 752 800 480 490 492 525'

GetResolutionFromDB()
{
	. /usr/pluto/bin/SQL_Ops.sh
	DEVICECATEGORY_Media_Director=8
	DEVICEDATA_Video_settings=89

	ComputerDev=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Media_Director" '' 'include-parent')

	Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$ComputerDev' AND FK_DeviceData='$DEVICEDATA_Video_settings' LIMIT 1"
	VideoSetting=$(RunSQL "$Q")
	VideoSetting=$(Field "1" "$VideoSetting")

	ScanType=
	if [[ -n "$VideoSetting" ]]; then
		Refresh=$(echo $VideoSetting | cut -d '/' -f2)
		ResolutionInfo=$(echo $VideoSetting | cut -d '/' -f1)
		ResX=$(echo $ResolutionInfo | cut -d' ' -f1)
		ResY=$(echo $ResolutionInfo | cut -d' ' -f2)
		ResScanType=$(echo $ResolutionInfo | cut -d' ' -f3)
		if [[ -z "$Refresh" || -z "$ResX" || -z "$ResY" ]]; then
			Logging "$TYPE" "$SEVERITY_CRITICAL" "Xconfigure" "Malformed DeviceData: VideoSetting='$VideoSetting'"
			MalformedVideoSetting='Malformed VideoSetting'
		fi
		if [[ "$ResScanType" == i ]]; then
			ScanType=interlace
		fi
	fi

	# it happened to have malformed video settings, at least once
	if [[ -n "$MalformedVideoSetting" || -z "$VideoSetting" ]]; then
		ResX='640'
		ResY='480'
		Refresh='60'
		ScanType=
	fi
	
	Resolution="${ResX}x${ResY}"

	Output=$(GetDeviceData "$ComputerDev" "$DEVICEDATA_Connector")
	TVStandard=$(GetDeviceData "$ComputerDev" "$DEVICEDATA_TV_Standard")
}

ParseResolution()
{
	ResolutionSet=y
	Resolution="$1"
	if [[ "$Resolution" == *@* ]]; then
		Refresh=${Resolution##*@}
		Resolution=${Resolution%@*}
	else
		Refresh=60
	fi
	
	ResX=${Resolution%%x*}
	ResY=${Resolution#*x}
	if [[ "$ResY" == *i ]]; then
		ResY="${ResY%i}"
		Resolution="${Resolution%i}"
		ScanType="interlace"
	elif [[ "$ResY" == *p ]]; then
		ResY="${ResY%p}"
		Resolution="${Resolution%p}"
		ScanType=
	fi
	
	if [[ -z "$ResX" || -z "$ResY" || -z "$Refresh" || "$ResX" == *[![:digit:]]* || "$ResY" == *[![:digit:]]* || "$Refresh" == *[![:digit:]]* ]]; then
		echo "Mismatched resolution '$Resolution@$Refresh'. Use format: <ResX>x<ResY>@<Refresh>"
		exit 1
	fi
}

SetResolution()
{
	local Resolution="$1" ResX="$2" ResY="$3" Refresh="$4" ScanType="$5" Output="$6" TVStandard="$7"
	
	local ConnectedMonitor
	case "$Output" in
		VGA)
			ConnectedMonitor="CRT"
		;;
		DVI)
			ConnectedMonitor="DFP"
		;;
		Component)
			ConnectedMonitor="TV"
			TVOutFormat="Component"
		;;
		Composite)
			ConnectedMonitor="TV"
			TVOutFormat="Composite"
		;;
		S-Video)
			ConnectedMonitor="TV"
			TVOutFormat="SVideo"
		;;
	esac

	Modeline="$(GenModeline "$DisplayDriver" "$ResX" "$ResY" "$Refresh" "$ScanType")"
	

	awk -v"ResX=$ResX" -v"ResY=$ResY" -v"Refresh=$Refresh" -v"Modeline=$Modeline" -v"nvHD=$TVStandard" -v"ConnectedMonitor=$ConnectedMonitor" -v"TVOutFormat=$TVOutFormat" -f/usr/pluto/bin/X-ChangeResolution.awk "$ConfigFile" >"$ConfigFile.$$"
	mv "$ConfigFile"{.$$,}

	# TODO: Use python-Xkit for all xorg.conf manipulations
	# 
#	if [ -f /usr/pluto/bin/X-ChangeResolution.py ] ; then
#		python /usr/pluto/bin/X-ChangeResolution.py $ResX $ResY $ConfigFile
#	fi
}

XvMC_Lib()
{
	local libXvMC
	case "$DisplayDriver" in
		nvidia)
			libXvMC="libXvMCNVIDIA_dynamic.so.1"
	#		XineVideoDriver="xxmc"
		;;
	#	via)
	#		libXvMC="libviaXvMC.so.1"
	#		XineVideoDriver="xxmc"
	#	;;
		*)
			libXvMC="libXvMC.so.1"
	#		XineVideoDriver="xv"
		;;
	esac

	echo "$libXvMC"
}

CheckComponents()
{
	local -a Files=(
		/usr/pluto/bin/X-ChangeDisplayDriver.awk
		/usr/pluto/bin/X-GetDisplayDriver.awk
		/usr/pluto/bin/X-ModulesSection.awk
		/usr/pluto/bin/X-UI_Sections.awk
	)
	local File
	
	for File in "${Files[@]}"; do
		if [[ ! -f "$File" ]]; then
			Logging "$TYPE" "$SEVERITY_CRITICAL" "Xconfigure" "File not found: /usr/pluto/bin/X-ChangeDisplayDriver.awk."
			exit 1
		fi
	done
}

UpdateModules()
{
	/usr/pluto/bin/X-UpdateModules.sh --conffile "$ConfigFile" --driver "$DisplayDriver"
}

UpdateUISections()
{
	local OpenGL="$1" AlphaBlending="$2"
		awk -v"OpenGL=$OpenGL" -v"AlphaBlending=$AlphaBlending" -f/usr/pluto/bin/X-UI_Sections.awk "$ConfigFile" >"$ConfigFile.$$"
		mv "$ConfigFile"{.$$,}
}

ScriptCustomization()
{
	local Device_Video_Card
	local DeviceData
	
	Device_Video_Card=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Video_Cards" "no-recursion")
	if [[ -z "$Device_Video_Card" ]]; then
		return
	fi

	DeviceData=$(GetDeviceData "$Device_Video_Card" "$DEVICEDATA_Setup_Script")
	if [[ -z "$DeviceData" ]]; then
		return
	fi

	local ScriptPath="/usr/pluto/bin/$DeviceData"
	if [[ ! -f "$ScriptPath" ]]; then
		Logging "$TYPE" "$SEVERITY_CRITICAL" "Xconfigure" "Custom setup script not found: $DeviceData."
	fi

	. "$ScriptPath"
	GetCustomModeLines
}

ParseParameters()
{
	OrigParams=("$@")
	while [[ $# -gt 0 ]]; do
		case "$1" in
			--defaults) Defaults=y ;;
			--resolution)
				ParseResolution "$2"
				shift
			;;
			--scantype)
				case "$2" in
					progressive) ;;
					interlace|doublescan) ScanType="$2"; shift ;;
					*) echo "Unknown scan type '$2'"; exit 1 ;;
				esac
			;;
			--force) : ;; # obsolete
			--update-video-driver) UpdateVideoDriver=UpdateVideoDriver ;;
			--keep-resolution) KeepCurrentResolution=KeepCurrentResolution ;;
			--force-vesa) ForceVESA=ForceVESA ;;
			--conffile) ConfigFile="$2"; shift ;;
			--skiplock) SkipLock=SkipLock ;;
			--output) Output="$2" ; shift ;; # VGA, DVI, Component, Composite, S-Video
			--tv-standard) TVStandard="$2"; shift ;; # PAL, SECAM, NTSC, HD480p, HD720p, HD1080i, HD1080p
			--no-test) NoTest=NoTest ;;
			*) echo "Unknown option '$1'"; exit 1 ;;
		esac
		shift
	done
}

EnsureResolutionVariables()
{
	if [[ ! -f "$ConfigFile" || ! -s "$ConfigFile" ]]; then
		# TODO: Detect incomplete/corrupt config files too
		Logging "$TYPE" "$SEVERITY_WARNING" "Xconfigure" "Config file '$ConfigFile' not found or empty. Forcing use of defaults."
		Defaults=y
	fi

	if [[ -z "$ResolutionSet" ]]; then
		GetResolutionFromDB
	fi

	if [[ -z "$Defaults" && -n "$KeepCurrentResolution" ]]; then
		ResX=
		ResY=
		Refresh=
		Resolution=
		ScanType=
	fi
}

wmtweaks_default()
{
 echo '<?xml version="1.0" encoding="UTF-8"?>

<channel name="xfwm4" version="1.0">

    <property name="general" type="empty">

        <property name="use_compositing" type="bool" value="true"/>

    </property>

</channel>'
}

SetWMCompositor()
{
	local UseCompositing="$1"
	local WMTweaksFile

	local -a HomeDirs=(/root)

	local Ent
	local OldIFS="$IFS"
	IFS=":"
	local Username Pwd Uid Gid Comment HomeDir Shell
	while read Username Pwd Uid Gid Comment HomeDir Shell; do
		if [[ "$Uid" -ge 1000 && -d "$HomeDir" && "$HomeDir" != / ]]; then
			HomeDirs=("${HomeDirs[@]}" "$HomeDir")
		fi
	done < <(getent passwd | egrep ':/bin/bash$')
	IFS="$OldIFS"

	local user
	for user in "${HomeDirs[@]}"; do
		WMTweaksFile="$user/.config/xfce4/xfconf/xfce-perchannel-xml/xfwm4.xml"
		mkdir -p "$(dirname "$WMTweaksFile")"
		wmtweaks_default >"$WMTweaksFile"
		sed -i '/Xfwm\/UseCompositing/ s/value="."/value="'"$UseCompositing"'"/g' "$WMTweaksFile"
	done
}

rm -f /etc/pluto/X-PostStart.sh

ScriptCustomization
CheckComponents
ParseParameters "$@"

XorgConfLogging "Starting $0 $*"

if [[ -z "$SkipLock" ]]; then
	trap 'Unlock "Xconfigure" "Xconfigure" nolog; XorgConfLogging "Ending"' EXIT
	WaitLock "Xconfigure" "Xconfigure" nolog # don't run two copies of Xconfigure simultaneously
fi
EnsureResolutionVariables

DisplayDriver=$(GetVideoDriver)
if [[ -n "$ForceVESA" ]]; then
	DisplayDriver="fbdev"
fi
if [[ "$DisplayDriver" == "intel" ]]; then
	. /usr/pluto/bin/X-IntelSpecificFunctions.sh
elif [[ "$DisplayDriver" == "nvidia" ]]; then
	. /usr/pluto/bin/X-nVidiaSpecificFunctions.sh
#elif [[ "$DisplayDriver" == fglrx ]]; then
#	. /usr/pluto/bin/X-ATISpecificFunction.sh
fi

Logging "$TYPE" "$SEVERITY_STATUS" "Xconfigure" "Display Driver: $DisplayDriver" >&2
XorgConfLogging "Display Driver: $DisplayDriver"

if [[ -z "$DisplayDriver" ]]; then
	XorgConfLogging "Empty display driver. Will not continue. This shouldn't happen"
	exit 1
fi

CurrentDisplayDriver=$(awk -f/usr/pluto/bin/X-GetDisplayDriver.awk "$ConfigFile")
if [[ "$Defaults" == y ]]; then
	# write xorg.conf from scratch using the defaults
	if [[ ! -f /usr/pluto/templates/xorg.conf.in ]]; then
		Logging "$TYPE" "$SEVERITY_CRITICAL" "Xconfigure" "File not found: /usr/pluto/templates/xorg.conf.in. Can't setup defaults"
		exit 1
	fi
	cat /usr/pluto/templates/xorg.conf.in | awk -v"DisplayDriver=$DisplayDriver" -f/usr/pluto/bin/X-ChangeDisplayDriver.awk >"$ConfigFile"
	if [[ "$gpus" -gt "1" ]]; then
		sed -ie "s/#BusID.*/BusID\t\t\"PCI:${bus_id}\"/g" "$ConfigFile"
	fi
elif [[ -n "$UpdateVideoDriver" && "$DisplayDriver" != "$CurrentDisplayDriver" ]]; then
	# change video driver, but only if needed
	awk -v"DisplayDriver=$DisplayDriver" -f/usr/pluto/bin/X-ChangeDisplayDriver.awk "$ConfigFile" >"$ConfigFile.$$"
	mv "$ConfigFile"{.$$,}
	if [[ "$gpus" -gt "1" ]]; then
		sed -ie "s/#BusID.*/BusID\t\t\"PCI:${bus_id}\"/g" "$ConfigFile.$$"
	fi
fi

UpdateModules
OpenGL=$(OpenGLeffects)
AlphaBlending=$(AlphaBlendingEnabled)
UpdateUISections "$OpenGL" "$AlphaBlending"
SetWMCompositor "$AlphaBlending"
libXvMC=$(XvMC_Lib)

if ! BlacklistConfFiles "/etc/X11/XvMCConfig" ;then
	if [ ! -e /etc/X11/XvMCConfig.pbackup ] && [ -e /etc/X11/XvMCConfig ] ;then
		cp /etc/X11/XvMCConfig /etc/X11/XvMCConfig.pbackup
	fi

	echo "$libXvMC" >/etc/X11/XvMCConfig
fi
#XineConfSet video.driver "$XineVideoDriver"

# Setup resolution
if [[ -n "$Resolution" ]]; then
	SetResolution "$Resolution" "$ResX" "$ResY" "$Refresh" "$ScanType" "$Output" "$TVStandard"
fi

# Test detected display driver
# Don't test if driver is vesa (assumption: always works) or current driver (assumption: already tested and works)
# Doing this test last, to test all the changes, since the video driver (e.g. nvidia) may not work without some extra options that are set together with the resolution
if [[ "$DisplayDriver" != "$CurrentDisplayDriver" && "$DisplayDriver" != "fbdev" ]] && [[ -n "$Defaults" || -n "$UpdateVideoDriver" ]]; then
	if [[ -z "$NoTest" ]] && ! TestXConfig "$Display" "$ConfigFile" && [[ -z "$ForceVESA" ]]; then
		"0" "${OrigParams[@]}" --force-vesa --skiplock
		exit $?
	fi
fi
# ^^ 1004 testing - bad assumption that VESA always works due to dri module load which loads dynamically for VESA modules, dri severly kills the X session for AVWizard 
# on nvidia cards - these lines break due to install from pluto-nvidia-video-drivers
