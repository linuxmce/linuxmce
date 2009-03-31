#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh

set -e
#set -x

function fake_win32bins() {
	touch ${svn_dir}/${svn_branch_name}/src/bin/Pluto_S60.sis
	touch ${svn_dir}/${svn_branch_name}/src/bin/Orbiter.CAB
	touch ${svn_dir}/${svn_branch_name}/src/bin/Orbiter_Treo.CAB

	touch ${svn_dir}/${svn_branch_name}/src/bin/UpdateBinary.exe
	touch ${svn_dir}/${svn_branch_name}/src/bin/UpdateBinaryCE.exe 
	touch ${svn_dir}/${svn_branch_name}/src/bin/UpdateBinaryCE_x86.exe
	touch ${svn_dir}/${svn_branch_name}/src/bin/{Orbiter_Win32.dat,Orbiter_CeNet4_XScale.dat}
	touch "${svn_dir}/${svn_branch_name}/src/bin/Symbian Series 60 mobile.vmc"
	touch "${svn_dir}/${svn_branch_name}/src/bin/Windows Mobile Smartphone.vmc"
	touch ${svn_dir}/${svn_branch_name}/src/bin/Orbiter_CeNet4_x86.dat

	touch ${svn_dir}/${svn_branch_name}/src/bin/OrbiterInstaller.msi
	touch ${svn_dir}/${svn_branch_name}/src/bin/OrbiterCE_SDL.CAB
	touch ${svn_dir}/${svn_branch_name}/src/bin/Orbiter_CeNet4_XScale.CAB
	touch ${svn_dir}/${svn_branch_name}/src/bin/Orbiter_CeNet4_x86.CAB
	touch ${svn_dir}/${svn_branch_name}/src/bin/ImportContacts.zip

	touch ${svn_dir}/${svn_branch_name}/src/bin/Orbiter.exe
	touch ${svn_dir}/${svn_branch_name}/src/bin/Orbiter_CeNet4_XScale.exe
	touch ${svn_dir}/${svn_branch_name}/src/bin/OrbiterSmartphone.exe
	touch ${svn_dir}/${svn_branch_name}/src/bin/OrbiterCE_SDL.exe
	touch ${svn_dir}/${svn_branch_name}/src/bin/Orbiter_CeNet4_x86.exe

	touch ${svn_dir}/${svn_branch_name}/src/bin/AYGSHELL.DLL
	touch ${svn_dir}/${svn_branch_name}/src/bin/PthreadsCE.dll
	touch ${svn_dir}/${svn_branch_name}/src/bin/Orbiter.MD5
	touch ${svn_dir}/${svn_branch_name}/src/bin/logo.gif

	touch ${svn_dir}/${svn_branch_name}/src/bin/PlutoBaSInstaller.msi
	touch ${svn_dir}/${svn_branch_name}/src/bin/PlutoRebootSetup.msi

	touch ${svn_dir}/${svn_branch_name}/src/bin/JavaMO.jar
	touch ${svn_dir}/${svn_branch_name}/src/bin/JavaMO.jad
	

}

function import_win32bins() {
	pushd ${svn_dir}/${svn_branch_name}/src/bin
	wget --ftp-user="$win32_ftp_user" --ftp-password="$win32_ftp_password" \
		"$win32_ftp_url"/bin/*

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
