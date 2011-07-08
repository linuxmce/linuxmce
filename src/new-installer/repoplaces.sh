#!/bin/bash
#
# Replace sources to point to old-releases instead of archive.
#
#
repoclean() {
	release=`lsb_release -c|cut -f2`
	oldrelease="intrepid"
	if [[ "$release" != "$oldrelease" ]]; then
		return
	fi	
	n1=$(grep -e 'security.' -e 'archive.' /etc/apt/sources.list | wc -l) #looks for references that shouldn't be there and counts them
	o1=0
	if [ "$n1" -gt "$o1" ] ; then 
		echo "Changing repositories..."
		sed -i.backup -e 's/\/.*.archive/\/\/old-releases/' -e 's/security.ubuntu/old-releases.ubuntu/' -e 's/\/archive/\/old-releases/' /etc/apt/sources.list; #changes us.archive, security.ubuntu, and /archive to old-releases. 
		grep -v '^#' /etc/apt/sources.list |  cat > /etc/apt/sources.tmp #removes duplicates in temp file
		cat /etc/apt/sources.tmp > /etc/apt/sources.list; #rewrites sources.list without comments
	else 
		echo "Repositories clean"
        fi
	sort -u /etc/apt/sources.list > /etc/apt/sources.tmp; #removes duplicates in temp file
	cat /etc/apt/sources.tmp > /etc/apt/sources.list; #rewrites sources.list without duplicates
}

repoclean
echo "Running update..."
apt-get update  
repoclean
echo "Running dist-upgrade..."
sudo apt-get -yu --force-yes dist-upgrade
echo "Cleaning up..."
repoclean
rm /etc/apt/sources.tmp
apt-get autoclean
exit 0
