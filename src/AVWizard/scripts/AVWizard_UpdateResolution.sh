#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh

Param="$1"
case "$Param" in
	reset)
		WizResolution=$(< /tmp/avwizard-resolution.txt)
		VideoResolution=$(SpcField 1 "$WizResolution")
		VideoRefresh=$(SpcField 2 "$WizResolution")
		VideoOutput=$(SpcField 5 "$WizResolution")
		TVStandard=$(SpcField 6 "$WizResolution")

		ResolutionDefaults="1024x768 60 1024 768 VGA"
		if [[ -f "/tmp/avwizard-resolution-defaults.txt" ]]; then
			ResolutionDefaults="$(</tmp/avwizard-resolution-defaults.txt)"
		fi

		DefaultResolution=$(SpcField 1 "$ResolutionDefaults")
		DefaultRefresh=$(SpcField 2 "$ResolutionDefaults")
		DefaultConnector=$(SpcField 5 "$ResolutionDefaults")
		DefaultTVStandard=$(SpcField 6 "$ResolutionDefaults")

		DefaultResolutionFullName=$(Resolution_GetFullName "$DefaultResolution")
		if [[ "$DefaultResolutionFullName" == *=* ]]; then
			DefaultResolution_Name="${DefaultResolutionFullName%%=*}"
			DefaultResolution_Size="${DefaultResolutionFullName#*=}"
		else
			DefaultResolution_Name="$DefaultResolutionFullName"
			DefaultResolution_Size="$DefaultResolutionFullName"
		fi

		echo "--> Reset resolution to default: $DefaultResolution@$DefaultRefresh/$DefaultConnector,$DefaultTVStandard (was resolution: $VideoResolution@$VideoRefresh/$VideoOutput,$TVStandard)"
		if [[ "$VideoRefresh" == "$DefaultRefresh" && "$VideoResolution" == "$DefaultResolution" && "$VideoOutput" == "$DefaultConnector" && "$TVStandard" == "$DefaultTVStandard" ]]; then
			exit 0 # No change required
		fi
		killall -USR2 AVWizard
		bash -x "$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --resolution "$DefaultResolution_Size" --output "$DefaultConnector" --tv-standard "$DefaultTVStandard" | tee-pluto /var/log/pluto/Xconfigure.log

		sed -ri '/Option.*"Composite"/d' "$XF86Config"
		WMTweaksFile="/root/.config/xfce4/xfconf/xfce-perchannel-xml/xfwm4.xml"
		cp "$WMTweaksFile"{,.orig}
		sed -i '/Xfwm\/UseCompositing/ s/value="."/value="1"/g' "$WMTweaksFile"
		
		echo "$ResolutionDefaults" >/tmp/avwizard-resolution.txt
	;;
	set_resolution)
		set -x
		RequestedResolution="$2"
		RequestedRefresh="$3"
		RequestedConnector="${4:-VGA}"
		RequestedTVStandard="$5"
		echo "--> Setting resolution: $RequestedResolution@$RequestedRefresh (Connector: $RequestedConnector, TVStandard: $RequestedTVStandard)"
		ResolutionFullName=$(Resolution_GetFullName "$RequestedResolution")

		if [[ "$ResolutionFullName" == *=* ]]; then
			VideoResolution_Name="${ResolutionFullName%%=*}"
			VideoResolution_Size="${ResolutionFullName#*=}"
		else
			VideoResolution_Name="$ResolutionFullName"
			VideoResolution_Size="$ResolutionFullName"
		fi

		killall -USR1 AVWizard

		bash -x "$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --resolution "$VideoResolution_Size@$RequestedRefresh" --output "$RequestedConnector" --tv-standard "$RequestedTVStandard" | tee-pluto /var/log/pluto/Xconfigure.log

		sed -ri '/Option.*"Composite"/d' "$XF86Config"
		WMTweaksFile="/root/.config/xfce4/mcs_settings/wmtweaks.xml"
		cp "$WMTweaksFile"{,.orig}
		sed -i '/Xfwm\/UseCompositing/ s/value="."/value="1"/g' "$WMTweaksFile"
		
		WindowWidth="${VideoResolution_Size%x*}"
		WindowHeight="${VideoResolution_Size#*x}"
		echo "$VideoResolution_Name $RequestedRefresh $WindowWidth $WindowHeight $RequestedConnector $RequestedTVStandard" >/tmp/avwizard-resolution.txt
		echo "--> Finished setting resolution"
		set +x
	;;
	*) echo "Unknown parameter: '$Param'"; exit 1 ;;
esac
