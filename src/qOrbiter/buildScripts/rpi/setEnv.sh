#!/bin/bash
export ROOT_FS_MOUNT="/mnt/rasp-pi-rootfs"
export QT_VERSION=qt-5.4.2
export TARGET_USER="lmcedev"
export TARGET_IP="192.168.80.2"
export TOOLCHAN_PATH=""
export STARTPATH=$(pwd)
export RPI_SYSROOT=$ROOT_FS_MOUNT
export RPI_TOOLCHAIN=$TOOLCHAN_PATH
export QT_BRANCH=5.5


echo "done with setEnv.sh"
