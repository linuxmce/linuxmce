#!/bin/bash
. /usr/pluto/bin/SQL_Ops.sh

DT_TTS=57
DD_DEFAULT_VOICE=283

#Remove the old nitech voices and db setting.
Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate='$DT_TTS'"
R=$(RunSQL "$Q")
DEVICE_TTS=$(Field 1 "$R")

Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$DEVICE_TTS' AND FK_DeviceData='$DD_DEFAULT_VOICE'"
R=$(RunSQL "$Q")
DEFAULT_VOICE=$(Field 1 "$R")

case "$DEFAULT_VOICE" in
	*nitech*)
		# if the cmu voice is installed use it
		if [ -d "/usr/share/festival/voices/us/cmu_us_slt_arctic_hts" ]; then
			VOICE='voice_cmu_us_slt_arctic_hts'
		else
			VOICE=''
		fi
		Q="UPDATE Device_DeviceData SET IK_DeviceData='$VOICE' WHERE FK_Device='$DEVICE_TTS' AND FK_DeviceData='$DD_DEFAULT_VOICE'"
		R=$(RunSQL "$Q")
	;;
esac

#Check to see if the CMU voice is installed
if [ -d "/usr/share/festival/voices/us/cmu_us_slt_arctic_hts" ] ; then
	echo "CMU HTS festival voice already installed, no action required."
	exit 0
fi

echo "Downloading and installing the CMU HTS festival voice..."
cd /tmp/
mkdir -p hts_tmp
cd hts_tmp/
#Download the voices
wget -c http://www.linuxmce.org/festvox_cmu_us_slt_arctic_hts.tar.gz

#Extract the voices
for t in $(ls); do
	tar xvf $t
done

#Install the voices
mkdir -p /usr/share/festival/voices/us
mv festival/lib/voices/us/* /usr/share/festival/voices/us

#Clean Up
cd /tmp/
rm -rf hts_tmp/

Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$DEVICE_TTS' AND FK_DeviceData='$DD_DEFAULT_VOICE'"
R=$(RunSQL "$Q")
DEFAULT_VOICE=$(Field 1 "$R")
if [[ -z "$DEFAULT_VOICE" ]]; then
	Q="UPDATE Device_DeviceData SET IK_DeviceData='voice_cmu_us_slt_arctic_hts' WHERE FK_Device='$DEVICE_TTS' AND FK_DeviceData='$DD_DEFAULT_VOICE'"
fi
echo "Finished installing CMU HTS voices."

mkdir -p /var/lib/asterisk/sounds/pluto/
