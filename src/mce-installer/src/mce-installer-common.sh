#!/bin/bash

DT_MEDIA_DIRECTOR=3
DT_CORE=1
DT_HYBRID=2

function Replace_Mirror {
	local mirror=$1
	local flag='0'
	if [[ "$mirror" == "(null)" ]] ;then
		echo "WARNING: Replace_Mirror called with (null) mirror"
	fi

	local newlines="
deb file:/usr/pluto/deb-cache/ ./
deb $mirror gutsy main restricted universe multiverse
deb $mirror gutsy-security main restricted universe multiverse
deb $mirror gutsy-updates  main restricted universe multiverse
"

	while read line ;do
		if [[ "$line" == '# Choosed mirror - start' ]] ;then
			flag='1'
			echo "# Choosed mirror - start
$newlines
# Choosed mirror - end" >> /etc/apt/sources.list.$$
		fi

		if [[ "$flag" == '0' ]] ;then
			echo "$line" >> /etc/apt/sources.list.$$
		fi
                
		if [[ "$line" == '# Choosed mirror - end' ]] ;then
			flag='0'
		fi

	done < /etc/apt/sources.list

	mv -f /etc/apt/sources.list.$$ /etc/apt/sources.list
}


function Setup_Apt_Conffiles {	
	## Backup initial apt sources.list
	if [ ! -e /etc/apt/sources.list.pbackup ] ;then
		cp /etc/apt/sources.list /etc/apt/sources.list.pbackup
	fi

	## Make sure we have an working mirror
	if [[ "$c_installMirror" == "(null)" ]] ;then
		c_installMirror="http://archive.ubuntu.com/ubuntu/"
	fi


	## Setup apt sources.list
	local Sources="# Pluto sources - start
# Choosed mirror - start
deb file:/usr/pluto/deb-cache/ ./
deb $c_installMirror gutsy main restricted multiverse universe
deb $c_installMirror gutsy-security main restricted multiverse universe
deb $c_installMirror gutsy-updates main restricted multiverse universe
deb http://linuxmce.com/ubuntu ./
# Choosed mirror - end
#deb http://10.0.0.82/ ./
# Pluto sources - end"

	local SourcesOffline="# Pluto sources offline - start
deb file:/usr/pluto/deb-cache/ ./
# Pluto sources offline - end"

	if ! grep -qF '# Pluto sources - start' /etc/apt/sources.list ;then
		echo "$Sources" > /etc/apt/sources.list.$$
		cat /etc/apt/sources.list.$$ /etc/apt/sources.list > /etc/apt/sources.list.$$.all
		mv -f /etc/apt/sources.list.$$.all /etc/apt/sources.list
		rm -f /etc/apt/sources.list.$$
	else
		Replace_Mirror "$c_installMirror"
	fi

	echo "$SourcesOffline" >/etc/apt/sources.list.offline

#	if [[ "$c_ubuntuExtraCdFrom" != "3" ]] && [[ ! -z "$c_ubuntuExtraCdFrom" ]] ;then
		#echo "Dir::Etc::sourcelist sources.list.offline;" >/etc/apt/apt.conf.d/00ubuntu_offline
		cp /etc/apt/sources.list /etc/apt/sources.list.original
		mv /etc/apt/sources.list.offline /etc/apt/sources.list
#	fi
	# Setup pluto's apt.conf
	pluto_apt_conf='// Pluto apt conf add-on
Apt::Cache-Limit "12582912";
Dpkg::Options { "--force-confold"; };
Acquire::http::timeout "10";
Acquire::ftp::timeout "10";
APT::Get::AllowUnauthenticated "true";
APT::Get::force-yes "yes";
'
	echo -n "$pluto_apt_conf" >/etc/apt/apt.conf.d/30pluto

	pluto_apt_preferences='
Package: *
Pin: origin
Pin-Priority: 9999

Package: *
Pin: release v=7.10,o=Ubuntu,a=gutsy,l=Ubuntu
Pin-Priority: 9998
'
	echo -n "$pluto_apt_preferences" >/etc/apt/preferences

	ln -s /usr/pluto/var/apt.conf.offline /etc/apt/apt.conf.d/99offline

	./mce-installer-preseed.sh
	apt-get update
}

function Setup_NIS {
	## Put a temporary nis config file that will prevent ypbind to start
	echo "
NISSERVER=false
NISCLIENT=false
YPPWDDIR=/etc
YPCHANGEOK=chsh
NISMASTER=
YPSERVARGS=
YPBINDARGS=
YPPASSWDDARGS=
YPXFRDARGS=
" > /etc/default/nis
}

