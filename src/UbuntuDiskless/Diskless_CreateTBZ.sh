#!/bin/bash

###########################################################
### Setup global variables
###########################################################
DISTRO="$(lsb_release -c -s)"
DEVICEDATA_Architecture=112
Architecture=$(apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g')
#Assign latest installed kernel and not running kernel
KVER=`ls /lib/modules/ --sort time|head -1`
DBST_SCRIPT='/usr/pluto/bin/Diskless_DebootstrapPluto.sh'
ARH_DIR='/usr/pluto/install'
DisklessFS="PlutoMD-${Architecture}.tar.bz2"
log_file=/var/log/pluto/Diskless_MD_Creation_$(date +%Y%m%d_%H%M%S).log
TEMP_DIR=$(mktemp -d /tmp/Diskless_CreateTBZ.XXXXXXXXX)
MYSQL_DB_CRED=""
SQL_DB="pluto_main"


###########################################################
### Setup Functions - Error checking and logging and trapping
###########################################################

Trap_Exit () {
	umount -fl $TEMP_DIR/var/cache/apt
	umount -fl $TEMP_DIR/usr/pluto/deb-cache
	umount -fl $TEMP_DIR/proc
	umount -fl $TEMP_DIR/sys
	umount -fl $TEMP_DIR/lib/modules/${KVER}/volatile

	rm -rf $TEMP_DIR
}

Setup_Logfile () {
mkdir -p /var/log/pluto
if [ ! -f ${log_file} ]; then
	touch ${log_file}
	if [ $? = 1 ]; then
		echo "`date` - Unable to write to ${log_file} - re-run script as root"
		exit 1
	fi
else
	#zero out an existing file
	echo > ${log_file}
fi
TeeMyOutput --outfile ${log_file} --stdboth --append -- "$@"
VerifyExitCode "Log Setup"
echo "`date` - Logging initiatilized to ${log_file}"
}

VerifyExitCode () {
        local EXITCODE=$?
        if [ "$EXITCODE" != "0" ] ; then
        	echo "An error (Exit code $EXITCODE) occured during the last action"
        	echo "$1"
                exit 1
        fi
}

TeeMyOutput () {

# Usage:
# source TeeMyOutput.sh --outfile <file> [--infile <file>] [--stdout|--stderr|--stdboth] [--append] [--exclude <egrep pattern>] -- "$@"
#   --outfile <file>         the file to tee our output to
#   --infile <file>          the file to feed ourselves with on stdin
#   --stdout                 redirect stdout (default)
#   --stderr                 redirect stderr
#   --stdboth                redirect both stdout and stderr
#   --append                 run tee in append mode
#   --exclude <pattern>      strip matching lines from output; pattern is used with egrep
#
# Environment:
#   SHELLCMD="<shell command>" (ex: bash -x)

if [[ -n "$TeeMyOutput" ]]; then
        return 0
fi
Me="TeeMyOutput"

# parse parameters
for ((i = 1; i <= "$#"; i++)); do
        Parm="${!i}"
        case "$Parm" in
                --outfile) ((i++)); OutFile="${!i}" ;;
                --infile) ((i++)); InFile="${!i}" ;;
                --stdout|--stderr|--stdboth) Mode="${!i#--std}" ;;
                --append) Append=yes ;;
                --exclude) ((i++)); Exclude="${!i}" ;;
                --) LastParm="$i"; break ;;
                *) echo "$Me: Unknown parameter '$Parm'"; exit 1
        esac
done

if [[ -z "$OutFile" ]]; then
        echo "$Me: No outfile"
        exit 1
fi

if [[ -z "$LastParm" ]]; then
        LastParm="$#"
fi

# original parameters
for ((i = "$LastParm" + 1; i <= "$#"; i++)); do
        OrigParms=("${OrigParms[@]}" "${!i}")
done

# construct command components
case "$Mode" in
        out) OurRedirect=() ;;
        err) OurRedirect=("2>&1" "1>/dev/null") ;;
        both) OurRedirect=("2>&1") ;;
esac

if [[ "$Append" == yes ]]; then
        TeeParm=(-a)
fi

if [[ -n "$InFile" ]]; then
        OurRedirect=("${OurRedirect[@]}" "<$InFile")
fi

# do our stuff
export TeeMyOutput=yes
ExitCodeFile="/tmp/TeeMyOutputExitCode_$$"
trap "rm -rf '$ExitCodeFile'" EXIT

