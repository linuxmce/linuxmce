#!/bin/bash


DISTRO="$(lsb_release -c -s)"
# COMPOS="lmce-alpha1 lmce-alpha1-updates"
COMPOS="lmce-alpha-latest lmce-alpha-latest-updates"

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



function InstallNeededPackages
{
	apt-get update
	apt-get -y install dpkg-dev || error "installing dpkg-dev"

	mkdir -pv "${LOCAL_REPO_BASE}/${LOCAL_REPO_DIR}"

}

function AddAptRetries
{
        local changed
                
                 
        if ! grep -q "^[^#]*Acquire::Retries" /etc/apt/apt.conf
        then
           echo 'Acquire::Retries "0"'>>/etc/apt/apt.conf
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


function CopyDebsFromIso
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



