# 
# Do not Edit! Generated by:
# kickstarter.py
# 

lang en_US.UTF-8
keyboard us
timezone --utc America/New_York
auth --useshadow --enablemd5

clearpart --all

part /dos --size 64 --ondisk sdb --fstype=vfat --active
part / --size 3000 --ondisk sdb --fstype=ext3

rootpw meego 
xconfig --startxonboot
bootloader --timeout=0 --append="quiet"
desktop --autologinuser=meego  --defaultdesktop=XFCE --session="/usr/bin/OrbiterSession"
user --name meego  --groups audio,video --password meego 

# For anything after 20110221

repo   --name=oss  --baseurl=http://repo.meego.com/MeeGo/builds/trunk/latest/repos/oss/ia32/packages --save --debuginfo --source --gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-meego
repo   --name=nonoss --baseurl=http://repo.meego.com/MeeGo/builds/trunk/latest/repos/non-oss/ia32/packages --excludepkgs=pvr-bin

repo   --name=kernel --baseurl=http://repo.pub.meego.com/home:/vgrade/MeeGo_current_Core_standard
repo   --name=tschak909 --baseurl=http://repo.pub.meego.com/home:/tschak909/meego_current_extras
repo   --name=xfce --baseurl=http://repo.pub.meego.com/home:/auke:/xfce/meego_current_extras

%packages
@MeeGo Core
@X for Netbooks
#@MeeGo Compliance
@MeeGo X Window System

meego-panel-networks

pluto-orbiter
matchbox-keyboard

gtk2-immodule-xim

kernel-adaptation-joggler
emgd-bin-1812

# XFCE Bits
xfwm4
xfwm4-themes

moblin-gtk-engine

# remove crap
-bluez
-buteo-mtp
-buteo-syncfw
-buteo-syncml
-buteo-sync-plugins
-ohm-plugin-resolver
-libresource
-libresource-client
-libresource-qt
-libsignon
-libsignon-passwordplugin
-libsignon-saslplugin
-libtelepathy
-ofono
-ohm
-ohm-config
-ohm-plugin-core
-ohm-plugin-ruleengine
-ohm-plugin-misc
-PackageKit-qt
-qt-mobility
-telepathy-farsight
-telepathy-gabble
-telepathy-glib
-telepathy-mission-control
-telepathy-qt4
-telepathy-qt4-farsight
-telepathy-ring
-telepathy-sofiasip
-tracker
-geoclue
-pacrunner

plymouth-lite
ntp
ntpdate

openssh-clients
openssh-server

meego-netbook-theme
netbook-icon-theme
wmctrl

%end

%post

# save a little bit of space at least...
rm -f /boot/initrd*

# make sure there aren't core files lying around
rm -f /core*

# Prelink can reduce boot time
if [ -x /usr/sbin/prelink ]; then
    /usr/sbin/prelink -aRqm
fi

echo "192.168.80.1 dcerouter" >>/etc/hosts

cat > /etc/xdg/autostart/matchbox-keyboard.desktop << EOF
[Desktop Entry]
Name=Matchbox Virtual Keyboard
Exec=/usr/bin/matchbox-keyboard -d
EOF

cat > /etc/X11/xorg.conf << EOF
Section "ServerLayout"
    Identifier     "Default Layout"
    Screen 0       "Panel"		0 0
    InputDevice    "TKPANEL"            "CorePointer"
    # Following lines disable screen blanking
    Option  "BlankTime"     "0"
    Option  "StandbyTime"   "0"
    Option  "SuspendTime"   "0"
    Option  "OffTime"       "0"
EndSection

Section "Screen"
    Identifier    "Panel"
    Device        "Intel_IEGD-0"
    Monitor       "LVDS"
    DefaultDepth  24
    SubSection    "Display"
	Depth     24
	Modes     "800x480"
    EndSubSection
EndSection

Section "Device"
    Identifier "Intel_IEGD-0"
    Driver     "emgd"
    VendorName "Intel(R) DEG"
    BoardName  "Embedded Graphics"
    BusID      "0:2:0"
    Screen      0
    Option     "PcfVersion"				"1792"
    Option     "ConfigId"				"1"
    Option     "PortDrivers"				"lvds"
    Option     "ALL/1/name"				"OpenFrame-Sharp"
    Option     "ALL/1/General/PortOrder"		"40000"
    Option     "ALL/1/General/DisplayConfig"		"1"
    Option     "ALL/1/General/DisplayDetect"		"0"

    Option     "ALL/1/Port/4/General/name"		"LVDS"
    Option     "ALL/1/Port/4/General/EdidAvail"		"0"
    Option     "ALL/1/Port/4/General/EdidNotAvail"	"4"
    Option     "ALL/1/Port/4/General/Rotation"		"0"
    Option     "ALL/1/Port/4/General/Edid"		"0"
    Option     "ALL/1/Port/4/FpInfo/BkltMethod"		"0"
    Option     "ALL/1/Port/4/Dtd/1/PixelClock"		"33264"
    Option     "ALL/1/Port/4/Dtd/1/HorzActive"		"800"
    Option     "ALL/1/Port/4/Dtd/1/HorzSync"		"41"
    Option     "ALL/1/Port/4/Dtd/1/HorzSyncPulse"	"128"
    Option     "ALL/1/Port/4/Dtd/1/HorzBlank"		"256"
    Option     "ALL/1/Port/4/Dtd/1/VertActive"		"480"
    Option     "ALL/1/Port/4/Dtd/1/VertSync"		"10"
    Option     "ALL/1/Port/4/Dtd/1/VertSyncPulse"	"6"
    Option     "ALL/1/Port/4/Dtd/1/VertBlank"		"45"
    Option     "ALL/1/Port/4/Dtd/1/Flags"		"0x20000"
    Option     "ALL/1/Port/4/Attr/27"			"0"
    Option     "ALL/1/Port/4/Attr/26"			"24"
    Option     "ALL/1/Port/4/Attr/60"			"1"
    Option     "ALL/1/Port/4/Attr/49"			"0"
    Option     "ALL/1/Port/4/Attr/36"			"0x7d8675"
    Option     "ALL/1/Port/4/Attr/37"			"0x8f8f8a"
