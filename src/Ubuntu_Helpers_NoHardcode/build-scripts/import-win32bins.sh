#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh

set -e
#set -x

function try_deb() {
	# we try to download the needed file
	# from deb.linuxmce.org, and only 
	# create an empty file, if deb doesn't
	# carry it.
	file=$1
	touch $1
	wget http://deb.linuxmce.org/$1 -O $1 &> /dev/null || : 
}
	

function fake_win32bins() {
	DisplayMessage "*** STEP: Fake Importing WIN32 binaries"
	pushd ${svn_dir}/${svn_branch_name}/src/bin
	try_deb Pluto_S60.sis
	try_deb Pluto_S60.sisx
	try_deb Orbiter.CAB
	try_deb Orbiter_Treo.CAB

	try_deb UpdateBinary.exe
	try_deb UpdateBinaryCE.exe 
	try_deb UpdateBinaryCE_x86.exe
	try_deb Orbiter_CeNet4_XScale.dat
	try_deb Orbiter_Win32.dat
	touch "Symbian Series 60 mobile.vmc"
	touch "Windows Mobile Smartphone.vmc"
	try_deb Orbiter_CeNet4_x86.dat

	try_deb OrbiterInstaller.msi
	try_deb OrbiterCE_SDL.CAB
	try_deb Orbiter_CeNet4_XScale.CAB
	try_deb Orbiter_CeNet4_x86.CAB
	try_deb ImportContacts.zip

	try_deb Orbiter.exe
	try_deb Orbiter_CeNet4_XScale.exe
	try_deb OrbiterSmartphone.exe
	try_deb OrbiterCE_SDL.exe
	try_deb Orbiter_CeNet4_x86.exe

	try_deb AYGSHELL.DLL
	try_deb PthreadsCE.dll
	try_deb Orbiter.MD5
	try_deb logo.gif

	try_deb PlutoBaSInstaller.msi
	try_deb PlutoRebootSetup.msi

#	wget -q http://hob.dyndns.org/javamo/JavaMO.jar
#	wget -q http://hob.dyndns.org/javamo/JavaMO.jad
        cp ${svn_dir}/${svn_branch_name}/src/Orbiter/Maemo/fremantle.install .
        
	try_deb JavaMO.jar
	try_deb JavaMO.jad
	popd
		

}

function import_win32bins() {
	pushd ${svn_dir}/${svn_branch_name}/src/bin
	wget --ftp-user="$win32_ftp_user" --ftp-password="$win32_ftp_password" \
		"$win32_ftp_url"/bin/*

	cp ${svn_dir}/${svn_branch_name}/src/Orbiter/Maemo/fremantle.install .

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
