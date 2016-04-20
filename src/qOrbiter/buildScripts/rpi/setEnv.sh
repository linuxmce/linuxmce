#!/bin/sh
export ROOT_FS_MOUNT="/mnt/rasp-pi-rootfs"
export TARGET_USER="golgoj4"
export TARGET_IP="192.168.80.2"
export TOOLCHAN_PATH=""
export STARTPATH=$(pwd)
export RPI_SYSROOT=$ROOT_FS_MOUNT
export RPI_TOOLCHAIN=$TOOLCHAN_PATH
export QT_BRANCH=5.5


echo "done with setEnv.sh"