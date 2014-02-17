#!/bin/bash

# summary of how this script can be called:
#        * <postinst> `configure' <most-recently-configured-version>
#if [[ "$2" != "" ]] ;then
#	/usr/pluto/bin/Diskless_CreateTBZ.sh || echo "WARNING: Diskless_CreateTBZ.sh returned an error!"
#fi

if [ -x /usr/pluto/bin/Debug_LogKernelModules.sh ]; then
	/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
fi

#update-rc.d -f haltMDs start 01 0 6 . || /bin/true
#update-rc.d -f haltMDs stop 01 0 6 . || /bin/true
update-rc.d -f haltMDs remove || /bin/true

## delete bad file from previous installs
rm -f /etc/init/md_halt.conf



## move deb-cache to newer multi-arch friendly location
## update sourcs.list on core and all MDs to reflect new locations
UPDATE_CACHE=""
# Core
HOST_DISTRO=$(lsb_release -i -s | tr '[:upper:]' '[:lower:]')
HOST_RELEASE=$(lsb_release -c -s)
HOST_ARCH=$(apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)
DEB_CACHE=$HOST_DISTRO-$HOST_RELEASE-$HOST_ARCH
if [[ -n "$(find /var/cache/apt/archives/ -iname '*.deb')" ]]; then
	echo "Moving the Core's apt cache to /usr/pluto/deb-cache/$DEB_CACHE"
	find /var/cache/apt/archives/ -iname '*.deb' -exec mv {} /usr/pluto/deb-cache/$DEB_CACHE \;
	UPDATE_CACHE="$DEB_CACHE"
fi
# replace sources.list "/deb-cache " with "/deb-cache/$DEB_CACHE "
sed -i "s:/deb-cache :/deb-cache/$DEB_CACHE :" /etc/apt/sources.list
# MDs
for Moon_RootLocation in /usr/pluto/diskless/*; do
	if [[ -n "$(find ${Moon_RootLocation}/var/cache/apt/archives/ -iname '*.deb')" ]]; then
		TARGET_DISTRO=$(LC_ALL="C" chroot ${Moon_RootLocation} lsb_release -i -s | tr '[:upper:]' '[:lower:]')
		TARGET_RELEASE=$(LC_ALL="C" chroot ${Moon_RootLocation} lsb_release -c -s)
		TARGET_ARCH=$(LC_ALL="C" chroot ${Moon_RootLocation} apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)
		DEB_CACHE=$TARGET_DISTRO-$TARGET_RELEASE-$TARGET_ARCH

		echo "Moving this MDs apt cache to /usr/pluto/deb-cache/$DEB_CACHE"
		find ${Moon_RootLocation}/var/cache/apt/archives/ -iname '*.deb' -exec mv {} /usr/pluto/deb-cache/$DEB_CACHE \;
		if [[ "$UPDATE_CACHE" != "*$DEB_CACHE*" ]]; then
			UPDATE_CACHE="$UPDATE_CACHE $DEB_CACHE"
		fi
	fi
	# replace sources.list "/deb-cache " with "/deb-cache/$DEB_CACHE "
	sed -i "s:/deb-cache :/deb-cache/$DEB_CACHE :" ${Moon_RootLocation}/etc/apt/sources.list
done
# update package files
for cache_name in "$UPDATE_CACHE"; do
	echo "Updating packages list in /usr/pluto/deb-cache/$cache_name"
	/usr/pluto/bin/update-debcache.sh /usr/pluto/deb-cache/$cache_name
done