Run()
{
        eval exec "${OurRedirect[@]}"
        $SHELLCMD "$0" "${OrigParms[@]}"
        echo $? >"$ExitCodeFile"
}

if [[ -z "$Exclude" ]]; then
        Run | tee "${TeeParm[@]}" "$OutFile"
else
        Run | grep --line-buffered -v "$Exclude" | tee "${TeeParm[@]}" "$OutFile"
fi

ExitCode=$(<"$ExitCodeFile")
exit "$ExitCode"
exit 1 # just in case


}


###########################################################
### Setup Functions - Reference functions
###########################################################

StatsMessage () {
	printf "`date` - $* \n"
}

RunSQL () {
#In order to remove the includes, all we need is the RunSQL function
mysql -A -N "$SQL_DB" $MYSQL_DB_CRED -e "$1;" | tr '\n\t ' $'\x20'$'\x01'$'\x02' | sed 's/ *$//'
}

Field () {
local Row FieldNumber
FieldNumber="$1"; shift
Row="$*"
echo "$Row" | cut -d$'\x01' -f"$FieldNumber" | tr $'\x02' ' '
}


###########################################################
### Setup Functions - General functions
###########################################################

MD_Create_And_Populate_Temp_Dir () {
StatsMessage "Creating Necessary files for MD creation to:"
#Create the temp dir used for MD Staging
if [[ -r /usr/pluto/install/PlutoMD_Debootstraped.tar.bz2 ]] ;then
	pushd $TEMP_DIR
		StatsMessage "Untarring $ARH_DIR/PlutoMD_Debootstraped.tar.bz2"
		tar -xf "$ARH_DIR"/PlutoMD_Debootstraped.tar.bz2
	popd
else
	StatsMessage "Creating debootstrap"
	debootstrap "$(lsb_release -c -s)" "$TEMP_DIR" "http://archive.ubuntu.com/ubuntu/"
fi
}

MD_System_Level_Prep () {
StatsMessage "Preparing the diskless system"
## Set up chroot installation environment

# Make sure, the root user is connecting to DCEROUTER for any MySQL connection
echo "[client]
host = dcerouter
" > "$TEMP_DIR"/root/.my.cnf

## Disable invoke-rc.d scripts
mv "$TEMP_DIR"/sbin/start-stop-daemon{,.pluto-install}
mv "$TEMP_DIR"/sbin/initctl{,.pluto-install}
echo -en '#!/bin/bash\necho "WARNING: we dont want invoke-rc.d to run right now"\nexit 101\n' >"$TEMP_DIR"/usr/sbin/policy-rc.d
echo -en '#!/bin/bash\necho "WARNING: fake start-stop-daemon called"\n' >"$TEMP_DIR"/sbin/start-stop-daemon
echo -en '#!/bin/bash\necho "WARNING: fake initctl called"\n' >"$TEMP_DIR"/sbin/initctl
chmod +x "$TEMP_DIR"/usr/sbin/policy-rc.d
chmod +x "$TEMP_DIR"/sbin/start-stop-daemon
chmod +x "$TEMP_DIR"/sbin/initctl

## Setup initial resolv.conf
cp /etc/resolv.conf "$TEMP_DIR"/etc/resolv.conf

## Enable some needed mount points
mkdir -p $TEMP_DIR/var/cache/apt
mkdir -p $TEMP_DIR/usr/pluto/deb-cache
mount --bind /var/cache/apt $TEMP_DIR/var/cache/apt
mount --bind /usr/pluto/deb-cache $TEMP_DIR/usr/pluto/deb-cache
mount none -t sysfs $TEMP_DIR/sys
mount none -t proc $TEMP_DIR/proc


## Setup apt in pluto style
## FIXME: maybe we need to make sources.list from scratch ?
cp {,"$TEMP_DIR"}/etc/apt/sources.list
cp -r /etc/apt/sources.lis* $TEMP_DIR/etc/apt 

[[ -f /etc/apt/apt.conf.d/30pluto ]] && cp {,"$TEMP_DIR"}/etc/apt/apt.conf.d/30pluto
[[ -f /etc/apt/preferences ]] && cp {,"$TEMP_DIR"}/etc/apt/preferences
[[ -f /etc/apt/apt.conf ]] && cp {,"$TEMP_DIR"}/etc/apt/apt.conf

## Setup initial ssh access    
StatsMessage "Setting up SSH"
[[ -f /usr/pluto/keys/id_dsa_pluto.pub ]] && mkdir -p "$TEMP_DIR"/root/.ssh && cat /usr/pluto/keys/id_dsa_pluto.pub >> "$TEMP_DIR"/root/.ssh/authorized_keys
}

