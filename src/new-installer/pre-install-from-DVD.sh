#!/bin/bash

source pre-install-common.sh


# PACKAGES_DIR=/media/kubuntu-8.10/var/www
AddAptRetries
InstallAptCacherNg
ConfigureApt

InstallNeededPackages

# CopyDebsFromDir "$PACKAGES_DIR"

CreatePackagesFiles

AddRepoToSourcesTop "file:${LOCAL_REPO_BASE} ${LOCAL_REPO_DIR}"
AddRepoToSources "http://deb.linuxmce.org/ubuntu/ ${DISTRO}  ${COMPOS}"
AddRepoToSources "http://debian.slimdevices.com/ stable  main"
AddRepoToSources "http://old-releases.ubuntu.com/ubuntu intrepid-security main restricted universe multiverse"
AddRepoToSources "http://old-releases.ubuntu.com/ubuntu intrepid main restricted universe multiverse"

#AddRepoToSources "http://packages.medibuntu.org/ ${DISTRO}  free non-free"
AddGpgKeyToKeyring http://packages.medibuntu.org/medibuntu-key.gpg

# AddRepoToSources "http://www.avenard.org/files/ubuntu-repos intrepid release"
# AddGpgKeyToKeyring  http://www.avenard.org/files/ubuntu-repos/ubuntu-repos.key
apt-get update

# We add all the deb's here, that can be pre-installed without causing problems
apt-get -y -f --force-yes install video-wizard-videos
apt-get -y -f --force-yes install pluto-sample-media

        
