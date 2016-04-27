#!/bin/bash
source setEnv.sh
if [ -d "rpi-tools/$QT_VERSION" ]; then
#make clean -j8
cd rpi-tools/$QT_VERSION
./configure -release -force-debug-info -opengl es2 -device linux-rasp-pi-g++ -device-option CROSS_COMPILE=$STARTPATH/rpi-tools/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf- -sysroot $STARTPATH/rpi-tools/sysroot -opensource -confirm-license -make libs -prefix /usr/local/qt5pi -extprefix $STARTPATH/qt/qt5pi -hostprefix  $STARTPATH/qt/qt5 -c++11 -make examples -v -I $STARTPATH/rpi-tools/sysroot/opt/vc/include/ -I $STARTPATH/rpi-tools/sysroot/opt/vc/include/interface/vcos/pthreads/ -I $STARTPATH/rpi-tools/sysroot/opt/vc/include/interface/vcos/ -skip webkit
#make module-qtdeclarative -j8
echo "You Have 10 seconds to cancel auto build"
sleep 10
STARTTIME=$(date)
ENDTIME=""
make -j8
if [ $? -eq 0 ]; then
make install
ENDTIME=$(date)
echo "Finished install"
echo "Qt Sync to rpi"
rsync -avz qt/qt5pi  $TARGET_USER@$TARGET_IP:/usr/local
else
clear
echo "Your Build failed. manually run to find the errors"
ENDTIME=$(date)
fi

fi
echo "Started at $STARTTIME"
echo "Finished at $ENDTIME"