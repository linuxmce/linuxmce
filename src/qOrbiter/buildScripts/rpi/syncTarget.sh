#!/bin/sh
if [ -z $TARGET_USER];
then
echo "PI_MD env var not set, no sync"
else
echo "syncing"
rsync -avz $TARGET_USER@$TARGET_IP:/lib rpi-tools/sysroot
rsync -avz $TARGET_USER@$TARGET_IP:/usr/include rpi-tools/sysroot/usr
rsync -avz $TARGET_USER@$TARGET_IP:/usr/lib rpi-tools/sysroot/usr
rsync -avz $TARGET_USER@$TARGET_IP:/opt/vc rpi-tools/sysroot/opt
rsync -avz qt/qt5pi  $TARGET_USER@$TARGET_IP:/usr/local/
fi

exit