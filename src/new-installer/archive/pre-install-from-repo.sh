#!/bin/bash

source pre-install-common.sh

AddAptRetries

InstallAptCacherNg
ConfigureApt

InstallNeededPackages
CreatePackagesFiles

# Make sure sources.conf has EOL at EOF
echo>>/etc/apt/sources.conf

AddRepoToSourcesTop "file:${LOCAL_REPO_BASE} ${LOCAL_REPO_DIR}"
AddRepoToSources "http://deb.linuxmce.org/ubuntu/ ${DISTRO}  ${COMPOS}"
AddRepoToSources "http://debian.slimdevices.com/ stable  main"

# Check where our distro is hosted
DISTRO_HOST=
wget http://archive.ubuntu.com/ubuntu/dists/${DISTRO}/main/binary-i386/Packages.gz && DISTRO_HOST=archive
wget http://old-releases.ubuntu.com/ubuntu/dists/${DISTRO}/main/binary-i386/Packages.gz && DISTRO_HOST=old-releases
AddRepoToSources "http://${DISTRO_HOST}.ubuntu.com/ubuntu ${DISTRO} main restricted universe multiverse"
AddRepoToSources "http://${DISTRO_HOST}.ubuntu.com/ubuntu ${DISTRO}-security main restricted universe multiverse"
AddRepoToSources "http://archive.canonical.com/ubuntu ${DISTRO} partner"

if AddRepoToSources "http://packages.medibuntu.org/ ${DISTRO}  free non-free"
then
	AddGpgKeyToKeyring http://packages.medibuntu.org/medibuntu-key.gpg
fi

# AddRepoToSources "http://www.avenard.org/files/ubuntu-repos intrepid release"
# AddGpgKeyToKeyring  http://www.avenard.org/files/ubuntu-repos/ubuntu-repos.key


apt-get update
