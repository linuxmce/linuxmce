#!/bin/bash

. /etc/lmce-build/builder.conf

if [[ "$NUM_CORES" -gt 1 ]] ; then make_jobs="-j $NUM_CORES"; fi

make O=rpi-out/ $make_jobs rpi_b

cp rpi-out/u-boot.bin rpi-sdcard/kernel.img

pushd rpi-sdcard >/dev/null
tar czvf ../../rpi-sdcard.tgz *
popd >/dev/null

