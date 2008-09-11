#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh

set -e
set -x

function fake_win32bins() {
	touch ${svn_dir}/trunk/src/bin/Pluto_S60.sis
	touch ${svn_dir}/trunk/src/bin/Orbiter.CAB
	touch ${svn_dir}/trunk/src/bin/Orbiter_Treo.CAB

	touch ${svn_dir}/trunk/src/bin/UpdateBinary.exe
	touch ${svn_dir}/trunk/src/bin/UpdateBinaryCE.exe 
	touch ${svn_dir}/trunk/src/bin/UpdateBinaryCE_x86.exe
	touch ${svn_dir}/trunk/src/bin/{Orbiter_Win32.dat,Orbiter_CeNet4_XScale.dat}
	touch "${svn_dir}/trunk/src/bin/Symbian Series 60 mobile.vmc"
	touch "${svn_dir}/trunk/src/bin/Windows Mobile Smartphone.vmc"
	touch ${svn_dir}/trunk/src/bin/Orbiter_CeNet4_x86.dat

	touch ${svn_dir}/trunk/src/bin/OrbiterInstaller.msi
	touch ${svn_dir}/trunk/src/bin/OrbiterCE_SDL.CAB
	touch ${svn_dir}/trunk/src/bin/Orbiter_CeNet4_XScale.CAB
	touch ${svn_dir}/trunk/src/bin/Orbiter_CeNet4_x86.CAB
	touch ${svn_dir}/trunk/src/bin/ImportContacts.zip

	touch ${svn_dir}/trunk/src/bin/Orbiter.exe
	touch ${svn_dir}/trunk/src/bin/Orbiter_CeNet4_XScale.exe
	touch ${svn_dir}/trunk/src/bin/OrbiterSmartphone.exe
	touch ${svn_dir}/trunk/src/bin/OrbiterCE_SDL.exe
	touch ${svn_dir}/trunk/src/bin/Orbiter_CeNet4_x86.exe

	touch ${svn_dir}/trunk/src/bin/AYGSHELL.DLL
	touch ${svn_dir}/trunk/src/bin/PthreadsCE.dll
	touch ${svn_dir}/trunk/src/bin/Orbiter.MD5
	touch ${svn_dir}/trunk/src/bin/logo.gif

	touch ${svn_dir}/trunk/src/bin/PlutoBaSInstaller.msi
	touch ${svn_dir}/trunk/src/bin/PlutoRebootSetup.msi
}

function import_win32bins() {
	pushd ${svn_dir}/trunk/src/bin
	wget --ftp-user=ftp --ftp-password=toomanypeopledownloading ftp://builder32.linuxmce.com/WinBinaries/Windows_Output_LinuxMCE/src/bin/*
	popd

	pushd ${svn_dir}/trunk/src/lib
	wget --ftp-user=ftp --ftp-password=toomanypeopledownloading ftp://builder32.linuxmce.com/WinBinaries/Windows_Output_LinuxMCE/src/lib/* 
	popd
}

DisplayMessage "*** STEP: Importing WIN32 binaries"
trap 'Error "Undefined error in $0"' EXIT

import_win32bins

trap - EXIT
