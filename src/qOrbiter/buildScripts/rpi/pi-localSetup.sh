#!/bin/bash
DEVUSER="lmcedev"
echo "Getting dependencies for Qt5"
sudo apt-get install debhelper dh-apparmor freetds-common freetds-dev gir1.2-atspi-2.0 gir1.2-gst-plugins-base-0.10 gir1.2-gstreamer-0.10 gir1.2-gtk-2.0 icu-devtools libatk1.0-dev libatspi2.0-dev libcairo-script-interpreter2 libcairo2-dev libct4 libcups2-dev libdbus-glib-1-dev libgbm-dev libgdk-pixbuf2.0-dev libgstreamer-plugins-base0.10-dev libgstreamer0.10-dev libgtk2.0-dev libicu-dev  libjpeg8-dev libodbc1 libopenvg1-mesa-dev libpango1.0-dev libpixman-1-dev libpq-dev libsqlite3-dev libsybdb5 libudev-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev libxcb-render-util0-dev libxcb-xkb-dev libxcomposite-dev libxcursor-dev libxft-dev libxi-dev libxinerama-dev libxkbcommon-dev  libxkbcommon-x11-dev libxml2-dev libxrandr-dev libxrender-dev libxtst-dev po-debconf unixodbc unixodbc-dev x11proto-composite-dev x11proto-randr-dev x11proto-record-dev x11proto-render-dev x11proto-xinerama-dev xvfb libjpeg-dev libudev-dev libinput-dev libts-dev libxcb-xinerama0-dev rsync

sudo apt-get build-dep qt4-x11
sudo apt-get build-dep libqt5gui5

if id "$DEVUSER" >/dev/null 2>&1; then
        echo "user exists"
           
if [ -z /usr/local/qt5pi ];
then
echo "No local qt5 dir for rsync setup"
sudo mkdir /usr/local/qt5pi
sudo chown $DEVUSER:$DEVUSER /usr/local/qt5pi
fi
       
else
        echo "user does not exist"
        
        useradd -m -p lmce $DEVUSER
     
fi