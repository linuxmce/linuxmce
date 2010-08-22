#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh

set -e
#set -x

function fake_win32bins() {
	pushd ${svn_dir}/${svn_branch_name}/src/bin
	touch Pluto_S60.sis
	touch Pluto_S60.sisx
	touch Orbiter.CAB
	touch Orbiter_Treo.CAB

	touch UpdateBinary.exe
	touch UpdateBinaryCE.exe 
	touch UpdateBinaryCE_x86.exe
	touch {Orbiter_Win32.dat,Orbiter_CeNet4_XScale.dat}
	touch "Symbian Series 60 mobile.vmc"
	touch "Windows Mobile Smartphone.vmc"
	touch Orbiter_CeNet4_x86.dat

	touch OrbiterInstaller.msi
	touch OrbiterCE_SDL.CAB
	touch Orbiter_CeNet4_XScale.CAB
	touch Orbiter_CeNet4_x86.CAB
	touch ImportContacts.zip

	touch Orbiter.exe
	touch Orbiter_CeNet4_XScale.exe
	touch OrbiterSmartphone.exe
	touch OrbiterCE_SDL.exe
	touch Orbiter_CeNet4_x86.exe

	touch AYGSHELL.DLL
	touch PthreadsCE.dll
	touch Orbiter.MD5
	touch logo.gif

	touch PlutoBaSInstaller.msi
	touch PlutoRebootSetup.msi

#	wget -q http://hob.dyndns.org/javamo/JavaMO.jar
#	wget -q http://hob.dyndns.org/javamo/JavaMO.jad
	
	touch JavaMO.jar
	touch JavaMO.jad
	popd
		

}

function import_win32bins() {
	pushd ${svn_dir}/${svn_branch_name}/src/bin
	wget --ftp-user="$win32_ftp_user" --ftp-password="$win32_ftp_password" \
		"$win32_ftp_url"/bin/*

	wget -q http://hob.dyndns.org/javamo/JavaMO.jar
	wget -q http://hob.dyndns.org/javamo/JavaMO.jad
	
	touch JavaMO.jar
	touch JavaMO.jad

	pushd ${svn_dir}/${svn_branch_name}/src/lib
	wget --ftp-user="$win32_ftp_user" --ftp-password="$win32_ftp_password" \
		"$win32_ftp_url"/lib/* 
	
}

DisplayMessage "*** STEP: Importing WIN32 binaries"
trap 'Error "Undefined error in $0"' EXIT

if [ x"$win32_create_fake" = x"yes" ] ; then
	fake_win32bins
elif [ x"$win32_ftp_url" != x"" -a x"$win32_ftp_user" != x"" ] ; then
	import_win32bins
else
	echo "You must either define win32_create_fake OR "
	echo "define the win32_ftp variables"
fi

trap - EXIT
