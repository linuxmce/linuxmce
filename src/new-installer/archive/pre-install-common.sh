#!/bin/bash


DISTRO="$(lsb_release -c -s)"
# COMPOS="lmce-alpha1 lmce-alpha1-updates"
COMPOS="lmce-alpha-latest lmce-alpha-latest-updates"
COMPOS="beta2"
LOCAL_REPO_BASE=/usr/pluto/deb-cache
LOCAL_REPO_DIR=./


if [ ! -e /etc/apt/sources.list.pbackup ]
then
	cp -av /etc/apt/sources.list /etc/apt/sources.list.pbackup
fi



function error
{
	echo "$0: Error $1"
	exit 1
}

function VerifyExitCode
{
        EXITCODE=$?
        if [ "$EXITCODE" != "0" ] ; then
        	echo "An error (Exit code $EXITCODE) occured during the last action"
        	echo "$1"
                exit 1
        fi
}

function InstallNeededPackages
{
	apt-get update
	apt-get -y --force-yes install dpkg-dev
	VerifyExitCode "dpkg-dev"

	mkdir -pv "${LOCAL_REPO_BASE}/${LOCAL_REPO_DIR}"

	wget http://fpdownload.adobe.com/get/flashplayer/current/install_flash_player_10_linux.deb
	dpkg -i install_flash_player_10_linux.deb

}

function AddAptRetries
{
        local changed
                
                 
        if ! grep -q "^[^#]*APT::Acquire { Retries" /etc/apt/apt.conf
        then
           echo 'APT::Acquire { Retries  "20" }'>>/etc/apt/apt.conf
           changed=0
        else
           echo "Acquire::Retries already set."
           changed=1
        fi
        return $changed
}


function CopyDebsFromIso
{
	local isofile=$1
	local subdir=$2


	local mountpoint=`mktemp -d` || error "making up a mountpoint name."
	mkdir -pv "$mountpoint" || error "creating $mountpoint mountpoint directory"

	mount -v "$isofile" "$mountpoint" -o loop || error "mounting $isofile on $mountpoint"
	cp -av "${mountpoint}/${subdir}/"*.deb "${LOCAL_REPO_BASE}/${LOCAL_REPO_DIR}" || error "copying ${mountpoint}/${subdir}/*.deb to ${LOCAL_REPO_BASE}/${LOCAL_REPO_DIR}"
	umount -v $mountpoint || error "unmounting $mountpoint"

	rmdir -v $mountpoint || error "removing $mountpoint directory"

}


function CopyDebsFromDir
{
	local packages_dir=$1


	cp -av "${packages_dir}/"*.deb "${LOCAL_REPO_BASE}/${LOCAL_REPO_DIR}"

}



function CreatePackagesFiles
{

	( cd "${LOCAL_REPO_BASE}"; \
		dpkg-scanpackages -m "${LOCAL_REPO_DIR}" /dev/null | \
		tee "${LOCAL_REPO_DIR}/Packages" | \
		gzip -9c >"${LOCAL_REPO_DIR}/Packages.gz" )

}



function AddRepoToSourcesTop
{
        local repository="$1"
        local changed


        if ! grep -q "^[^#]*${repository}" /etc/apt/sources.list
        then
                sed -e "1ideb ${repository}" -i /etc/apt/sources.list
                changed=0
        else
                sed -e "/${repository}/d" -i /etc/apt/sources.list
                sed -e "1ideb ${repository}" -i /etc/apt/sources.list
                echo "Repository ${repository} already active, moved to top"
                changed=1
        fi
}



function AddRepoToSources
{
	local repository="$1"
	local changed


	if ! grep -q "^[^#]*${repository}" /etc/apt/sources.list
	then
		echo "deb ${repository}" >>/etc/apt/sources.list
		changed=0
	else
		echo "Repository ${repository} seems already active"
		changed=1
	fi

	return $changed
}



function AddGpgKeyToKeyring
{
	local gpg_key="$1"

	wget -q "$gpg_key" -O- | apt-key add -

}

function InstallAptCacherNg
{
    echo "Installing apt-cacher-ng"
    # this should be installed first of all.
#    apt-get -y install apt-cacher-ng
#    service apt-cacher-ng stop
#    service apt-cacher-ng start
}
 
function ConfigureApt
{
    echo "Configuring apt-cacher-ng"
#	apt-cacher seems to be the culprit for lots of problems for people who have flakey connections.
# 	therefore we disable apt-cacher for the time
#    echo 'Acquire::http { Proxy "http://localhost:3142"; };' > /etc/apt/apt.conf.d/02proxy
    apt-get update
}