#    Option     "ALL/1/Port/4/Attr/43"    		"14"
    Option     "ALL/1/Port/4/Attr/45"    		"0"
EndSection

Section "Files"
        ModulePath   "/usr/lib/xorg/modules"
        FontPath     "/usr/share/X11/fonts/misc"
EndSection

Section "Module"
        Load  "dbe"
        Load  "freetype"
        Load  "extmod"
        Load  "record"
	Load  "dri"
	Load  "glx"
EndSection

Section "InputClass"
        Identifier "evdev keyboard catchall"
        MatchIsKeyboard "on"
        MatchDevicePath "/dev/input/event*"
        Driver "evdev"
EndSection

Section "InputClass"
        Identifier "evdev pointer catchall"
        MatchIsPointer "on"
        MatchDevicePath "/dev/input/event*"
        Driver "evdev"
EndSection

#Section "InputDevice"
#       Identifier  "Keyboard"
#	Driver      "kbd"
#        Option      "XkbLayout" "us"
#        Option      "CoreKeyboard"
#EndSection

#Section "InputDevice"
#        Identifier  "Mouse"
#        Driver      "mouse"
#        Option      "Protocol" "Auto"
#        Option      "ZAxisMapping" "4 5"
#        Option      "CorePointer"
#        Option      "Device" "/dev/input/mice"
#EndSection

Section "Monitor"
	Identifier   "LVDS"
EndSection

Section "DRI"
        Mode         0666
EndSection

Section "InputDevice"              
        Identifier      "TKPANEL"  
        Driver          "evdev"  
        Option          "Device"                "/dev/input/event0"
        Option          "DeviceName"            "touchscreen"
        Option          "MinX"                  "0"
        Option          "MinY"                  "0"
        Option          "MaxX"                  "32768"

        Option          "MaxY"                  "32768"
	Option          "MAXABS"                "32768"
        Option          "AN"                    "-300440"
        Option          "BN"                    "0"
        Option          "CN"                    "5754880"
        Option          "DN"                    "0"                     
        Option          "EN"                    "-178080"    
        Option          "FN"                    "8050080"
        Option          "DIVIDER"               "-378996"
        Option          "SCALE"                 "32"   
        Option          "ReportingMode"         "Raw"  
#       Option          "Emulate3Buttons"              
#       Option          "Emulate3Timeout"       "50"     
	Option          "SendCoreEvents"        "On"
        Option          "MoveLimit"             "1"      
#       Option          "DebugLevel"            "5"
EndSection
EOF

cat > /usr/bin/OrbiterSession << EOF
#!/bin/sh

# Set time via NTP

/etc/init.d/ntpd stop
ntpdate ntp.ucsd.edu
/etc/init.d/ntpd 
ldconfig

xfwm4 &
export GTK_IM_MODULE=matchbox-im-invoker

RET=0

	/usr/libexec/carrick-connection-panel -s &

while [ 1 ]
do
	/usr/bin/Orbiter
done
	
EOF

# Build the input method cache
update-gtk-immodules i686-linux-gnu 

chmod +x /usr/bin/OrbiterSession
cp /usr/bin/OrbiterSession /usr/bin/orbitersession

# remove daemons we do not need

rm /etc/xdg/autostart/matchbox-panel.desktop
rm /etc/xdg/autostart/msyncd.desktop
rm /etc/xdg/autostart/mthemedaemon.desktop
rm /etc/xdg/autostart/pulseaudio.desktop
rm /etc/xdg/autostart/tracker*.desktop

 
# work around for poor key import UI in PackageKit
rm -f /var/lib/rpm/__db*
rpm --rebuilddb

if [ -f /etc/pki/rpm-gpg/RPM-GPG-KEY-meego ]; then
    rpm --import /etc/pki/rpm-gpg/RPM-GPG-KEY-meego
fi

%end

%post --nochroot
if [ -n "$IMG_NAME" ]; then
    echo "BUILD: $IMG_NAME" >> $INSTALL_ROOT/etc/meego-release
fi

cd $INSTALL_ROOT/dos/
wget http://bug10738.openaos.org/images/joggler/image-support-files/joggler-fat-partition.tgz
tar xzvf joggler-fat-partition.tgz
rm joggler-fat-partition.tgz

sed -i 's/bzImage/vmlinuz-2.6.35.10-18.1-adaptation-joggler quiet splash/g' $INSTALL_ROOT/dos/grub.cfg

%end
