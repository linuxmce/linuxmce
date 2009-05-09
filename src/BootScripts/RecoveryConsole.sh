#!/bin/bash

Connectivity()
{
	Fail=0
	Threshold=5;
	while [[ "$Fail" -lt "$Threshold" ]]; do
		./bin/ping -c 1 dcerouter &>/dev/null && Fail=0 || : $((Fail+=1))
		./bin/sleep 5
	done
}

ShellLoop()
{
	exec </dev/tty11 &>/dev/tty11
	while :; do
		echo "-- Diskless MD analysis and recovery console --"
		echo
		./bin/sh
		./bin/sleep 1
	done
}

Emergency()
{
	Connectivity
	./bin/chroot . chvt 11
}

mke2fs -m 0 /dev/ram0
mkdir -p /mnt/recovery-rd
mount /dev/ram0 /mnt/recovery-rd
cd /mnt/recovery-rd
tar -xf /usr/pluto/install/ramdisk.tar.bz2
mount -t devfs devfs dev
mount -t proc proc proc

Emergency &
#ShellLoop &
openvt -c 11 -- /usr/pluto/bin/RecoveryConsole_ShellLoop.sh
