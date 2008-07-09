#!/bin/bash -e

rsync_skin_host="10.0.2.3"
rsync_media_host="10.0.2.3"

http_skin_host="www.linuxmce.org"
http_media_host="www.linuxmce.org"

. /etc/lmce-build/builder.conf

function ImportPublicSkinFromRSyncHost {
	skins_dir=/home/samba/www_docs/graphics
	mkdir -p /usr/pluto/orbiter/
	mkdir -p "$skins_dir"

	rm -f /usr/pluto/orbiter/skins
	ln -s "$skins_dir" /usr/pluto/orbiter/skins

	apt-get -y install rsync
	pushd "$skins_dir"
		rsync -avz --delete-after rsync://$rsync_skin_host/skins-public ./
	popd

	pushd /usr/pluto/orbiter/skins
		cp -r LinuxMCE/* Basic/
	popd

	pushd "${build_dir}"
		rm -f samba
		ln -s /home/samba
	popd
}

function ImportPublicMediaFromRSyncHost {
	mkdir -p '/home/samba/www_docs/sample media/'
	pushd '/home/samba/www_docs/sample media/'
		rsync -avz --delete-after rsync://$rsync_skin_host/sample-media ./
	popd
}

function DownloadIfNeeded {
	local host=$1
	local filepath=$2
	local filename=$3

	mkdir -p "$filepath"
	pushd "$filepath"
		md5_cur="$filepath"/$filename.md5.txt
		md5_last="$filepath"/$filename.md5.last.txt
		rm -f "$md5_cur"
	     	wget http://$host/build/$filename.md5.txt
		local md5c=`cat "$md5_cur"`
		if [ -e "$md5_last" ] ; then
			local md5l=`cat "$md5_last"`
			if [ x"$md5c" = x"$md5l" ] ; then
				return $(false)
			fi
		fi
		
		rm -f $filename
		wget http://$host/build/$filename
		md5d=`md5sum $filename`
		if [ x"$md5d" = x"$md5c" ] ; then
		    return $(true)
		fi

		mv "$md5_cur" "$md5_last"
	popd
        return $(false)
}

function ImportPublicSkinFromHTTPHost {
	if DownloadIfNeeded $http_skin_host "/home/samba/www_docs/" "graphics.tbz" ; then
		pushd "/home/samba/www_docs/"
			rm -rf "graphics"
			tar xvfj "graphics.tbz"
		popd
		mkdir -p /usr/pluto/orbiter/
		ln -s "/home/samba/www_docs/graphics" /usr/pluto/orbiter/skins
		pushd /usr/pluto/orbiter/skins
			cp -r LinuxMCE/* Basic/
		popd
		pushd "${build_dir}"
			rm -f samba
			ln -s /home/samba samba
		popd
	fi
}

function ImportPublicMediaFromHTTPHost {
	if DownloadIfNeeded $http_media_host "/home/samba/www_docs/" "sample_media.tbz" ; then
		pushd "/home/samba/www_docs/"
			rm -rf "sample media"
			tar xvfj "sample_media.tbz"
		popd
	fi
}

function IsReachable {
	local host=$1
	if [ x"$2" != x"" ] ; then
	    echo "Checking if $2 ($host) is reachable"
	fi
	local tmp=`ping -c 5 -i 0.2 $host  | grep 0\ received` || true
	if [ x"$tmp" = x"" ] ; then
	    return $(true)
	fi
	return $(false)
}

# Import skin
if IsReachable $rsync_skin_host "rsync skin host" ; then
	ImportPublicSkinFromRSyncHost
elif IsReachable $http_skin_host_reachable "http skin host" ; then
	ImportPublicSkinFromHTTPHost
else
	echo "Failed to download skins."
	echo "If you don't have them already, cd2-build.sh will fail."
fi

# Import media
if IsReachable $rsync_media_host "rsync media host" ; then
	ImportPublicMediaFromRSyncHost
elif IsReachable $http_media_host "http media host" ; then
	ImportPublicMediaFromHTTPHost
else
	echo "Failed to download media."
	echo "If you don't have them already, cd2-build.sh will fail."
fi

# Download videos
if IsReachable $http_media_host "http media host" ; then
	if DownloadIfNeeded $http_media_host "/var/lmce-build/extras/" video-wizard-videos.tar ; then
		pushd /var/lmce-build/extras/
			rm -rf video-wizard-videos
			tar xf video-wizard-videos.tar
			pushd video-wizard-videos
				dpkg-deb -b . ..
			popd
		popd
	fi
fi
