#!/bin/bash

source pre-install-common.sh


InstallNeededPackages

CopyDebsFromIso "LinuxMCE-CD1.iso" "deb-cache"
CopyDebsFromIso "LinuxMCE-CD2.iso" "cachecd1-cache"
##CopyDebsFromIso "kubuntu-*-desktop-*.iso" "pool"

CreatePackagesFiles

AddRepoToSources "file:${LOCAL_REPO_BASE} ${LOCAL_REPO_DIR}"
AddRepoToSources "http://deb.linuxmce.org/ubuntu/ ${DISTRO}  ${COMPOS}"
AddRepoToSources "http://debian.slimdevices.com/ stable  main"

if AddRepoToSources "http://packages.medibuntu.org/ ${DISTRO}  free non-free"
then
	AddGpgKeyToKeyring http://packages.medibuntu.org/medibuntu-key.gpg
fi

apt-get update

