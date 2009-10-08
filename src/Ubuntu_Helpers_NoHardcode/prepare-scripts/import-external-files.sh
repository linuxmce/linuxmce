#!/bin/bash

set -e

echo ""
echo "********************************************************************************"
echo "*** Running: $0"
echo "********************************************************************************"

rsync_skin_host="10.0.2.3"
rsync_media_host="10.0.2.3"

http_skin_host="download.linuxmce.org"
http_media_host="download.linuxmce.org"

# needs a better name
base_dir=/home/samba/www_docs
extra_dir=/var/lmce-build/extras

. /etc/lmce-build/builder.conf

function ImportPublicSkinFromRSyncHost {
	local skins_dir=${base_dir}/graphics
# we don't want this on a lmce machine
#	mkdir -p /usr/pluto/orbiter/
	mkdir -p "$skins_dir"

# we don't want this on a lmce machine
# get MakeRelease to take them from "$skins_dir" instead
#	rm -f /usr/pluto/orbiter/skins
#	ln -s "$skins_dir" /usr/pluto/orbiter/skins

	apt-get -y install rsync
	pushd "$skins_dir"
		rsync -avz --delete-after rsync://$rsync_skin_host/skins-public ./
	popd

# we don't want this on a lmce machine
# get MakeRelease to take them from "$skins_dir" instead
#	pushd /usr/pluto/orbiter/skins
	pushd "$skins_dir"
		cp -r LinuxMCE/* Basic/
	popd

	pushd "${build_dir}"
		rm -f samba
		ln -s /home/samba
	popd
}

function ImportPublicMediaFromRSyncHost {
	mkdir -p "${base_dir}/sample media/"
	pushd "${base_dir}/sample media/"
		rsync -avz --delete-after rsync://$rsync_skin_host/sample-media ./
	popd
}



function DownloadIfNeeded
{
	local host=$1
	local filepath=$2
	local filename=$3

	local md5_old=""
	local md5_cur=""
	local md5_new=""


	mkdir -p "$filepath"
	pushd "$filepath"


		# get old md5sum from file or calculate from file
		if [ -e "${filename}.md5.txt" ]
		then
			md5_old=$(cat "${filename}.md5.txt")

		elif [ -e "${filename}" ]
		then
			md5_old=$(md5sum ${filename})
		fi

		# get current md5sum file
		rm -f "${filename}.md5.txt"
	     	wget -q http://$host/build/${filename}.md5.txt
		md5_cur=$(cat "${filename}.md5.txt")

		if [ "${md5_old}" != "${md5_cur}" ]
		then
			# move old file out of the way
			if [ -e "${filename}" ]
			then
				mv -fv "${filename}" "${filename}.last"
			fi

			# download new file
			wget http://$host/build/${filename}

			# calculate new md5sum from file
			md5_new=$(md5sum ${filename})

			if [ "${md5_cur}" = "${md5_new}" ]
			then
				# remove old file
				rm -fv "${filename}.last"
			else
				echo "WARNING: Downloaded file: http://$host/build/${filename} does not match md5sum."
				rm -f "${filename}.md5.txt"

				# restore old file
				if [ -e "${filename}.last" ]
				then
					mv -fv "${filename}.last" "${filename}"
					md5_cur="${md5_old}"
				fi
			fi
		fi


	popd

	return $(test "${md5_old}" != "${md5_cur}")
}


function ImportPublicSkinFromHTTPHost {
	local skins_dir="${base_dir}/graphics"

	if DownloadIfNeeded $http_skin_host "${base_dir}" "graphics.tbz"
	then
		echo "Unpacking graphics.tbz in ${base_dir}"
		pushd "${base_dir}"
			rm -rf "graphics"
			tar xjf "graphics.tbz"
		popd

# we don't want this on a lmce machine
# get MakeRelease to take them from "$skins_dir" instead
#		mkdir -p /usr/pluto/orbiter/
#		ln -s "/home/samba/www_docs/graphics" /usr/pluto/orbiter/skins
#		pushd /usr/pluto/orbiter/skins
		echo "Copying LinuxMCE skins to ${skins_dir}/Basic/"
		pushd "${skins_dir}"
			mkdir -p Basic
			cp -r LinuxMCE/* Basic/
		popd

		pushd "${build_dir}"
			rm -f samba
			ln -sv /home/samba samba
		popd
	fi
}


function ImportPublicMediaFromHTTPHost {
	if DownloadIfNeeded $http_media_host "${base_dir}" "sample_media.tbz" 
	then
		echo "Unpacking sample_media.tbz"
		pushd "${base_dir}"
			rm -rf "sample media"
			tar xjf "sample_media.tbz"
		popd
	fi
}

function ImportWizardVideosFromHTTPHost {
	if DownloadIfNeeded "$http_media_host" "${extra_dir}" video-wizard-videos.tar
	then
		echo "Unpacking video-wizard-videos.tar"
		pushd ${extra_dir}
			rm -rf video-wizard-videos
			tar xf video-wizard-videos.tar
			echo "Creating video-wizard-videos debian package"
			pushd video-wizard-videos
				dpkg-deb -b . ..
			popd
		popd
	fi
}

function ImportAVWizardSoundsFromHTTPHost {
	if DownloadIfNeeded "$http_media_host" "${extra_dir}" pluto-avwizard-sounds_1.1.tar.bz2
	then
		echo "Unpacking pluto-avwizard-sounds_1.1.tar.bz2"
		pushd ${extra_dir}
			rm -rf pluto-avwizard-sounds
			tar xjf pluto-avwizard-sounds_1.1.tar.bz2
			echo "Creating pluto-avwizard-sounds debian package"
			pushd pluto-avwizard-sounds
				dpkg-deb -b . ..
			popd
		popd
	fi
}

function IsReachable {
	local host=$1
	echo "Checking if $2 ($host) is reachable"
	local tmp=`ping -c 5 -i 0.2 $host  | grep '[^0] received'` || true
	if [ x"$tmp" != x"" ] ; then
	    return $(true)
	fi
	return $(false)
}


# Import skin
if IsReachable "$rsync_skin_host" "rsync skin host" ; then
	ImportPublicSkinFromRSyncHost
elif IsReachable "$http_skin_host" "http skin host" ; then
	ImportPublicSkinFromHTTPHost
else
	echo "Failed to download skins."
	echo "If you don't have them already, cd2-build.sh will fail."
fi

# Import media
if IsReachable "$rsync_media_host" "rsync media host" ; then
	ImportPublicMediaFromRSyncHost
elif IsReachable "$http_media_host" "http media host" ; then
	ImportPublicMediaFromHTTPHost
else
	echo "Failed to download media."
	echo "If you don't have them already, cd2-build.sh will fail."
fi


if IsReachable "$http_media_host" "http media host"
then
	# Import videos
	ImportWizardVideosFromHTTPHost

	# Import sounds
	ImportAVWizardSoundsFromHTTPHost
else
	echo "Failed to download videos and sounds."
	echo "If you don't have them already, the build will likely fail."
fi


echo "*** Done: $0"