function wmtweaks_default()
{
	echo '<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE mcs-option SYSTEM "mcs-option.dtd">

<mcs-option>
	<option name="Xfwm/UseCompositing" type="int" value="1"/>
</mcs-option>'
}

function Setup_XOrg {
	## Make X accessible by Pluto software, for all existing and new users
	KDExhost="#!/bin/bash
	xhost +
	"

	for user in /home/*; do
		if [[ ! -d "$user" ]]; then
			continue
		fi
		owner=$(stat -c '%u:%g' "$user")

		Dir="$user/.kde/Autostart"
		mkdir -p "$Dir"
		echo "$KDExhost" >"$Dir/xhost"
		chown "$owner" "$Dir/xhost"
		chmod +x "$Dir/xhost"

		## Configure window manager trasparancy manager
		WMTweaksFile="$user/.config/xfce4/mcs_settings/wmtweaks.xml"
		mkdir -p "$(dirname "$WMTweaksFile")"

		if [[ -f "$WMTweaksFile" ]]; then
			sed -i '/Xfwm\/UseCompositing/ s/value="."/value="1"/g' "$WMTweaksFile"
		else
			wmtweaks_default >"$WMTweaksFile"
		fi
		
		chown -R "$owner" "$user"/.kde
		chown -R "$owner" "$user"/.config
	done

	Dir="/etc/skel/.kde/Autostart"
	echo "$KDExhost" >"$Dir/xhost"
	chmod +x "$Dir/xhost"
	
	File="/etc/profile"
	if ! grep -q "export KDEWM=/usr/bin/xfwm4" "$File" 2>/dev/null; then
		echo "export KDEWM=/usr/bin/xfwm4" >>"$File"
	fi

	WMTweaksFile="/etc/skel/.config/xfce4/mcs_settings/wmtweaks.xml"
	mkdir -p "$(dirname "$WMTweaksFile")"
	wmtweaks_default >"$WMTweaksFile"
	
	## Add xrecord extention if missing
	if grep -q ".*Load.*\"record\"" /etc/X11/xorg.conf ;then
		echo "Found xrecord , skiping ..."
	else 
		sed -i 's/.*Section.*\"Module\"/Section "Module"\n Load "record"/g' /etc/X11/xorg.conf
	fi
}


function UI_SetOptions {
	local OrbiterDev="$1"
	local OpenGLeffects="$2"
	local AlphaBlending="$3"
	local UI_Version="$4"

	DEVICEDATA_Use_OpenGL_effects=172
	DEVICEDATA_Use_alpha_blended_UI=169
	DEVICEDATA_PK_UI=104

	# disable OpenGL effects
	Q="REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
	     VALUES ('$OrbiterDev', '$DEVICEDATA_Use_OpenGL_effects', '$OpenGLeffects')"
	RunSQL "$Q"

	# disable alpha blending
	Q="REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
	     VALUES ('$OrbiterDev', '$DEVICEDATA_Use_alpha_blended_UI', '$AlphaBlending')"
	RunSQL "$Q"

	# set UI	
	Q="REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
	     VALUES ('$OrbiterDev', '$DEVICEDATA_PK_UI', '$UI_Version')"
	RunSQL "$Q"


	local xorg_file="/etc/X11/xorg.conf"
	local composite_value="true"
	if [[ "$AlphaBlending" == "0" ]] ;then
		composite_value="false"
	fi

	if grep -qi "Section.*\"Extensions\"" "$xorg_file" ;then
		echo 'Found `Section "Extensions"` in xorg.conf'

		if grep -qi "Option \"Composite\"" "$xorg_file" ;then
			echo 'Found `Option "Composite"` in xorg.conf'

			sed -i "s/Option.*\"Composite\".*/Option \"Composite\" \"$composite_value\"/ig" "$xorg_file"
		else
			echo 'Not Found `Option "Composite"` in xorg.conf'

			sed -i "s/Section.*\"Extensions\"/Section \"Extensions\"\\nOption \"Composite\" \"$composite_value\"/ig" "$xorg_file"
		fi
	else
		echo 'Not Found `Section "Extenstions"` or `Option "Composite"`'
		echo 'Section "Extensions"' >> "$xorg_file"
		echo "    Option \"Composite\" \"$composite_value\"" >> "$xorg_file"
		echo 'EndSection' >> "$xorg_file"
	fi
}

function Setup_Firstboot {
	cp ./firstboot /etc/rc2.d/S89firstboot
}
