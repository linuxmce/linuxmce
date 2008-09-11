#!/bin/bash
error=0
# /usr/pluto/bin/ConfirmDependencies -r -D pluto_main -h dcerouter -u root -d 46 install
#Device: Generic PC as MD (#28 Generic PC as MD) requires software
# Skipping package Windows .NET Dlls because it is not compatible with this distro
#Device: OnScreen Orbiter (#62 OnScreen Orbiter) requires software
#.Pluto Orbiter
#..ALSA libraries
#..SDL ALSA support
#..X protocol client library
#..X misc extensions library
#..X input extension library
#..X video extension library
#..X client data
#..SDL TTF library w/ FT2
#...SDL (Simple DirectMedia Layer) library
#...FreeType 2 shared libraries
#..PNG library
#...zlib
#..SDL image library
#...JPEG runtime library
#..SDL SGE library
#...GNU Standard C++ Library v5
#..X Session Management library
#..X misc utils library
#..X lightweight misc utils library
#..X pixmap library
#..X RR&R library
#..X Rendering client library
#..X Toolkit Intrinsics
#..X protocol trapping
#..X event record&test
#..X cursor management library
#..XFT2 FreeType-based font library for X
#..Pluto App Server
#...MySQL 4 client libraries
#...MySQL 4 common
#...udev
#..Xine Media Player User Interface
#...ALSA Base
#..Pluto DCE library
#...Pluto SerializeClass library
#....Pluto PlutoUtils library
#..X Window System Core
#..ethtool
#..screen
#..Pluto Boot Scripts
#...NetCat
#...Pluto MessageSend
#...MySQL 4 client
#...NFS Kernel Server
#...syslinux
#...bzip2
#...NTP date
#...Discover 1
#...Pluto Shift Key State
#...FTP upload
#...iproute
#...whois (mkpasswd)
# Skipping package libcrypt-smbhash-perl because it is not compatible with this distro
#...libdigest-md4-perl
#...Network Storage
#...lshwd
#...Dummy - Pluto Console Utilities
#....SSH
#....cdrecord
#....Vi IMproved (VIM)
#...Pluto Update System
#...bind9-host
#...Pluto Hal Daemon
#....Pluto Serial Class library
#....Hal
#...Pluto Storage Devices
#....Automount Daemon
#....Pluto SmbFS Kernel Module
#....Samba Utils
#....Samba Client
#....nbtscan
#...Pluto Disk Monitor
#...Pluto PNP Detection Scripts
#...Pluto Test Serial Port
#...Pluto Sound Card Setup Scripts
#...Pluto Capture Card Setup Scripts
#..Microsoft TrueType core fonts
#..Pluto Confirm Dependencies
#...Pluto pluto_main library
#..less
#..SDL-gfx
# Skipping package PThread because it is not compatible with this distro
# Skipping package Windows .NET Dlls because it is not compatible with this distro
#..eject
#..Macromedia Flash Player plugin for Mozilla
#...Mozilla Firefox
#..Pluto SDL Helpers
#..abcde
#...libogg0
#...libvorbis0a
#...cd-discid
#...flac
#...libao2
#...libflac6
#...liboggflac1
#...libvorbisenc2
#...libvorbisfile3
#...vorbis-tools
#...cdparanoia
#....libcdparanoia0
#..Xdialog
#...libgtk1.2-common
#...libgtk1.2
#..xloadimage
#..Pluto SimplePhone
#...Linphone common files
#...Linphone library
#..bootsplash
#..Pluto X Configuration Scripts
#..xcompmgr
#..libxvmc1
#..IceWM
#..Pluto IceWM Configuration
#..Pluto AV Wizard
#...Pluto Xine Player
#....Xine player library
#.....libdvdcss
#.....id-my-disc
#....ICE library
#....Speex Speach Codec
#....X printing extension library
#....ALSA utilities
#....imagemagick
#...Pluto HAL Device Finder
#...Pluto AV Wizard Sounds
#...Pluto AV Wizard Skin Basic
#...bc
#..Mesa DRI Library
#..Pluto Add Software
#Device: Orbiter Embedded Phone (#1759 Orbiter Embedded Phone) requires software
#.Pluto SimplePhone
#Device: Xine Player (#5 Xine Player) requires software
#.Pluto Xine Player
#Device: MythTV Player (#35 MythTV Player) requires software
#.Pluto MythTV Player
#..Qt  GUI Library MT v3
#..Debian Font Manager
#..generic font config library
#..Network Audio System libraries
#..XML parsing library
#..generic font config shared library
#..Perl module for FT2
#..GLib2 library
#..GLib2 common files
#..Color management library
#..MNG library
#..FreeType 1 library
#..pkg-config
#..TTF Bitstream Vera
#..Update Configuration File (UCF)
#..aRts C library
#..MySQL 3 client libraries
#..Qt3 MySQL driver
#..MythTV library
#...LAME library
#..XMLTV Util
#..MythTV backend
#..MythTV frontend
#Device: Disk Drive (#11 Disk Drive) requires software
#.Pluto Disk Drive
#..CDDB library
#..SSL shared libraries
#..Pluto Disk Drive Functions
#..libdvdread
#Device: Photo Screen Saver (#1825 Photo Screen Saver) requires software
#.Pluto Photo Screen Saver
#Device: External Media Identifier (#1701 External Media Identifier) doesn't require any software.
#Device: App Server (#26 App Server) requires software
#.Pluto App Server
#Device: HAL (#1808 HAL) requires software
#.Pluto Hal Daemon
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 12
	# Rep. type: 1
	# Package: ALSA libraries Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libasound2" "deb http://deb.plutohome.com/debian" "replacements main" "1" "1.0.8-1" "" "" "" "" "" "" "" "" "12"; then
		echo "Confirmation of package 12 'ALSA libraries' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'ALSA libraries' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 15
	# Rep. type: 1
	# Package: SDL ALSA support Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libsdl1.2debian-alsa" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.2.7-10" "" "" "" "" "" "" "" "" "15"; then
		echo "Confirmation of package 15 'SDL ALSA support' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'SDL ALSA support' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 18
	# Rep. type: 1
	# Package: X protocol client library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libx11-6" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.3.0.dfsg.1-8" "" "" "" "" "" "" "" "" "18"; then
		echo "Confirmation of package 18 'X protocol client library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X protocol client library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 20
	# Rep. type: 1
	# Package: X misc extensions library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libxext6" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.3.0.dfsg.1-8" "" "" "" "" "" "" "" "" "20"; then
		echo "Confirmation of package 20 'X misc extensions library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X misc extensions library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 22
	# Rep. type: 1
	# Package: X input extension library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libxi6" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.3.0.dfsg.1-8" "" "" "" "" "" "" "" "" "22"; then
		echo "Confirmation of package 22 'X input extension library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X input extension library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 24
	# Rep. type: 1
	# Package: X video extension library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libxv1" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.3.0.dfsg.1-8" "" "" "" "" "" "" "" "" "24"; then
		echo "Confirmation of package 24 'X video extension library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X video extension library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 33
	# Rep. type: 1
	# Package: X client data Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "xlibs-data" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.3.0.dfsg.1-8" "" "" "" "" "" "" "" "" "33"; then
		echo "Confirmation of package 33 'X client data' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X client data' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 8
	# Rep. type: 1
	# Package: SDL (Simple DirectMedia Layer) library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libsdl1.2debian" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.2.7-10" "" "" "" "" "" "" "" "" "8"; then
		echo "Confirmation of package 8 'SDL (Simple DirectMedia Layer) library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'SDL (Simple DirectMedia Layer) library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 35
	# Rep. type: 1
	# Package: FreeType 2 shared libraries Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libfreetype6" "deb http://deb.plutohome.com/debian" "sarge" "1" "2.1.7-2.2" "" "" "" "" "" "" "" "" "35"; then
		echo "Confirmation of package 35 'FreeType 2 shared libraries' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'FreeType 2 shared libraries' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 37
	# Rep. type: 1
	# Package: SDL TTF library w/ FT2 Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libsdl-ttf2.0-0" "deb http://deb.plutohome.com/debian" "sarge" "1" "2.0.6-5" "" "" "" "" "" "" "" "" "37"; then
		echo "Confirmation of package 37 'SDL TTF library w/ FT2' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'SDL TTF library w/ FT2' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 165
	# Rep. type: 1
	# Package: zlib Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "zlib1g" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.2.1.1-7" "" "" "" "" "" "" "" "" "165"; then
		echo "Confirmation of package 165 'zlib' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'zlib' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 41
	# Rep. type: 1
	# Package: PNG library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libpng12-0" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.2.7-1" "" "" "" "" "" "" "" "" "41"; then
		echo "Confirmation of package 41 'PNG library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'PNG library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 39
	# Rep. type: 1
	# Package: JPEG runtime library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libjpeg62" "deb http://deb.plutohome.com/debian" "sarge" "1" "6b-9" "" "" "" "" "" "" "" "" "39"; then
		echo "Confirmation of package 39 'JPEG runtime library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'JPEG runtime library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 43
	# Rep. type: 1
	# Package: SDL image library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libsdl-image1.2" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.2.3-5" "" "" "" "" "" "" "" "" "43"; then
		echo "Confirmation of package 43 'SDL image library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'SDL image library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 163
	# Rep. type: 1
	# Package: GNU Standard C++ Library v5 Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libstdc++5" "deb http://deb.plutohome.com/debian" "sarge" "1" "3.3.4-13" "" "" "" "" "" "" "" "" "163"; then
		echo "Confirmation of package 163 'GNU Standard C++ Library v5' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'GNU Standard C++ Library v5' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 47
	# Rep. type: 1
	# Package: SDL SGE library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libsdl-sge" "deb http://deb.plutohome.com/debian" "sarge" "1" "020904-5" "" "" "" "" "" "" "" "" "47"; then
		echo "Confirmation of package 47 'SDL SGE library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'SDL SGE library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 55
	# Rep. type: 1
	# Package: X Session Management library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libsm6" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.3.0.dfsg.1-8" "" "" "" "" "" "" "" "" "55"; then
		echo "Confirmation of package 55 'X Session Management library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X Session Management library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 59
	# Rep. type: 1
	# Package: X misc utils library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libxmu6" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.3.0.dfsg.1-8" "" "" "" "" "" "" "" "" "59"; then
		echo "Confirmation of package 59 'X misc utils library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X misc utils library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 61
	# Rep. type: 1
	# Package: X lightweight misc utils library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libxmuu1" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.3.0.dfsg.1-8" "" "" "" "" "" "" "" "" "61"; then
		echo "Confirmation of package 61 'X lightweight misc utils library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X lightweight misc utils library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 65
	# Rep. type: 1
	# Package: X pixmap library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libxpm4" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.3.0.dfsg.1-8" "" "" "" "" "" "" "" "" "65"; then
		echo "Confirmation of package 65 'X pixmap library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X pixmap library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 67
	# Rep. type: 1
	# Package: X RR&R library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libxrandr2" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.3.0.dfsg.1-8" "" "" "" "" "" "" "" "" "67"; then
		echo "Confirmation of package 67 'X RR&R library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X RR&R library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 69
	# Rep. type: 1
	# Package: X Rendering client library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libxrender1" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.3.0.dfsg.1-8" "" "" "" "" "" "" "" "" "69"; then
		echo "Confirmation of package 69 'X Rendering client library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X Rendering client library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 71
	# Rep. type: 1
	# Package: X Toolkit Intrinsics Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libxt6" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.3.0.dfsg.1-8" "" "" "" "" "" "" "" "" "71"; then
		echo "Confirmation of package 71 'X Toolkit Intrinsics' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X Toolkit Intrinsics' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 73
	# Rep. type: 1
	# Package: X protocol trapping Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libxtrap6" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.3.0.dfsg.1-8" "" "" "" "" "" "" "" "" "73"; then
		echo "Confirmation of package 73 'X protocol trapping' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X protocol trapping' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 75
	# Rep. type: 1
	# Package: X event record&test Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libxtst6" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.3.0.dfsg.1-8" "" "" "" "" "" "" "" "" "75"; then
		echo "Confirmation of package 75 'X event record&test' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X event record&test' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 99
	# Rep. type: 1
	# Package: X cursor management library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libxcursor1" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.1.3-1" "" "" "" "" "" "" "" "" "99"; then
		echo "Confirmation of package 99 'X cursor management library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X cursor management library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 101
	# Rep. type: 1
	# Package: XFT2 FreeType-based font library for X Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libxft2" "deb http://deb.plutohome.com/debian" "sarge" "1" "2.1.2-6" "" "" "" "" "" "" "" "" "101"; then
		echo "Confirmation of package 101 'XFT2 FreeType-based font library for X' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'XFT2 FreeType-based font library for X' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 3
	# Rep. type: 1
	# Package: MySQL 4 client libraries Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libmysqlclient12" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.0.21-7" "" "" "" "" "" "" "" "" "3"; then
		echo "Confirmation of package 3 'MySQL 4 client libraries' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'MySQL 4 client libraries' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 5
	# Rep. type: 1
	# Package: MySQL 4 common Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "mysql-common" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.0.21-7" "" "" "" "" "" "" "" "" "5"; then
		echo "Confirmation of package 5 'MySQL 4 common' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'MySQL 4 common' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 463
	# Rep. type: 1
	# Package: udev Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "udev" "deb http://deb.plutohome.com/debian" "replacements main" "1" "" "" "" "" "" "" "" "" "" "463"; then
		echo "Confirmation of package 463 'udev' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'udev' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 126
	# Rep. type: 1
	# Package: Pluto App Server Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-app-server" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "pluto/bin" "" "" "" "" "" "" "" "126"; then
		echo "Confirmation of package 126 'Pluto App Server' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto App Server' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 275
	# Rep. type: 1
	# Package: ALSA Base Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "alsa-base" "deb http://deb.plutohome.com/debian" "replacements main" "1" "" "" "" "" "" "" "" "" "" "275"; then
		echo "Confirmation of package 275 'ALSA Base' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'ALSA Base' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 189
	# Rep. type: 1
	# Package: Xine Media Player User Interface Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "xine-ui" "deb http://deb.plutohome.com/debian" "sarge" "1" "0.99.1-1" "" "" "" "" "" "" "" "" "189"; then
		echo "Confirmation of package 189 'Xine Media Player User Interface' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Xine Media Player User Interface' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 197
	# Rep. type: 1
	# Package: Pluto PlutoUtils library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-plutoutils" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "pluto/lib" "" "" "" "" "197"; then
		echo "Confirmation of package 197 'Pluto PlutoUtils library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto PlutoUtils library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 238
	# Rep. type: 1
	# Package: Pluto SerializeClass library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-serializeclass" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "pluto/lib" "" "" "" "" "238"; then
		echo "Confirmation of package 238 'Pluto SerializeClass library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto SerializeClass library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 193
	# Rep. type: 1
	# Package: Pluto DCE library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-dcecommon" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "pluto/lib" "" "" "" "" "193"; then
		echo "Confirmation of package 193 'Pluto DCE library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto DCE library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 222
	# Rep. type: 1
	# Package: X Window System Core Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "x-window-system-core" "deb http://deb.plutohome.com/debian" "replacements main" "1" "" "" "" "" "" "" "" "" "" "222"; then
		echo "Confirmation of package 222 'X Window System Core' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X Window System Core' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 225
	# Rep. type: 1
	# Package: ethtool Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "ethtool" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "225"; then
		echo "Confirmation of package 225 'ethtool' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'ethtool' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 228
	# Rep. type: 1
	# Package: screen Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "screen" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "228"; then
		echo "Confirmation of package 228 'screen' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'screen' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 235
	# Rep. type: 1
	# Package: NetCat Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "netcat" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "235"; then
		echo "Confirmation of package 235 'NetCat' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'NetCat' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 242
	# Rep. type: 1
	# Package: Pluto MessageSend Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-messagesend" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "pluto/bin" "" "" "" "" "" "" "" "242"; then
		echo "Confirmation of package 242 'Pluto MessageSend' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto MessageSend' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 249
	# Rep. type: 1
	# Package: MySQL 4 client Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "mysql-client" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "249"; then
		echo "Confirmation of package 249 'MySQL 4 client' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'MySQL 4 client' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 252
	# Rep. type: 1
	# Package: NFS Kernel Server Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "nfs-kernel-server" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "252"; then
		echo "Confirmation of package 252 'NFS Kernel Server' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'NFS Kernel Server' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 258
	# Rep. type: 1
	# Package: syslinux Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "syslinux" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "258"; then
		echo "Confirmation of package 258 'syslinux' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'syslinux' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 263
	# Rep. type: 1
	# Package: bzip2 Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "bzip2" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "263"; then
		echo "Confirmation of package 263 'bzip2' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'bzip2' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 264
	# Rep. type: 1
	# Package: NTP date Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "ntpdate" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "264"; then
		echo "Confirmation of package 264 'NTP date' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'NTP date' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 273
	# Rep. type: 1
	# Package: Discover 1 Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "discover1" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "273"; then
		echo "Confirmation of package 273 'Discover 1' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Discover 1' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 274
	# Rep. type: 1
	# Package: Pluto Shift Key State Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-shiftstate" "deb http://deb.plutohome.com/debian" "20dev main" "1" "" "pluto/bin" "" "" "" "" "" "" "" "274"; then
		echo "Confirmation of package 274 'Pluto Shift Key State' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Shift Key State' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 292
	# Rep. type: 1
	# Package: FTP upload Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "ftp-upload" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "292"; then
		echo "Confirmation of package 292 'FTP upload' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'FTP upload' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 295
	# Rep. type: 1
	# Package: iproute Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "iproute" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "295"; then
		echo "Confirmation of package 295 'iproute' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'iproute' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 321
	# Rep. type: 1
	# Package: whois (mkpasswd) Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "whois" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "321"; then
		echo "Confirmation of package 321 'whois (mkpasswd)' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'whois (mkpasswd)' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 389
	# Rep. type: 1
	# Package: libdigest-md4-perl Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libdigest-md4-perl" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "389"; then
		echo "Confirmation of package 389 'libdigest-md4-perl' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'libdigest-md4-perl' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 503
	# Rep. type: 1
	# Package: Network Storage Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-network-storage" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "" "" "" "" "" "503"; then
		echo "Confirmation of package 503 'Network Storage' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Network Storage' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 511
	# Rep. type: 1
	# Package: lshwd Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "lshwd" "deb http://deb.plutohome.com/debian" "replacements main" "1" "" "" "" "" "" "" "" "" "" "511"; then
		echo "Confirmation of package 511 'lshwd' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'lshwd' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 250
	# Rep. type: 1
	# Package: SSH Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "ssh" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "250"; then
		echo "Confirmation of package 250 'SSH' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'SSH' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 251
	# Rep. type: 1
	# Package: cdrecord Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "cdrecord" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "251"; then
		echo "Confirmation of package 251 'cdrecord' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'cdrecord' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 253
	# Rep. type: 1
	# Package: Vi IMproved (VIM) Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "vim" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "253"; then
		echo "Confirmation of package 253 'Vi IMproved (VIM)' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Vi IMproved (VIM)' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 522
	# Rep. type: 1
	# Package: Dummy - Pluto Console Utilities Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-dummy-console-tools" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "" "" "" "" "" "522"; then
		echo "Confirmation of package 522 'Dummy - Pluto Console Utilities' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Dummy - Pluto Console Utilities' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 523
	# Rep. type: 1
	# Package: Pluto Update System Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-update-system" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "" "" "" "" "" "523"; then
		echo "Confirmation of package 523 'Pluto Update System' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Update System' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 524
	# Rep. type: 1
	# Package: bind9-host Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "bind9-host" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "524"; then
		echo "Confirmation of package 524 'bind9-host' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'bind9-host' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 191
	# Rep. type: 1
	# Package: Pluto Serial Class library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-libserial" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "pluto/lib" "" "" "" "" "191"; then
		echo "Confirmation of package 191 'Pluto Serial Class library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Serial Class library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 531
	# Rep. type: 1
	# Package: Hal Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "hal" "deb http://deb.plutohome.com/debian" "replacements main" "1" "" "" "" "" "" "" "" "" "" "531"; then
		echo "Confirmation of package 531 'Hal' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Hal' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 530
	# Rep. type: 1
	# Package: Pluto Hal Daemon Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-hald" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "pluto/bin" "" "" "" "" "" "" "" "530"; then
		echo "Confirmation of package 530 'Pluto Hal Daemon' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Hal Daemon' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 534
	# Rep. type: 1
	# Package: Automount Daemon Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "autofs" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "534"; then
		echo "Confirmation of package 534 'Automount Daemon' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Automount Daemon' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 545
	# Rep. type: 1
	# Package: Pluto SmbFS Kernel Module Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-smbfs" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "/lib/modules/" "" "" "" "" "" "" "" "545"; then
		echo "Confirmation of package 545 'Pluto SmbFS Kernel Module' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto SmbFS Kernel Module' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 547
	# Rep. type: 1
	# Package: Samba Utils Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "smbfs" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "547"; then
		echo "Confirmation of package 547 'Samba Utils' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Samba Utils' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 591
	# Rep. type: 1
	# Package: Samba Client Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "smbclient" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "591"; then
		echo "Confirmation of package 591 'Samba Client' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Samba Client' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 594
	# Rep. type: 1
	# Package: nbtscan Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "nbtscan" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "594"; then
		echo "Confirmation of package 594 'nbtscan' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'nbtscan' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 533
	# Rep. type: 1
	# Package: Pluto Storage Devices Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-storage-devices" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "" "" "" "" "" "533"; then
		echo "Confirmation of package 533 'Pluto Storage Devices' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Storage Devices' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 554
	# Rep. type: 1
	# Package: Pluto Disk Monitor Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-disk-monitor" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "" "" "" "" "" "554"; then
		echo "Confirmation of package 554 'Pluto Disk Monitor' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Disk Monitor' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 562
	# Rep. type: 1
	# Package: Pluto PNP Detection Scripts Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-pnp-detection" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "" "" "" "" "" "562"; then
		echo "Confirmation of package 562 'Pluto PNP Detection Scripts' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto PNP Detection Scripts' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 564
	# Rep. type: 1
	# Package: Pluto Test Serial Port Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-test-serial-port" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "pluto/bin" "" "" "" "" "" "" "" "564"; then
		echo "Confirmation of package 564 'Pluto Test Serial Port' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Test Serial Port' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 579
	# Rep. type: 1
	# Package: Pluto Sound Card Setup Scripts Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-sound-card-scripts" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "" "" "" "" "" "579"; then
		echo "Confirmation of package 579 'Pluto Sound Card Setup Scripts' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Sound Card Setup Scripts' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 580
	# Rep. type: 1
	# Package: Pluto Capture Card Setup Scripts Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-capture-card-scripts" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "" "" "" "" "" "580"; then
		echo "Confirmation of package 580 'Pluto Capture Card Setup Scripts' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Capture Card Setup Scripts' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 234
	# Rep. type: 1
	# Package: Pluto Boot Scripts Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-boot-scripts" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "" "" "" "" "" "234"; then
		echo "Confirmation of package 234 'Pluto Boot Scripts' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Boot Scripts' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 243
	# Rep. type: 1
	# Package: Microsoft TrueType core fonts Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "msttcorefonts" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "243"; then
		echo "Confirmation of package 243 'Microsoft TrueType core fonts' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Microsoft TrueType core fonts' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 237
	# Rep. type: 1
	# Package: Pluto pluto_main library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-pluto-main-db" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "pluto/bin" "" "" "" "" "237"; then
		echo "Confirmation of package 237 'Pluto pluto_main library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto pluto_main library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 272
	# Rep. type: 1
	# Package: Pluto Confirm Dependencies Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-confirm-dependencies" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "pluto/bin" "" "" "" "" "" "" "" "272"; then
		echo "Confirmation of package 272 'Pluto Confirm Dependencies' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Confirm Dependencies' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 276
	# Rep. type: 1
	# Package: less Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "less" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "276"; then
		echo "Confirmation of package 276 'less' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'less' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 288
	# Rep. type: 1
	# Package: SDL-gfx Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libsdl-gfx1.2" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "288"; then
		echo "Confirmation of package 288 'SDL-gfx' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'SDL-gfx' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 317
	# Rep. type: 1
	# Package: eject Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "eject" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "317"; then
		echo "Confirmation of package 317 'eject' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'eject' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 296
	# Rep. type: 1
	# Package: Mozilla Firefox Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "mozilla-firefox" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "296"; then
		echo "Confirmation of package 296 'Mozilla Firefox' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Mozilla Firefox' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 345
	# Rep. type: 1
	# Package: Macromedia Flash Player plugin for Mozilla Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "flashplayer-mozilla" "deb http://www.yttron.as.ro/" "unstable main" "1" "" "" "" "" "" "" "" "" "" "345"; then
		echo "Confirmation of package 345 'Macromedia Flash Player plugin for Mozilla' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Macromedia Flash Player plugin for Mozilla' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 346
	# Rep. type: 1
	# Package: Pluto SDL Helpers Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-sdl-helpers" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "pluto/lib" "" "" "" "" "346"; then
		echo "Confirmation of package 346 'Pluto SDL Helpers' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto SDL Helpers' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 54
	# Rep. type: 1
	# Package: libogg0 Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libogg0" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.1.0-1" "" "" "" "" "" "" "" "" "54"; then
		echo "Confirmation of package 54 'libogg0' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'libogg0' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 58
	# Rep. type: 1
	# Package: libvorbis0a Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libvorbis0a" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.0.1-1" "" "" "" "" "" "" "" "" "58"; then
		echo "Confirmation of package 58 'libvorbis0a' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'libvorbis0a' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 409
	# Rep. type: 1
	# Package: cd-discid Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "cd-discid" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "409"; then
		echo "Confirmation of package 409 'cd-discid' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'cd-discid' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 410
	# Rep. type: 1
	# Package: flac Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "flac" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "410"; then
		echo "Confirmation of package 410 'flac' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'flac' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 411
	# Rep. type: 1
	# Package: libao2 Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libao2" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "411"; then
		echo "Confirmation of package 411 'libao2' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'libao2' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 412
	# Rep. type: 1
	# Package: libflac6 Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libflac6" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "412"; then
		echo "Confirmation of package 412 'libflac6' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'libflac6' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 413
	# Rep. type: 1
	# Package: liboggflac1 Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "liboggflac1" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "413"; then
		echo "Confirmation of package 413 'liboggflac1' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'liboggflac1' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 414
	# Rep. type: 1
	# Package: libvorbisenc2 Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libvorbisenc2" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "414"; then
		echo "Confirmation of package 414 'libvorbisenc2' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'libvorbisenc2' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 415
	# Rep. type: 1
	# Package: libvorbisfile3 Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libvorbisfile3" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "415"; then
		echo "Confirmation of package 415 'libvorbisfile3' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'libvorbisfile3' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 416
	# Rep. type: 1
	# Package: vorbis-tools Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "vorbis-tools" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "416"; then
		echo "Confirmation of package 416 'vorbis-tools' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'vorbis-tools' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 418
	# Rep. type: 1
	# Package: libcdparanoia0 Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libcdparanoia0" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "418"; then
		echo "Confirmation of package 418 'libcdparanoia0' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'libcdparanoia0' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 417
	# Rep. type: 1
	# Package: cdparanoia Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "cdparanoia" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "417"; then
		echo "Confirmation of package 417 'cdparanoia' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'cdparanoia' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 408
	# Rep. type: 1
	# Package: abcde Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "abcde" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "408"; then
		echo "Confirmation of package 408 'abcde' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'abcde' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 441
	# Rep. type: 1
	# Package: libgtk1.2-common Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libgtk1.2-common" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "441"; then
		echo "Confirmation of package 441 'libgtk1.2-common' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'libgtk1.2-common' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 442
	# Rep. type: 1
	# Package: libgtk1.2 Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libgtk1.2" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "442"; then
		echo "Confirmation of package 442 'libgtk1.2' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'libgtk1.2' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 440
	# Rep. type: 1
	# Package: Xdialog Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "xdialog" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "440"; then
		echo "Confirmation of package 440 'Xdialog' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Xdialog' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 443
	# Rep. type: 1
	# Package: xloadimage Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "xloadimage" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "443"; then
		echo "Confirmation of package 443 'xloadimage' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'xloadimage' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 230
	# Rep. type: 1
	# Package: Linphone common files Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "linphone-common" "deb http://deb.plutohome.com/debian" "replacements main" "1" "" "" "" "" "" "" "" "" "" "230"; then
		echo "Confirmation of package 230 'Linphone common files' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Linphone common files' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 348
	# Rep. type: 1
	# Package: Linphone library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "liblinphone1" "deb http://deb.plutohome.com/debian" "replacements main" "1" "" "" "" "" "" "" "" "" "" "348"; then
		echo "Confirmation of package 348 'Linphone library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Linphone library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 499
	# Rep. type: 1
	# Package: Pluto SimplePhone Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-simplephone" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "pluto/bin" "" "" "" "" "" "" "" "499"; then
		echo "Confirmation of package 499 'Pluto SimplePhone' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto SimplePhone' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 512
	# Rep. type: 1
	# Package: bootsplash Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "bootsplash" "deb http://deb.plutohome.com/debian" "replacements main" "1" "" "" "" "" "" "" "" "" "" "512"; then
		echo "Confirmation of package 512 'bootsplash' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'bootsplash' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 520
	# Rep. type: 1
	# Package: Pluto X Configuration Scripts Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-x-scripts" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "" "" "" "" "" "520"; then
		echo "Confirmation of package 520 'Pluto X Configuration Scripts' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto X Configuration Scripts' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 532
	# Rep. type: 1
	# Package: xcompmgr Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "xcompmgr" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "532"; then
		echo "Confirmation of package 532 'xcompmgr' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'xcompmgr' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 537
	# Rep. type: 1
	# Package: libxvmc1 Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libxvmc1" "deb http://deb.plutohome.com/debian" "replacements main" "1" "" "" "" "" "" "" "" "" "" "537"; then
		echo "Confirmation of package 537 'libxvmc1' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'libxvmc1' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 548
	# Rep. type: 1
	# Package: IceWM Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "icewm" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "548"; then
		echo "Confirmation of package 548 'IceWM' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'IceWM' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 549
	# Rep. type: 1
	# Package: Pluto IceWM Configuration Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-icewm-config" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "" "" "" "" "" "549"; then
		echo "Confirmation of package 549 'Pluto IceWM Configuration' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto IceWM Configuration' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 205
	# Rep. type: 1
	# Package: libdvdcss Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libdvdcss" "deb http://www.yttron.as.ro/" "realextra main" "1" "" "" "" "" "" "" "" "" "" "205"; then
		echo "Confirmation of package 205 'libdvdcss' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'libdvdcss' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 527
	# Rep. type: 1
	# Package: id-my-disc Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "id-my-disc" "deb http://www.yttron.as.ro/" "realextra main" "1" "" "" "" "" "" "" "" "" "" "527"; then
		echo "Confirmation of package 527 'id-my-disc' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'id-my-disc' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 50
	# Rep. type: 1
	# Package: Xine player library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libxine1-dbg" "deb http://deb.plutohome.com/debian" "sarge" "1" "1-rc6a-1" "" "" "" "" "" "" "" "" "50"; then
		echo "Confirmation of package 50 'Xine player library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Xine player library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 52
	# Rep. type: 1
	# Package: ICE library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libice6" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.3.0.dfsg.1-8" "" "" "" "" "" "" "" "" "52"; then
		echo "Confirmation of package 52 'ICE library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'ICE library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 57
	# Rep. type: 1
	# Package: Speex Speach Codec Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libspeex1" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.0.rel.4-1" "" "" "" "" "" "" "" "" "57"; then
		echo "Confirmation of package 57 'Speex Speach Codec' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Speex Speach Codec' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 63
	# Rep. type: 1
	# Package: X printing extension library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libxp6" "deb http://deb.plutohome.com/debian" "sarge" "1" "4.3.0.dfsg.1-8" "" "" "" "" "" "" "" "" "63"; then
		echo "Confirmation of package 63 'X printing extension library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'X printing extension library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 293
	# Rep. type: 1
	# Package: ALSA utilities Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "alsa-utils" "deb http://deb.plutohome.com/debian" "replacements main" "1" "" "" "" "" "" "" "" "" "" "293"; then
		echo "Confirmation of package 293 'ALSA utilities' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'ALSA utilities' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 451
	# Rep. type: 1
	# Package: imagemagick Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "imagemagick" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "451"; then
		echo "Confirmation of package 451 'imagemagick' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'imagemagick' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 117
	# Rep. type: 1
	# Package: Pluto Xine Player Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-xine-player" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "pluto/bin" "" "" "" "" "" "" "" "117"; then
		echo "Confirmation of package 117 'Pluto Xine Player' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Xine Player' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 535
	# Rep. type: 1
	# Package: Pluto HAL Device Finder Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-hal-device-finder" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "pluto/bin" "" "" "" "" "" "" "" "535"; then
		echo "Confirmation of package 535 'Pluto HAL Device Finder' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto HAL Device Finder' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 561
	# Rep. type: 1
	# Package: Pluto AV Wizard Sounds Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-avwizard-sounds" "deb http://deb.plutohome.com/debian" "replacements main" "1" "" "" "" "" "" "" "" "" "" "561"; then
		echo "Confirmation of package 561 'Pluto AV Wizard Sounds' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto AV Wizard Sounds' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 567
	# Rep. type: 1
	# Package: Pluto AV Wizard Skin Basic Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-avwizard-skin-basic" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "" "" "" "" "" "" "" "" "567"; then
		echo "Confirmation of package 567 'Pluto AV Wizard Skin Basic' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto AV Wizard Skin Basic' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 571
	# Rep. type: 1
	# Package: bc Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "bc" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "571"; then
		echo "Confirmation of package 571 'bc' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'bc' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 556
	# Rep. type: 1
	# Package: Pluto AV Wizard Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-avwizard" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "pluto/bin" "" "" "" "" "" "" "" "556"; then
		echo "Confirmation of package 556 'Pluto AV Wizard' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto AV Wizard' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 570
	# Rep. type: 1
	# Package: Mesa DRI Library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libgl1-mesa-dri" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "570"; then
		echo "Confirmation of package 570 'Mesa DRI Library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Mesa DRI Library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 573
	# Rep. type: 1
	# Package: Pluto Add Software Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-add-software" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "pluto/bin" "" "" "" "" "" "" "" "573"; then
		echo "Confirmation of package 573 'Pluto Add Software' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Add Software' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 119
	# Rep. type: 1
	# Package: Pluto Orbiter Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-orbiter" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "pluto/bin" "" "" "" "" "" "" "" "119"; then
		echo "Confirmation of package 119 'Pluto Orbiter' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Orbiter' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 80
	# Rep. type: 1
	# Package: Qt  GUI Library MT v3 Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libqt3c102-mt" "deb http://deb.plutohome.com/debian" "sarge" "1" "3:3.3.3-4.1" "" "" "" "" "" "" "" "" "80"; then
		echo "Confirmation of package 80 'Qt  GUI Library MT v3' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Qt  GUI Library MT v3' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 82
	# Rep. type: 1
	# Package: Debian Font Manager Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "defoma" "deb http://deb.plutohome.com/debian" "sarge" "1" "0.11.8-0.1" "" "" "" "" "" "" "" "" "82"; then
		echo "Confirmation of package 82 'Debian Font Manager' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Debian Font Manager' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 83
	# Rep. type: 1
	# Package: generic font config library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "fontconfig" "deb http://deb.plutohome.com/debian" "sarge" "1" "2.2.3-1" "" "" "" "" "" "" "" "" "83"; then
		echo "Confirmation of package 83 'generic font config library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'generic font config library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 84
	# Rep. type: 1
	# Package: Network Audio System libraries Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libaudio2" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.6d-2" "" "" "" "" "" "" "" "" "84"; then
		echo "Confirmation of package 84 'Network Audio System libraries' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Network Audio System libraries' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 86
	# Rep. type: 1
	# Package: XML parsing library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libexpat1" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.95.8-1" "" "" "" "" "" "" "" "" "86"; then
		echo "Confirmation of package 86 'XML parsing library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'XML parsing library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 87
	# Rep. type: 1
	# Package: generic font config shared library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libfontconfig1" "deb http://deb.plutohome.com/debian" "sarge" "1" "2.2.3-1" "" "" "" "" "" "" "" "" "87"; then
		echo "Confirmation of package 87 'generic font config shared library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'generic font config shared library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 89
	# Rep. type: 1
	# Package: Perl module for FT2 Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libft-perl" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.2-13.1" "" "" "" "" "" "" "" "" "89"; then
		echo "Confirmation of package 89 'Perl module for FT2' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Perl module for FT2' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 90
	# Rep. type: 1
	# Package: GLib2 library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libglib2.0-0" "deb http://deb.plutohome.com/debian" "sarge" "1" "2.4.7-1" "" "" "" "" "" "" "" "" "90"; then
		echo "Confirmation of package 90 'GLib2 library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'GLib2 library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 91
	# Rep. type: 1
	# Package: GLib2 common files Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libglib2.0-data" "deb http://deb.plutohome.com/debian" "sarge" "1" "2.4.7-1" "" "" "" "" "" "" "" "" "91"; then
		echo "Confirmation of package 91 'GLib2 common files' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'GLib2 common files' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 92
	# Rep. type: 1
	# Package: Color management library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "liblcms1" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.12-2.1" "" "" "" "" "" "" "" "" "92"; then
		echo "Confirmation of package 92 'Color management library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Color management library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 95
	# Rep. type: 1
	# Package: MNG library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libmng1" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.0.8-1" "" "" "" "" "" "" "" "" "95"; then
		echo "Confirmation of package 95 'MNG library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'MNG library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 98
	# Rep. type: 1
	# Package: FreeType 1 library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libttf2" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.4pre.20030402-1.1" "" "" "" "" "" "" "" "" "98"; then
		echo "Confirmation of package 98 'FreeType 1 library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'FreeType 1 library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 103
	# Rep. type: 1
	# Package: pkg-config Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pkg-config" "deb http://deb.plutohome.com/debian" "sarge" "1" "0.15.0-4" "" "" "" "" "" "" "" "" "103"; then
		echo "Confirmation of package 103 'pkg-config' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'pkg-config' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 105
	# Rep. type: 1
	# Package: TTF Bitstream Vera Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "ttf-bitstream-vera" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.10-3" "" "" "" "" "" "" "" "" "105"; then
		echo "Confirmation of package 105 'TTF Bitstream Vera' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'TTF Bitstream Vera' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 106
	# Rep. type: 1
	# Package: Update Configuration File (UCF) Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "ucf" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.09" "" "" "" "" "" "" "" "" "106"; then
		echo "Confirmation of package 106 'Update Configuration File (UCF)' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Update Configuration File (UCF)' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 107
	# Rep. type: 1
	# Package: aRts C library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libartsc0" "deb http://deb.plutohome.com/debian" "sarge" "1" "1.3.1-1" "" "" "" "" "" "" "" "" "107"; then
		echo "Confirmation of package 107 'aRts C library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'aRts C library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 109
	# Rep. type: 1
	# Package: MySQL 3 client libraries Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libmysqlclient10" "deb http://deb.plutohome.com/debian" "sarge" "1" "3.23.56-2" "" "" "" "" "" "" "" "" "109"; then
		echo "Confirmation of package 109 'MySQL 3 client libraries' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'MySQL 3 client libraries' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 110
	# Rep. type: 1
	# Package: Qt3 MySQL driver Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libqt3c102-mt-mysql" "deb http://deb.plutohome.com/debian" "sarge" "1" "3:3.3.3-4.1" "" "" "" "" "" "" "" "" "110"; then
		echo "Confirmation of package 110 'Qt3 MySQL driver' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Qt3 MySQL driver' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 115
	# Rep. type: 1
	# Package: LAME library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "liblame0" "deb http://www.yttron.as.ro/" "sarge main" "1" "3.96.1-1" "" "" "" "" "" "" "" "" "115"; then
		echo "Confirmation of package 115 'LAME library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'LAME library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 113
	# Rep. type: 1
	# Package: MythTV library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libmyth-0.20" "deb http://deb.plutohome.com/debian" "unstable mythtv" "1" "" "" "" "" "" "" "" "" "" "113"; then
		echo "Confirmation of package 113 'MythTV library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'MythTV library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 216
	# Rep. type: 1
	# Package: XMLTV Util Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "xmltv-util" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "216"; then
		echo "Confirmation of package 216 'XMLTV Util' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'XMLTV Util' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 236
	# Rep. type: 1
	# Package: MythTV backend Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "mythtv-backend" "deb http://deb.plutohome.com/debian" "unstable mythtv" "1" "" "" "" "" "" "" "" "" "" "236"; then
		echo "Confirmation of package 236 'MythTV backend' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'MythTV backend' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 268
	# Rep. type: 1
	# Package: MythTV frontend Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "mythtv-frontend" "deb http://deb.plutohome.com/debian" "unstable mythtv" "1" "" "" "" "" "" "" "" "" "" "268"; then
		echo "Confirmation of package 268 'MythTV frontend' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'MythTV frontend' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 133
	# Rep. type: 1
	# Package: Pluto MythTV Player Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-mythtv-player" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "pluto/bin" "" "" "" "" "" "" "" "133"; then
		echo "Confirmation of package 133 'Pluto MythTV Player' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto MythTV Player' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 111
	# Rep. type: 1
	# Package: CDDB library Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libcddb0" "deb http://www.yttron.as.ro/" "sarge main" "1" "" "" "" "" "" "" "" "" "" "111"; then
		echo "Confirmation of package 111 'CDDB library' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'CDDB library' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 166
	# Rep. type: 1
	# Package: SSL shared libraries Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libssl0.9.7" "deb http://deb.plutohome.com/debian" "sarge" "1" "0.9.7d-5" "" "" "" "" "" "" "" "" "166"; then
		echo "Confirmation of package 166 'SSL shared libraries' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'SSL shared libraries' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 497
	# Rep. type: 1
	# Package: Pluto Disk Drive Functions Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-disc-drive-functions" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "pluto/lib" "" "" "" "" "" "" "" "497"; then
		echo "Confirmation of package 497 'Pluto Disk Drive Functions' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Disk Drive Functions' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 501
	# Rep. type: 1
	# Package: libdvdread Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "libdvdread3" "deb http://deb.plutohome.com/debian" "sarge" "1" "" "" "" "" "" "" "" "" "" "501"; then
		echo "Confirmation of package 501 'libdvdread' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'libdvdread' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 122
	# Rep. type: 1
	# Package: Pluto Disk Drive Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-disk-drive" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "pluto/bin" "" "" "" "" "" "" "" "122"; then
		echo "Confirmation of package 122 'Pluto Disk Drive' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Disk Drive' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
ok=0
while [ "$ok" -eq 0 ]; do
	dpkg --configure -a --force-confold
	# PK_Package: 582
	# Rep. type: 1
	# Package: Pluto Photo Screen Saver Type: Package (.deb, .rpm, etc.)
	if /usr/pluto/install/ConfirmDependencies_Debian.sh "pluto-photo-screen-saver" "deb http://deb.plutohome.com/debian" "20dev main" "1" "2.0.0.41.06110707" "pluto/bin" "" "" "" "" "" "" "" "582"; then
		echo "Confirmation of package 582 'Pluto Photo Screen Saver' went ok."
		ok=1
	else
		echo "***************************************************"
		echo "***ERROR*** Processing of package 'Pluto Photo Screen Saver' failed"
		echo "***************************************************"
		echo -n 'Do you want to try again? [Y/n]: '
		read answer
		if [ "$answer" == n -o "$answer" == N ]; then
			echo '*** Leaving package uninstalled'
			ok=1
			error=1
		fi
	fi
done
if [ "$error" -ne 0 ]; then
	echo "Note: Some packages failed to install"
fi
rm -f /usr/pluto/install/.notdone
echo "*************************"
echo "*************************"
echo "Installation complete."
echo "*************************"
echo "*************************"