MD_Seamless_Compatability () {
######################
#Foxconn NT330i
######################
StatsMessage "Setting up Foxconn NT330i compatability"
if ! grep atl1c $TEMP_DIR/etc/modules >/dev/null; then
	#White space should not be present in the modules files, you MUST remove for the MD to boot properly
	sed -i '/^$/d' $TEMP_DIR/etc/modules
	echo atl1c>>$TEMP_DIR/etc/modules
fi

if ! grep atl1c $TEMP_DIR/etc/initramfs-tools/modules >/dev/null; then
	#White space should not be present in the modules files, you MUST remove for the MD to boot properly
	sed -i '/^$/d' $TEMP_DIR/etc/initramfs-tools/modules
	echo atl1c>>$TEMP_DIR/etc/initramfs-tools/modules
fi

if ! grep atl1c /etc/initramfs-tools-interactor/modules >/dev/null; then
	echo atl1c>>/etc/initramfs-tools-interactor/modules
	VerifyExitCode "insertion of atl1c to modules file failed"
	#White space should not be present in the modules files, you MUST remove for the MD to boot properly
	sed -i '/^$/d' /etc/initramfs-tools-interactor/modules
	modprobe atl1c
	VerifyExitCode "Modprobe for atl1c failed"
fi

######################
#End Foxconn NT330i
######################

}

MD_Preseed () {
## Setup debconf interface to 'noninteractive'
StatsMessage "PreSeeding package installation preferences"

LC_ALL=C chroot $TEMP_DIR apt-get -y -qq update
VerifyExitCode "apt update"

#Setup the medibuntu repo
LC_ALL=C chroot $TEMP_DIR apt-get -y --force-yes install medibuntu-keyring 
VerifyExitCode "medibuntu apt add keyring"

LC_ALL=C chroot $TEMP_DIR apt-get -y --force-yes install debconf-utils
VerifyExitCode "install of debconf-utils"

#create preseed file
echo "debconf debconf/frontend	select Noninteractive
# Choices: critical, high, medium, low
debconf debconf/priority        select critical
msttcorefonts   msttcorefonts/http_proxy        string
msttcorefonts   msttcorefonts/defoma    note
msttcorefonts   msttcorefonts/dlurl     string
msttcorefonts   msttcorefonts/savedir   string
msttcorefonts   msttcorefonts/baddldir  note
msttcorefonts   msttcorefonts/dldir     string
msttcorefonts   msttcorefonts/blurb     note
msttcorefonts   msttcorefonts/accepted-mscorefonts-eula boolean true
msttcorefonts   msttcorefonts/present-mscorefonts-eula  boolean false
sun-java6-bin   shared/accepted-sun-dlj-v1-1    boolean true
sun-java6-jre   shared/accepted-sun-dlj-v1-1    boolean true
sun-java6-jre   sun-java6-jre/jcepolicy note
sun-java6-jre   sun-java6-jre/stopthread        boolean true
debconf debconf/frontend        select  Noninteractive
# Choices: critical, high, medium, low
debconf debconf/priority        select  critical
" > $TEMP_DIR/tmp/preseed.cfg

LC_ALL=C chroot $TEMP_DIR debconf-set-selections /tmp/preseed.cfg
VerifyExitCode "debconf-set-selections - preseed data"

#For some odd reason, set-selections adds a space for Noninteractive and Critical that needs to be removed - debconf doesn't handle extra white space well
sed -i 's/Value:  /Value: /g' $TEMP_DIR/var/cache/debconf/config.dat

#remove preseed file, no need to clutter things up
rm $TEMP_DIR/tmp/preseed.cfg


}

