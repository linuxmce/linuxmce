#!/bin/bash

VERSION=1404
PACKAGE_VERSION=$(date +%Y%m%d%H%M)
OS=fremantle
HOME=Maemo
DEST=build
SRC=src

rm -rf ${DEST}
mkdir ${DEST}
mkdir ${DEST}/usr
cp -r ${SRC}/share ${DEST}/usr
cp ${SRC}/debian/* ${DEST}


export SNR_CPPFLAGS="-DMAEMO_NOKIA770 -DPADORBITER"
export SNR_LDFLAGS=""

cd ../../		
mkdir -p lib/ bin/

echo "Cleaning previous build"
rm -f lib/* bin/*

for TARGET in SerializeClass SDL_Helpers DCE PlutoUtils
do
	echo "== Building ${TARGET} ... =="
	cd ${TARGET}
	make clean && make
	if [ $? -ne 0 ] ; then
		echo "Failed building of ${TARGET}: ${?}!"
		exit 1
	fi		
	cd ../
done

echo "Curr dir is `pwd`" ...

echo "Buiding the Orbiter ..."

cd Orbiter
make -f Makefile_Nokia770 clean && make -f Makefile_Nokia770 bin
if [ $? -ne 0 ] ; then
	echo "Failed building of Maemo wrapper: ${?}!"
	exit 1
fi		

echo "Finished with building of the Orbiter. ..."

cd ${HOME}

mkdir $DEST/usr/lib
cp ../../lib/*.so ${DEST}/usr/lib
mkdir $DEST/usr/bin
cp ../../bin/Orbiter ${DEST}/usr/bin
mkdir -p $DEST/usr/pluto/share
cp ../Linux/GTKOrbiter.glade ${DEST}/usr/pluto/share


export SNR_CPPFLAGS="-DOS2008"

make clean && make lmce

cp linuxmceorbiter ${DEST}/usr/bin

strip ${DEST}/usr/bin/* ${DEST}/usr/lib/* 

cd ${DEST}

touch md5sum
for FILE in `find usr | grep -v "svn"`; do
	if [ -f $FILE ] 
	then
		md5sum $FILE >> md5sum
	fi
done

for VAL in OS VERSION PACKAGE_VERSION
do
	eval RES="$"${VAL};
	sed "s/%%${VAL}%%/${RES}/" control > control.tmp
	mv control.tmp control
done

sed "s/%%SIZE%%/`du -s usr/ | awk '{print $1}'`/" control > control.tmp
mv control.tmp control

tar vzcf data.tar.gz --exclude=".svn" usr/
tar vzcf control.tar.gz control md5sum
ar q lmceorbiter-${OS}-${VERSION}-${PACKAGE_VERSION}_armel.deb  debian-binary control.tar.gz data.tar.gz

echo "Maemo Orbiter package is build"
