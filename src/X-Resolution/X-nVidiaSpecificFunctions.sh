#!/bin/bash

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

	(
		echo "#!/bin/bash"
		echo 'nvidia-settings -a "SyncToVBlank=1"'
		echo 'nvidia-settings -a "XVideoTextureSyncToVBlank=1"'
		echo 'nvidia-settings -a "XVideoBlitterSyncToVBlank=1"'
		echo "xrandr -r '$Refresh'"
	) >/etc/pluto/X-PostStart.sh
	chmod +x /etc/pluto/X-PostStart.sh
}