MD_Install_Packages () {
StatsMessage "Installing packages to MD"

## Update the chrooted system (needed when created from archive)
LC_ALL=C chroot $TEMP_DIR apt-get -f -y install
LC_ALL=C chroot $TEMP_DIR apt-get -f -y dist-upgrade
VerifyExitCode "Dist-upgrade failed"


#Install headers and run depmod for the seamless integraiton function, ensure no errors exist
LC_ALL=C chroot $TEMP_DIR apt-get -y install linux-headers-generic
VerifyExitCode "Install linux headers package failed"
KVER=`ls $TEMP_DIR/lib/modules/ --sort time|head -1`
LC_ALL=C chroot $TEMP_DIR depmod -v $KVER
VerifyExitCode "depmod failed - ensure you run an apt-get dist-upgrade on your core before running again"

# generate locales
StatsMessage "Generating locales"
echo "en_US.UTF-8 UTF-8" >"$TEMP_DIR"/etc/locale.gen
LC_ALL=C chroot "$TEMP_DIR" apt-get -f -y install locales

StatsMessage "Installing kernel"
LC_ALL=C chroot "$TEMP_DIR" apt-get -f -y --no-install-recommends install linux-image-generic


########## START CREATE LIST OF DEVICES #################"
## that will run on the md so we will know
## what software to preinstall there
## FIXME: get this list from the database 
DEVICE_LIST="28 62 1759 5 11 1825 26 1808 1901"

# Determine if MythTV is installed by looking for MythTV_Plugin...
Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=36"
MythTV_Installed=$(RunSQL "$Q")
if [ $MythTV_Installed ];then
	#MythTV_Plugin is installed, so install MythTV_Player on MD
	DEVICE_LIST="$DEVICE_LIST 35"
fi
########## END CREATE LIST OF DEVICES ###################"


## Prevent lpadmin from running as it blocks the system
LC_ALL=C chroot "$TEMP_DIR" apt-get -y install cupsys-client
cp "$TEMP_DIR"/usr/sbin/lpadmin{,.disabled}
echo > "$TEMP_DIR"/usr/sbin/lpadmin 
LC_ALL=C chroot "$TEMP_DIR" apt-get -y install cups-pdf
mv "$TEMP_DIR"/usr/sbin/lpadmin{.disabled,}


## Prevent discover from running as it blocks the system
LC_ALL=C chroot "$TEMP_DIR" apt-get -y install discover
cp "$TEMP_DIR"/sbin/discover "$TEMP_DIR"/sbin/discover.disabled
echo > "$TEMP_DIR"/sbin/discover

## Begin installing the packages needed for the pluto devices
StatsMessage "Prep for MCE device installation"
echo "do_initrd = Yes" > $TEMP_DIR/etc/kernel-img.conf

for device in $DEVICE_LIST; do
	Q="SELECT
		Package_Source.Name
	   FROM
		Package_Source
		JOIN DeviceTemplate on Package_Source.FK_Package = DeviceTemplate.FK_Package
	   WHERE
		PK_DeviceTemplate = $device
	   AND
		FK_RepositorySource = 2					
	"

	R=$(RunSQL "$Q")
	VerifyExitCode "Connecting to MYSQL DB failed"
	
	for Row in $R; do
		pkg_name=$(Field 1 "$Row")
	done

	StatsMessage "Installing $pkg_name for device $device"
	RETURNCODE=1
	while [[ $RETURNCODE != 0 ]]; do
		LC_ALL=C chroot $TEMP_DIR apt-get -y install $pkg_name
		if [[ "$?" == "0" ]] ;then
			StatsMessage "Package $pkg_name installed ok!"
			RETURNCODE=0
		else
			StatsMessage "#### Package $pkg_name failed ($TEMP_DIR) - We wait 10sec and try again - to stop retrying press Ctrl-C. "
			sleep 10
		fi
	done
done

StatsMessage "Install other ancillary programs for MCE"
## Packages that are marked as dependencies only in the database
LC_ALL=C chroot $TEMP_DIR apt-get -y install id-my-disc

## Put back discover
mv "$TEMP_DIR"/sbin/discover.disabled "$TEMP_DIR"/sbin/discover

## If libdvdcss2 is installed on the hybrid/core
if [[ -d /usr/share/doc/libdvdcss2 ]] ;then
	pushd $TEMP_DIR >/dev/null
	chroot $TEMP_DIR apt-get install libdvdcss2
	VerifyExitCode "Installation of libdvdcss2 failed"
	popd
fi

LC_ALL=C chroot $TEMP_DIR apt-get -y install kubuntu-desktop
echo '/bin/false' >"$TEMP_DIR/etc/X11/default-display-manager"
# Update startup to remove kdm and network manager
LC_ALL=C chroot $TEMP_DIR update-rc.d -f kdm remove
LC_ALL=C chroot $TEMP_DIR update-rc.d -f NetworkManager remove

#implement external_media_identifier fix
LC_ALL=C chroot $TEMP_DIR ln -s /usr/lib/libdvdread.so.4 /usr/lib/libdvdread.so.3

#Install ancillary programs
LC_ALL=C chroot $TEMP_DIR apt-get -y install xserver-xorg-video-all linux-firmware
VerifyExitCode "Ancillary programs install failed"

# Install plymouth theme on MD in Lucid
if [[ "$DISTRO" = "lucid" ]] ; then
	LC_ALL=C chroot $TEMP_DIR apt-get -y install lmce-plymouth-theme
	VerifyExitCode "MCE plymouth theme install failed"
fi

#Install backported alsa modules for HDMI audio for legacy hardware
#tkmedia mentioned there are conflicts with this and capture cards using V4L, disabling for now
#if [[ "$DISTRO" = "lucid" ]] ; then
	#LC_ALL=C chroot $TEMP_DIR apt-get -y install linux-backports-modules-alsa-lucid-generic
	#VerifyExitCode "Alsa backport modules install failed for HDMI"
#fi

}

