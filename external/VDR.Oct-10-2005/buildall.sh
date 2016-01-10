#!/bin/bash

# TO DO:
#  VersionChecking of vdrdevel and vdrdevel-plugins-*
#  write it in the plutohome-build-database ???

# Variables used for Compiling-options...
# should be controled later from version-checking

COMPILEVDR="1"
COMPILEPLUGINS="1"

rm PACKAGES/*.deb


# plutohome relevant VDR Stuff:
if [ "$COMPILEVDR" == "1" ]; then
	#clean up the old debs
	rm PACKAGES/*
	rm vdr_1.*.deb
	rm vdr-dev_1.*.deb
	rm vdrdevel_1.*.deb
	rm vdrdevel-dev_*.deb

	echo -e " GOING TO COMPILE NEW VDRDEVEL-VERSION\r"

	# vdrdevel
	pushd vdrdevel-1.*
	dpkg-buildpackage -tc
	popd

	echo -e " VDRDEVEL READY, INSTALLING DEV-PACKAGE\r"
	##IMPORTANT, to install the developer-Package first, before compiling the plugins (Version-Number-matching!)
	dpkg -i vdrdevel-dev_*.deb

        #vdr main base
        pushd vdr-1.*
        dpkg-buildpackage -tc
        popd
	                                
	                                

else
	echo -e " NO VERSION CHANGES IN VDRDEVEL-SOURCES\r"
fi

# Plugins should be compiled every time completely, cause of vdrdevel internal version checking...
if [ "$COMPILEPLUGINS" == "1" ]; then
	#clean up the old plugins
      cd ./plugins
     
	rm vdrdevel-plugin-*.deb

	echo -e " GOING TO COMPILE NEW VDRDEVEL-PLUGINS\r"
	
	
	# control (over telnet, ansi OSD)
	pushd vdrdevel-plugin-control-*
	dpkg-buildpackage -tc
	popd

	# mplayer & mp3
#	pushd vdrdevel-plugin-mp3-*
#	dpkg-buildpackage -tc
#	popd

	# osd teletext
#	pushd vdrdevel-plugin-osdteletext-*
#	dpkg-buildpackage -tc
#	popd


	# streamdev server
#	pushd vdrdevel-plugin-streamdev-*
#	dpkg-buildpackage -tc
#	popd

	# text2skin server
#	pushd vdrdevel-plugin-text2skin-*
#	dpkg-buildpackage -tc
#	popd

	# tvonscreen
#	pushd vdrdevel-plugin-tvonscreen-*
#	dpkg-buildpackage -tc
#	popd


	# skinelchi
        pushd vdrdevel-plugin-skinelchi-*
        dpkg-buildpackage -tc
        popd
	                        
	# xine pluto powered!
	# got to fix 00_patches
	pushd vdrdevel-plugin-xine-*
	dpkg-buildpackage -tc
	popd
	
     cd ..


else
	echo -e " NO VERSION CHANGES IN PLUGIN-SOURCES\r"
	
fi

### now put the stuff together in PACKAGES:
cp vdr_1.*.deb PACKAGES/.
cp vdrdevel_1.*.deb PACKAGES/.
cp plugins/*.deb PACKAGES/.
echo -e "###################################\r"
echo -e "###################################\r"
echo -e "###         FINISHED               \r"
echo -e "###################################\r"
echo -e "###################################\r"

