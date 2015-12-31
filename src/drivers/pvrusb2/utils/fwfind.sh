#!/bin/bash

# GNU GENERAL PUBLIC LICENSE
# Version 2, June 1991

# Roger  <roger@eskimo.com>
# 20050717
# CVS: $Header$

# Variable Declarations
shopt -s -o nounset
declare -rx SCRIPT=${0##*/}

declare -a filename[0]="pvrusb2_inf_26c.zip" filename[1]="pvrusb2_23_21351.exe"
declare filefound=""
declare CDROM="/mnt/cdrom*"
declare -i COUNTER=0
declare DRIVERFOLDER="./win_driver"
declare -i RETURN

declare -r cp="/usr/bin/cp"
declare -r fwextract="./fwextract.pl"
declare -r rm="/usr/bin/rm"
declare -r unzip="/usr/bin/unzip"
declare -r wget="/usr/bin/wget"

# Sanity checks for binaries
if test ! -x "$fwextract" ; then
	printf "$SCRIPT:$LINENO: the perl fwextract.pl is not found -\
	please make sure it's in this folder - aborting\n " >&2
	exit 1
fi

if test ! -x "$cp" ; then
        printf "$SCRIPT:$LINENO: the command cp is not available -\
	please install sys-apps/coreutils - aborting\n " >&2
	exit 1
fi
			
if test ! -x "$rm" ; then
        printf "$SCRIPT:$LINENO: the command rm is not available -\
	please install sys-apps/coreutils - aborting\n " >&2
	exit 1
fi
			
if test ! -x "$wget" ; then
	printf "$SCRIPT:$LINENO: the command wget is not available -\
	please install net-misc/wget - aborting\n " >&2
	exit 1
fi


if test ! -x "$unzip" ; then
	printf "$SCRIPT:$LINENO: the command unzip is not available -\
	please install app-arch/unzip - aborting\n " >&2
	exit 1
fi



function dounzip
{
	$rm -rf $DRIVERFOLDER
	$unzip -oq $filefound -d $DRIVERFOLDER
	RETURN="$?"
	if test "$RETURN" -eq 1; then
		printf "Error: Problem unzipping file to %s\n" $DRIVERFOLDER
		exit 1
	fi
}

function docleanup
{
# remove $DRIVERFOLDER
rm -rf $DRIVERFOLDER
}

function docheck
{
# Check to make sure we have two firmwares 
# and they both have a file size greater then zero.
if test ! -s "v4l-fx2-pvrusb2.fw" -o ! -s "v4l-cx2341x-enc.fw" ; then
	printf "\nFirmware extraction failed!\n"
	printf "Missing either v4l-fx2-pvrusb2.fw v4l-cx2341x-enc.fw, or has file size of zero.\n"
	exit 1
else
	printf "\nSuccessfully extracted v4l-fx2-pvrusb2.fw v4l-cx2341x-enc.fw to this foler.\n"
	printf "You should now copy these files to your /lib/firmware folder.\n"
fi
}


# Main

# Check this folder for files in array.
printf "Checking this folder for Haupgage driver package...\n"
for (( COUNTER=0; COUNTER<2; COUNTER++ )) ; do
	if test -e "./${filename[COUNTER]}" ; then
		printf "Found driver file %s, unzipping to %s.\n" "${filename[COUNTER]}" $DRIVERFOLDER
		filefound=${filename[COUNTER]}
		dounzip
		$fwextract
		docheck
		docleanup
		exit 0
	fi
done

# Check for HCWUSB2.SYS in this folder
printf "Checking for HCWUSB2.SYS in within this folder...\n"
filefound=$(find ./ -maxdepth 1 -iname "HCWUSB2.SYS")
if test -s "$filefound" ; then
	$rm -rf $DRIVERFOLDER
	mkdir $DRIVERFOLDER
	# This is an case insensitive method of finding & copying a file.
	cp $filefound ./$DRIVERFOLDER/hcwusb2.sys
	$fwextract
	docheck
	docleanup
	
	exit 0
fi


# Check for HCWUSB2.SYS on cdrom
printf "Checking for HCWUSB2.SYS within the cdrom folder...\n"
filefound=$(find $CDROM -iname "HCWUSB2.SYS")
if test -s "$filefound" ; then
	$rm -rf $DRIVERFOLDER
	mkdir $DRIVERFOLDER
	$cp $filefound $DRIVERFOLDER/
	$fwextract
	docheck
	docleanup
	exit 0
fi


# If I can't find the driver locally, try the internet
printf "Checking Hauppauge website for a specific driver package...\n"
$wget -c http://hauppauge.lightpath.net/software/pvrusb2/pvrusb2_inf_26c.zip
if test -s "pvrusb2_inf_26c.zip" ; then
	printf "\nSuccessfully downloaded pvrusb2_inf_26c.zip\n"
	printf "Restart script to complete extraction.\n"
	exit 0
fi

printf "\nI can't find any driver package containing HCWUSB2.SYS or the specific \
file itself.\n"

exit 1