MD_Cleanup () {
StatsMessage "Cleaning up from package installations..."
umount $TEMP_DIR/var/cache/apt
umount $TEMP_DIR/usr/pluto/deb-cache
umount $TEMP_DIR/sys
umount $TEMP_DIR/proc
umount $TEMP_DIR/lib/modules/${KVER}/volatile

#Copy the packages.gz file to ensure apt-get update does not fail
mkdir -p $TEMP_DIR/usr/pluto/deb-cache/
cp -p /usr/pluto/deb-cache/Packages.gz $TEMP_DIR/usr/pluto/deb-cache/

mv -f "$TEMP_DIR"/sbin/start-stop-daemon{.pluto-install,}
mv -f "$TEMP_DIR"/sbin/initctl{.pluto-install,}
rm -f "$TEMP_DIR"/usr/sbin/policy-rc.d


#Copy the orbiter activation command to the MD's desktop
cp -r "$TEMP_DIR"/etc/skel/Desktop/* "$TEMP_DIR"/root/Desktop

#Remove the xorg file(s) from installation, need to start with a fresh slate
rm -f "$TEMP_DIR"/etc/X11/xorg.conf*

#Clean up apt from the installs
LC_ALL=C chroot "$TEMP_DIR" apt-get clean
VerifyExitCode "APT Clean"

#Create a list of installed packages
COLUMNS=1024 chroot "$TEMP_DIR" dpkg -l | awk '/^ii/ {print $2}' >/tmp/pkglist-diskless.txt # used for deb-cache cleanup in the builder

}

Create_Diskless_Tar () {
StatsMessage "Creating the compressed tar image file, this will take up to 1 hour depending on your system..."
mkdir -p "$ARH_DIR"
pushd "$TEMP_DIR" >/dev/null
tar -cjf "$ARH_DIR/$DisklessFS" *
VerifyExitCode "create tar file failed"
echo "$PlutoVersion" > "$ARH_DIR/$DisklessFS.version"
popd >/dev/null

rm -rf $TEMP_DIR

# HACK'O'MATIC And make sure, we have files for both architectures.
if [ -f /usr/pluto/install/PlutoMD-i386.tar.bz2 ]; then
	FILENEEDED=PlutoMD-amd64
else
	FILENEEDED=PlutoMD-i386
fi
if [ ! -f /usr/pluto/install/$FILENEEDED.tar.bz2 ]; then
	ln -sf /usr/pluto/install/$DisklessFS /usr/pluto/install/$FILENEEDED.tar.bz2
fi	
if [ ! -f /usr/pluto/install/$FILENEEDED.tar.bz2.version ]; then
	ln -sf /usr/pluto/install/$DisklessFS.version /usr/pluto/install/$FILENEEDED.tar.bz2.version
fi	

}

Create_PXE_Initramfs_Vmlinuz () {
StatsMessage "Building the initial initramfs and vmlinuz files for PXE booting"
# Let's create the default files needed for successful PXE boot.
/usr/pluto/bin/Diskless_BuildDefaultImage.sh
VerifyExitCode "PXE vmlinuz and initramfs"
}

###########################################################
### Main execution area
###########################################################

#Setup trap
trap "Trap_Exit" EXIT

#Set up logging
Setup_Logfile

#Function execution
MD_Create_And_Populate_Temp_Dir
MD_System_Level_Prep
MD_Seamless_Compatability
MD_Preseed
MD_Install_Packages
MD_Cleanup
Create_Diskless_Tar
Create_PXE_Initramfs_Vmlinuz

#Disable trap, everything was umounted and removed in the cleanup function
trap - EXIT

StatsMessage "Diskless media director image setup completed without a detected issue!"

#Exit successfully
exit 0

