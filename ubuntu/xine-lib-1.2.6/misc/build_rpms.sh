#!/bin/sh

#DATE=`date +%y%m%d`
DATE=1

PKGNAME=libxine1

# Some rpm checks
RPMVERSION=`rpm --version | tr [A-Z] [a-z] | sed -e 's/[a-z]//g' -e 's/\.//g' -e 's/ //g'`

# rpm version 4 return 40
if [ `expr $RPMVERSION` -lt 100 ]; then
  RPMVERSION=`expr $RPMVERSION \* 10`
fi

if [ `expr $RPMVERSION` -lt 400 ]; then
  RPM_BA="rpm -ba -ta ./xine-lib-1.2.6.tar.bz2"
  RPM_BB="rpm -bb -ta ./xine-lib-1.2.6.tar.bz2"
elif [ `expr $RPMVERSION` -lt 420 ]; then
  RPM_BA="rpm -ta ./xine-lib-1.2.6.tar.bz2 -ba"
  RPM_BB="rpm -ta ./xine-lib-1.2.6.tar.bz2 -bb"
else
  RPM_BA="rpmbuild -ta ./xine-lib-1.2.6.tar.bz2 -ba"
  RPM_BB="rpmbuild -ta ./xine-lib-1.2.6.tar.bz2 -bb"
fi
      
##VERSION="1.2.6"

echo "Creating tarball..."
rm -f config.cache && ./autogen.sh && make dist
rm -rf rpms
mkdir rpms

echo "*****************************************************"
echo
echo "building rpm for xine-lib 1.2.6"
echo 
echo "current architecture:pentium"
echo "rpms will be copied to ./rpms directory"
echo
echo "*****************************************************"

export XINE_BUILD=i586-pc-linux-gnu

eval $RPM_BA

mv /usr/src/redhat/SRPMS/libxine-1.2.6-$DATE.src.rpm ./rpms/
mv /usr/src/redhat/RPMS/i386/$PKGNAME-1.2.6-$DATE.i386.rpm ./rpms/$PKGNAME-1.2.6-$DATE.i586.rpm
mv /usr/src/redhat/RPMS/i386/$PKGNAME-devel-1.2.6-$DATE.i386.rpm ./rpms/$PKGNAME-devel-1.2.6-$DATE.i586.rpm

echo "*****************************************************"
echo
echo "building rpm for xine-lib 1.2.6"
echo 
echo "current architecture:pentiumpro"
echo "rpms will be copied to ./rpms directory"
echo
echo "*****************************************************"

export XINE_BUILD=i686-pc-linux-gnu

eval $RPM_BB

mv /usr/src/redhat/RPMS/i386/$PKGNAME-1.2.6-$DATE.i386.rpm ./rpms/$PKGNAME-1.2.6-$DATE.i686.rpm

echo "*****************************************************"
echo
echo "building rpm for xine-lib 1.2.6"
echo 
echo "current architecture:k6"
echo "rpms will be copied to ./rpms directory"
echo
echo "*****************************************************"

export XINE_BUILD=k6-pc-linux-gnu

eval $RPM_BB

mv /usr/src/redhat/RPMS/i386/$PKGNAME-1.2.6-$DATE.i386.rpm ./rpms/$PKGNAME-1.2.6-$DATE.k6.rpm

echo "*****************************************************"
echo
echo "building rpm for xine-lib 1.2.6"
echo 
echo "current architecture:k7"
echo "rpms will be copied to ./rpms directory"
echo
echo "*****************************************************"

export XINE_BUILD=athlon-pc-linux-gnu

eval $RPM_BB

mv /usr/src/redhat/RPMS/i386/$PKGNAME-1.2.6-$DATE.i386.rpm ./rpms/$PKGNAME-1.2.6-$DATE.k7.rpm

echo "Done."
