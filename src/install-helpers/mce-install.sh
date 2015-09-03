#!/bin/bash

TARGET_DISTRO=$(lsb_release -i -s | tr '[:upper:]' '[:lower:]')
TARGET_RELEASE=$(lsb_release -c -s)
REPO="main"

echo >>/etc/apt/sources.list
echo "deb http://deb.linuxmce.org/${TARGET_DISTRO}/ ${TARGET_RELEASE} ${REPO}" >>/etc/apt/sources.list

apt-get -q update
DEBIAN_FRONTEND="noninteractive" apt-get -y -q --allow-unauthenticated install lmce-hybrid | tee $(basename $0 | cut -d'.' -f1).log
