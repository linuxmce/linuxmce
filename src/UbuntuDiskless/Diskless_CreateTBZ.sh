#!/bin/bash

set -e
#set -x

#export PROXY="http://10.10.42.99:3142"
#export http_proxy="http://10.10.42.99:3142"

###########################################################
### Setup global variables
###########################################################
DEVICEDATA_Operating_System=209
DEVICEDATA_Architecture=112

###TARGET_TYPES="ubuntu-i386 ubuntu-amd64 raspbian-armhf" ### experimental
###TARGET_TYPES="ubuntu-i386 raspbian-armhf" ### experimental
#TARGET_TYPES="raspbian-armhf"
#TARGET_TYPES="ubuntu-amd64 ubuntu-i386"
#TARGET_TYPES="ubuntu-i386 ubuntu-amd64"
#TARGET_TYPES="ubuntu-amd64"
TARGET_TYPES="ubuntu-i386"

INSTALL_KUBUNTU_DESKTOP="yes"
#INSTALL_KUBUNTU_DESKTOP="no"

HOST_DISTRO=$(lsb_release -i -s | tr '[:upper:]' '[:lower:]')
HOST_RELEASE=$(lsb_release -c -s)
HOST_ARCH=$(apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)

#Assign latest installed kernel and not running kernel
HOST_KVER=$(find /lib/modules/* -maxdepth 0 -type d |sort -rV |head -1)

#host file locations
ARCHIVE_DIR='/usr/pluto/install'
log_file=/var/log/pluto/Diskless_MD_Creation_$(date +%Y%m%d_%H%M%S).log

#db credentials
MYSQL_DB_CRED=""
SQL_DB="pluto_main"

###########################################################
### Setup Functions - Error checking and logging and trapping
###########################################################

Trap_Exit () {
	PIDS=$(lsof ${TEMP_DIR} 2>/dev/null | awk '{print $2}' | tail -n+2 | uniq )
	until [[ -z "${PIDS}" ]] ; do
		kill ${PIDS}
		sleep 1
		PIDS=$(lsof ${TEMP_DIR} 2>/dev/null | awk '{print $2}' | tail -n+2 | uniq )
	done

	MNTS=$(mount | grep ${TEMP_DIR} | awk '{print $3;}' | sort -r)
	until [[ -z "${MNTS}" ]] ; do
		umount ${MNTS}
		sleep 1
		MNTS=$(mount | grep ${TEMP_DIR} | awk '{print $3;}' | sort -r)
	done

	rm -rf $TEMP_DIR
}

Setup_Logfile () {
	mkdir -p /var/log/pluto
	if [ ! -f ${log_file} ]; then
		touch ${log_file}
		if [ "$?" = 1 ]; then
			echo "$(date) - Unable to write to ${log_file} - re-run script as root"
			exit 1
		fi
	else
		#zero out an existing file
		echo > ${log_file}
	fi
	TeeMyOutput --outfile ${log_file} --stdboth --append -- "$@"
	VerifyExitCode "Log Setup"
	echo "$(date) - Logging initiatilized to ${log_file}"
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
	printf "$(date) - $* \n"
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

function do_debootstrap {
	local release_name="$1"
	local temp_dir="$2"
	local repository="$3"

	qemu_arch=""
	case "$TARGET_ARCH" in
		i386)
			qemu_arch="i386"
		;;
		amd64)
			qemu_arch="x86_64"
		;;
		armel|armhf)
			qemu_arch="arm"
		;;
	esac

	[[ -f $(which "qemu-${qemu_arch}-static") ]] && qemu_static_bin=$(which "qemu-${qemu_arch}-static")
	debootstrap --arch "$TARGET_ARCH" --foreign --no-check-gpg "$release_name" "$temp_dir" "$repository"
	mkdir -p "${temp_dir}/usr/bin"
	[[ -f "$qemu_static_bin" ]] && cp "$qemu_static_bin" "${temp_dir}/usr/bin"
	chroot "$temp_dir" /debootstrap/debootstrap --second-stage
}

MD_Create_And_Populate_Temp_Dir () {
	StatsMessage "Creating chroot for MD creation at: $TEMP_DIR"
	#Create the temp dir used for MD Staging
	if [[ -r "$ARCHIVE_DIR/$DBST_ARCHIVE" ]] ;then
		mkdir -p "$TEMP_DIR"
		pushd "$TEMP_DIR" >/dev/null
		StatsMessage "Untarring $ARCHIVE_DIR/$DBST_ARCHIVE"
		tar -xf "$ARCHIVE_DIR"/"$DBST_ARCHIVE"
		popd >/dev/null
	else
		StatsMessage "Creating debootstrap"
		do_debootstrap "$TARGET_RELEASE" "$TEMP_DIR" "$TARGET_REPO"
	fi

}

MD_System_Level_Prep () {
	StatsMessage "Preparing the diskless system"
	## Set up chroot installation environment

	# Make sure, the root user is connecting to DCEROUTER for any MySQL connection
	cat <<-EOF > $TEMP_DIR/root/.my.cnf
		[client]
		host = dcerouter
		EOF

	## Setup initial resolv.conf
	cp /etc/resolv.conf "$TEMP_DIR"/etc/resolv.conf

	## Enable some needed mount points
	if [[ "$TARGET_ARCH" == "$HOST_ARCH" ]]; then
		mkdir -p $TEMP_DIR/var/cache/apt
		mount --bind /var/cache/apt $TEMP_DIR/var/cache/apt
	fi

	mkdir -p /usr/pluto/deb-cache/$DEB_CACHE
	touch /usr/pluto/deb-cache/$DEB_CACHE/Packages
	gzip -9c < /usr/pluto/deb-cache/$DEB_CACHE/Packages > /usr/pluto/deb-cache/$DEB_CACHE/Packages.gz
	mkdir -p $TEMP_DIR/usr/pluto/deb-cache/$DEB_CACHE
	mount --bind /usr/pluto/deb-cache $TEMP_DIR/usr/pluto/deb-cache
	mount none -t devpts $TEMP_DIR/dev/pts
	mount none -t sysfs $TEMP_DIR/sys
	mount none -t proc $TEMP_DIR/proc

	## Setup apt in pluto style
	case "$TARGET_DISTRO" in
	#	"$HOST_DISTRO")
	#		StatsMessage "Setting up /etc/apt/sources.list for HOST_DISTRO: $TARGET_DISTRO"
	#		cp {,"$TEMP_DIR"}/etc/apt/sources.list
	#		#cp -R {,"$TEMP_DIR"}/etc/apt/sources.list.d/
	#		;;
		"ubuntu")
			StatsMessage "Setting up /etc/apt/sources.list for ubuntu"
			TARGET_REPO_NAME="main"
			cat <<-EOF > $TEMP_DIR/etc/apt/sources.list
				deb file:/usr/pluto/deb-cache/$DEB_CACHE ./
				deb http://deb.linuxmce.org/ubuntu/ $TARGET_RELEASE $TARGET_REPO_NAME
				#deb http://deb.linuxmce.org/ $TARGET_RELEASE $TARGET_REPO_NAME
				deb $TARGET_REPO $TARGET_RELEASE main restricted universe multiverse
				deb $TARGET_REPO $TARGET_RELEASE-updates main restricted universe multiverse
				deb $TARGET_REPO $TARGET_RELEASE-backports main restricted universe multiverse
				deb http://security.ubuntu.com/ubuntu/ $TARGET_RELEASE-security main restricted universe multiverse
				EOF
			;;
		"raspbian")
			StatsMessage "Setting up /etc/apt/sources.list for raspbian"
			cat <<-EOF > $TEMP_DIR/etc/apt/sources.list
				#deb http://10.10.42.99/raspbian-$TARGET_RELEASE/ ./
				deb file:/usr/pluto/deb-cache/$DEB_CACHE ./
				deb http://deb.linuxmce.org/raspbian/ $TARGET_RELEASE main
				deb $TARGET_REPO $TARGET_RELEASE main contrib non-free rpi
				deb http://archive.raspberrypi.org/debian $TARGET_RELEASE main
				EOF
			;;
	esac

	[[ -f /etc/apt/apt.conf.d/02proxy ]] && cp {,"$TEMP_DIR"}/etc/apt/apt.conf.d/02proxy
	[[ -f /etc/apt/apt.conf.d/30pluto ]] && cp {,"$TEMP_DIR"}/etc/apt/apt.conf.d/30pluto
	[[ -f /etc/apt/preferences ]] && cp {,"$TEMP_DIR"}/etc/apt/preferences
	[[ -f /etc/apt/apt.conf ]] && cp {,"$TEMP_DIR"}/etc/apt/apt.conf

	# This can significantly improve overall performance by preventing massive nubmers of 'sync's from occuring during install
	echo "force-unsafe-io" > ${TEMP_DIR}/etc/dpkg/dpkg.cfg.d/02apt-speedup

	## Setup initial ssh access
	StatsMessage "Setting up SSH"
	[[ -f /usr/pluto/keys/id_dsa_pluto.pub ]] && mkdir -p "$TEMP_DIR"/root/.ssh && cat /usr/pluto/keys/id_dsa_pluto.pub >> "$TEMP_DIR"/root/.ssh/authorized_keys

	## Setup kernel postinst script to repair vmlinuz/initrd.img symlinks in /
	StatsMessage "Setting up kernel postinst.d/"
	cat <<-"EOF" >$TEMP_DIR/etc/kernel/postinst.d/update-symlinks
		#!/bin/bash
		# LinuxMCE post kernel image install.
		#
		# We make sure we can read the image and the kernel, and the softlink is correct.
		chmod g+r /boot/* || :
		chmod o+r /boot/* || :

		pushd /
		ln -sf boot/initrd.img-$1 initrd.img || :
		ln -sf boot/vmlinuz-$1 vmlinuz || :
		popd

		exit 0
		EOF
	chmod +x $TEMP_DIR/etc/kernel/postinst.d/update-symlinks
}

MD_Seamless_Compatability () {
	case "$TARGET_DISTRO" in
		ubuntu)
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
			;;
		raspbian)
			echo -e "BOOT=nfs\nMODULES=most\nCOMPRESS=\n" > "$TEMP_DIR/etc/initramfs-tools/conf.d/linuxmce.conf"
			VerifyExitCode "Creation of etc/initramfs-tools/conf.d/linuxmce.conf file failed"

	                if ! grep smsc95xx $TEMP_DIR/etc/initramfs-tools/modules >/dev/null; then
	                        #White space should not be present in the modules files, you MUST remove for the MD to boot properly
	                        sed -i '/^$/d' "$TEMP_DIR/etc/initramfs-tools/modules"
	                        echo smsc95xx>>"$TEMP_DIR/etc/initramfs-tools/modules"
				VerifyExitCode "Insertion of smsc95xx module in initramfs-tools/modules failed"
	                fi
			;;
	esac
}

MD_Preseed () {
	StatsMessage "PreSeeding package installation preferences"
	## Setup debconf interface to 'noninteractive'
	LC_ALL=C chroot $TEMP_DIR apt-get -y --force-yes install debconf-i18n
	VerifyExitCode "install of debconf-i18n"

	#create preseed file
	cat <<-EOF >$TEMP_DIR/tmp/preseed.cfg
		debconf debconf/frontend	select Noninteractive
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
		debconf debconf/frontend        select Noninteractive
		# Choices: critical, high, medium, low
		debconf debconf/priority        select critical
		EOF

	LC_ALL=C chroot $TEMP_DIR debconf-set-selections /tmp/preseed.cfg
	VerifyExitCode "debconf-set-selections - preseed data"

	#For some odd reason, set-selections adds a space for Noninteractive and Critical that needs to be removed - debconf doesn't handle extra white space well
	sed -i 's/Value:  /Value: /g' $TEMP_DIR/var/cache/debconf/config.dat

	#remove preseed file, no need to clutter things up
	rm $TEMP_DIR/tmp/preseed.cfg

	# This does an update, while adding gpg keys for any that are missing. This is primarily for vlc
	# but will work for any source.
	gpgs=$(apt-get -y update |& grep -s NO_PUBKEY | awk '{ print $NF }' | cut -c 9-16);
	if [ -n "$gpgs" ]; then
		echo "$gpgs" | while read gpgkeys ; do
			gpg --keyserver pgp.mit.edu --recv-keys "$gpgkeys"
			gpg --export --armor "$gpgkeys" | apt-key add -
		done
	fi

	LC_ALL=C chroot $TEMP_DIR apt-get -y -qq update
	VerifyExitCode "apt update"

	LC_ALL=C chroot $TEMP_DIR apt-get -f -y install
}

MD_Install_Packages () {
	## FIXME: Need to convert this to dpkg-divert for greater reliability

	echo "Install/Upgrade service invocation packages sysv-rc and upstart"
	# Install service invocation utilities
	LC_ALL=C chroot $TEMP_DIR apt-get -f -y install sysv-rc screen pwgen
	case "$TARGET_RELEASE" in
		"trusty")
			LC_ALL=C chroot $TEMP_DIR apt-get -f -y install upstart
		;;
	esac

	echo "Disable all service invocation"

	LC_ALL=C chroot $TEMP_DIR dpkg-divert --add --rename --divert /sbin/start.orig /sbin/start || :
	LC_ALL=C chroot $TEMP_DIR dpkg-divert --add --rename --divert /sbin/start-stop-daemon.orig /sbin/start-stop-daemon || :
	LC_ALL=C chroot $TEMP_DIR dpkg-divert --add --rename --divert /usr/sbin/invoke-rc.d.orig /usr/sbin/invoke-rc.d || :
	LC_ALL=C chroot $TEMP_DIR dpkg-divert --add --rename --divert /sbin/restart.orig /sbin/restart || :
	LC_ALL=C chroot $TEMP_DIR dpkg-divert --add --rename --divert /sbin/initctl.orig /sbin/initctl || :
	LC_ALL=C chroot $TEMP_DIR dpkg-divert --add --rename --divert /bin/systemctl.orig /bin/systemctl || :

#	# disable service invocation and tools
#	mv -f $TEMP_DIR/sbin/start{,.orig} || :
#	mv -f $TEMP_DIR/sbin/start-stop-daemon{,.orig} || :
#	mv -f $TEMP_DIR/usr/sbin/invoke-rc.d{,.orig} || :
#	mv -f $TEMP_DIR/sbin/restart{,.orig} || :
#	mv -f $TEMP_DIR/sbin/initctl{,.orig} || :
#	mv -f $TEMP_DIR/bin/systemctl{,.orig} || :
#
	cat <<-EOF > $TEMP_DIR/sbin/start
		#!/bin/bash
		echo "DIVERTED: we dont want \$0 to run right now"
		exit 0
		EOF
	chmod +x $TEMP_DIR/sbin/start
	[[ -f "$TEMP_DIR/sbin/start-stop-daemon.orig" ]] && cp $TEMP_DIR/sbin/start $TEMP_DIR/sbin/start-stop-daemon
	[[ -f "$TEMP_DIR/usr/sbin/invoke-rc.d.orig" ]] && cp $TEMP_DIR/sbin/start $TEMP_DIR/sbin/invoke-rc.d
	[[ -f "$TEMP_DIR/sbin/restart.orig" ]] && cp $TEMP_DIR/sbin/start $TEMP_DIR/sbin/restart
	[[ -f "$TEMP_DIR/sbin/initctl.orig" ]] && cp $TEMP_DIR/sbin/start $TEMP_DIR/sbin/initctl
	[[ -f "$TEMP_DIR/sbin/systemctl.orig" ]] && cp $TEMP_DIR/sbin/start $TEMP_DIR/bin/systemctl


	StatsMessage "Installing packages to MD"
	## Update the chrooted system (needed when created from archive)
	LC_ALL=C chroot $TEMP_DIR apt-get -f -y dist-upgrade
	VerifyExitCode "Dist-upgrade failed"

	# generate locales
	StatsMessage "Generating locales"
	echo "en_US.UTF-8 UTF-8" >"$TEMP_DIR"/etc/locale.gen
	LC_ALL=C chroot "$TEMP_DIR" apt-get -f -y install locales

	case "$TARGET_DISTRO" in
		"ubuntu")
			StatsMessage "Installing kernel headers"
			#Install headers and run depmod for the seamless integraton function, ensure no errors exist
			TARGET_KVER_LTS_HES=""
			[[ "precise" == "$TARGET_RELEASE" ]] && TARGET_KVER_LTS_HES="-lts-trusty"
			[[ "trusty" == "$TARGET_RELEASE" ]] && TARGET_KVER_LTS_HES="-lts-utopic"
			[[ "xenial" == "$TARGET_RELEASE" ]] && TARGET_KVER_LTS_HES=""
			echo "LTS_HES = $TARGET_KVER_LTS_HES" >> $TEMP_DIR/etc/pluto.conf
			LC_ALL=C chroot "$TEMP_DIR" apt-get -y install linux-headers-generic"$TARGET_KVER_LTS_HES"
			VerifyExitCode "Install linux headers package failed"

			TARGET_KVER=$(ls -vd "$TEMP_DIR"/lib/modules/[0-9]* | sed 's/.*\///g' | tail -1)
			LC_ALL=C chroot "$TEMP_DIR" depmod -v "$TARGET_KVER"
			VerifyExitCode "depmod failed for $TARGET_KVER"

			StatsMessage "Installing kernel"
			LC_ALL=C chroot "$TEMP_DIR" apt-get -f -y --no-install-recommends install linux-image-generic"$TARGET_KVER_LTS_HES"
			VerifyExitCode "Install linux kernel package failed"

			## Prevent lpadmin from running as it blocks the system
			LC_ALL=C chroot "$TEMP_DIR" apt-get -f -y install cups-client
			cp "$TEMP_DIR"/usr/sbin/lpadmin{,.disabled}
			echo > "$TEMP_DIR"/usr/sbin/lpadmin
			LC_ALL=C chroot "$TEMP_DIR" apt-get -f -y install cups-pdf
			mv "$TEMP_DIR"/usr/sbin/lpadmin{.disabled,}
			;;
		"raspbian")
			LC_ALL=C chroot "$TEMP_DIR" apt-get -y install libraspberrypi-bin raspberrypi-bootloader rpi-update
			BRANCH=next LC_ALL=C chroot "$TEMP_DIR" rpi-update

			echo "AVWizardDone = 1" >> $TEMP_DIR/etc/pluto.conf
			echo "AVWizardOverride = 0" >> $TEMP_DIR/etc/pluto.conf

			case "$TARGET_DISTRO_ID" in
				19)  # Wheezy (for rpi1 u-boot pxe booting)
					# raspbian wheezy doesn't come with lsb-release by default??? fixed in jessie
					LC_ALL=C chroot "$TEMP_DIR" apt-get -y install lsb-release
					cat <<-EOF > $TEMP_DIR/etc/lsb-release
						DISTRIB_ID=Raspbian
						DISTRIB_CODENAME=$TARGET_RELEASE
						EOF

					# HACK: copy the foundation kernel.img to a normal linux kernal name with version
					LC_ALL=C chroot "$TEMP_DIR" mkdir -p /sdcard
					LC_ALL=C chroot "$TEMP_DIR" mkdir -p /tmp
					cat <<-EOF > $TEMP_DIR/tmp/symlink_kernel.sh
						#!/bin/bash
						cd /boot
						ln -sf kernel.img vmlinuz-3.6-trunk-rpi
						touch /boot/initrd.img-3.6-trunk-rpi
						EOF
					chmod +x $TEMP_DIR/tmp/symlink_kernel.sh
					LC_ALL=C chroot "$TEMP_DIR" /tmp/symlink_kernel.sh
					;;
			esac
	                LC_ALL=C chroot $TEMP_DIR apt-get -y install alsa-base alsa-utils
			# pulseaudio
			VerifyExitCode "alsa-base, alsa-utils or pulseaudio packages install failed"
			;;
	esac

	case "$TARGET_ARCH" in
		i386|amd64)
			echo "do_initrd = Yes" > $TEMP_DIR/etc/kernel-img.conf
			;;
	esac

	## Prevent discover from running as it blocks the system
	#LC_ALL=C chroot "$TEMP_DIR" apt-get -y install discover
	#cp "$TEMP_DIR"/sbin/discover "$TEMP_DIR"/sbin/discover.disabled
	#echo > "$TEMP_DIR"/sbin/discover


	########## START CREATE LIST OF DEVICES #################"
	## that will run on the md so we will know
	## what software to preinstall there
	## FIXME: get this list from the database
	case "$TARGET_DISTRO" in
		"ubuntu")
			DEVICE_LIST="28 62 1759 5 11 1825 26 1808 1901 2122 2278"
			;;
		"raspbian")
			# Classic MD/qMD/squeezelite
			DEVICE_LIST="2216 62 1759 2259 11 1825 26 1808 2278 2284"
			# qMD
			DEVICE_LIST="$DEVICE_LIST 2122"
			;;
	esac

#	# Determine if MythTV is installed by looking for MythTV_Plugin...
#	# Don't install mythtv into raspbian
#	if [[ "$TARGET_DISTRO" != "raspbian" ]]; then
#		Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=36"
#		MythTV_Installed=$(RunSQL "$Q")
#		if [ $MythTV_Installed ];then
#			#MythTV_Plugin is installed, so install MythTV_Player on MD
			DEVICE_LIST="$DEVICE_LIST 35"
#		fi
#	fi

	# Add vdr device packages to diskless image - needs some jessie armhf love
	#DEVICE_LIST="$DEVICE_LIST 1705"

	########## END CREATE LIST OF DEVICES ###################"

	## Begin installing the packages needed for the pluto devices
	StatsMessage "Prep for MCE device installation"

	for device in $DEVICE_LIST ; do
		Q="SELECT
			Package_Source.Name
		   FROM
			Package_Source
			JOIN DeviceTemplate on Package_Source.FK_Package = DeviceTemplate.FK_Package
		   WHERE
			PK_DeviceTemplate = $device
		   AND
			FK_RepositorySource = $TARGET_REPO_LMCE_SRC
		"

		R=$(RunSQL "$Q")
		VerifyExitCode "Connecting to MYSQL DB failed"

		for Row in $R ; do
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
				LC_ALL=C chroot $TEMP_DIR apt-get -f -y --force-yes install
			fi
		done
	done

	StatsMessage "Install other ancillary programs for MCE"

	## Put back discover
	#mv "$TEMP_DIR"/sbin/discover.disabled "$TEMP_DIR"/sbin/discover

	## Install additional packages
	case "$TARGET_DISTRO" in
		"ubuntu")
			LC_ALL=C chroot $TEMP_DIR apt-get -y install lmce-media-identifier
			## If libdvdcss2 is installed on the hybrid/core
			if [[ -d /usr/share/doc/libdvdcss2 ]] ;then
				pushd $TEMP_DIR >/dev/null
				if [[ "$TARGET_DISTRO_ID" -ge "22" ]] ; then
					LC_ALL=C chroot $TEMP_DIR apt-get -y install libdvd-pkg || :
				else
					LC_ALL=C chroot $TEMP_DIR apt-get -y install libdvdcss2 || :
				fi
				popd >/dev/null
			fi

			if [[ "$INSTALL_KUBUNTU_DESKTOP" != "no" ]]; then
				LC_ALL=C chroot $TEMP_DIR apt-get -y install kubuntu-desktop
				#LC_ALL=C chroot $TEMP_DIR apt-get -y install kde-minimal
			fi
			echo '/bin/false' >"$TEMP_DIR/etc/X11/default-display-manager"

			# Update startup to remove kdm and network manager
			LC_ALL=C chroot $TEMP_DIR update-rc.d -f kdm remove || :
			LC_ALL=C chroot $TEMP_DIR update-rc.d -f sddm remove || :
			LC_ALL=C chroot $TEMP_DIR update-rc.d -f NetworkManager remove
			LC_ALL=C chroot $TEMP_DIR systemctl mask failsafe-x.service || :

			#Install ancillary programs
			LC_ALL=C chroot $TEMP_DIR apt-get -y install xserver-xorg"$TARGET_KVER_LTS_HES"
			VerifyExitCode "xserver-xorg$TARGET_KVER_LTS_HES install failed"
			LC_ALL=C chroot $TEMP_DIR apt-get -y install xserver-xorg-video-all"$TARGET_KVER_LTS_HES" linux-firmware
			VerifyExitCode "Ancillary programs install failed"

			## Install plymouth theme on MD in Ubuntu
			#LC_ALL=C chroot $TEMP_DIR apt-get -y install lmce-plymouth-theme
			#VerifyExitCode "MCE plymouth theme install failed"

	                # upstart fix to prevent waiting on MD boot
	                if [[ -f "$TEMP_DIR/etc/init/rc-sysinit.conf" ]]; then
				sed -i".pbackup" 's/ and static-network-up//g' "$TEMP_DIR/etc/init/rc-sysinit.conf"
			fi
			;;
		"raspbian")
			#FIXME: why is this required, something missing?
			#LC_ALL=C chroot $TEMP_DIR addgroup --force-badname Debian-exim
			#VerifyExitCode "addgroup Debian-Exim failed"
			LC_ALL=C chroot "$TEMP_DIR" sed -i '/Debian-exim/d' /var/lib/dpkg/statoverride
			LC_ALL=C chroot "$TEMP_DIR" apt-get -y install abcde acl adduser adwaita-icon-theme alsa-base alsa-utils apt apt-utils aptitude aptitude-common aspell aspell-en at-spi2-core attr autofs avahi-daemon base-files base-passwd bash bc beep bind9-host binutils bluez-obexd bluez-tools bsd-mailx bsdmainutils bsdutils bzip2 ca-certificates cabextract cd-discid cdparanoia cifs-utils colord colord-data coreutils cpio cpp cpp-4.9 cron cryptsetup-bin curl dash dbus dbus-x11 dconf-gsettings-backend dconf-service debconf debconf-i18n debianutils device-tree-compiler dh-python dictionaries-common diffutils dmidecode dmsetup dosfstools dpkg e2fslibs e2fsprogs eject emacsen-common enchant ethtool exim4-base exim4-config exim4-daemon-light fbset file findutils flac fontconfig fontconfig-config fonts-dejavu fonts-dejavu-core fonts-dejavu-extra fonts-droid fonts-freefont-ttf fonts-liberation fonts-tlwg-purisa freepats fuse gawk gcc-4.6-base gcc-4.7-base gcc-4.8-base gcc-4.9-base gconf-service gconf2 gconf2-common gcr geoip-database ghostscript gir1.2-glib-2.0 gksu glib-networking glib-networking-common glib-networking-services gnome-keyring gnome-mime-data gnome-osd gnupg gpgv grep groff-base gsettings-desktop-schemas gsfonts gstreamer1.0-alsa gstreamer1.0-libav gstreamer1.0-omx gstreamer1.0-plugins-bad gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-x gzip hdparm hicolor-icon-theme hostname hunspell-en-us ifupdown imagemagick-common info init init-system-helpers initramfs-tools initscripts insserv install-info iproute iproute2 iptables iputils-ping isc-dhcp-client isc-dhcp-common iso-codes keyboard-configuration keyutils klibc-utils kmod krb5-locales lame less liba52-0.7.4 libaa1 libaacs0 libacl1 libaio1 libalgorithm-c3-perl libao-common libao4 libapt-inst1.5 libapt-pkg4.12 libarchive-extract-perl libarchive13 libart-2.0-2 libasn1-8-heimdal libasound2 libasound2-data libasound2-plugins libaspell15 libass5 libasyncns0 libatasmart4 libatk-bridge2.0-0 libatk1.0-0 libatk1.0-data libatspi2.0-0 libattr1 libaudio2 libaudit-common libaudit1 libauthen-sasl-perl libavahi-client3 libavahi-common-data libavahi-common3 libavahi-compat-libdnssd1 libavahi-core7 libavahi-glib1 libavc1394-0 libavcodec-ffmpeg56 libavcodec56 libavformat-ffmpeg56 libavformat56 libavresample2 libavutil-ffmpeg54 libavutil54 libbfb0 libbind9-90 libblas-common libblas3 libblkid1 libbluetooth3 libbluray1 libbonobo2-0 libbonobo2-common libbonoboui2-0 libbonoboui2-common libboost-iostreams1.49.0 libboost-iostreams1.50.0 libboost-iostreams1.53.0 libboost-iostreams1.54.0 libboost-iostreams1.55.0 libbsd0 libbz2-1.0 libc-bin libc6 libc6-dbg libcaca0 libcairo-gobject2 libcairo2 libcanberra-gtk3-0 libcanberra-gtk3-module libcanberra0 libcap-ng0 libcap2 libcap2-bin libcddb2 libcdio13 libcdparanoia0 libcec-platform1 libcec3v4 libcgi-fast-perl libcgi-pm-perl libchromaprint0 libclass-accessor-perl libclass-c3-perl libclass-c3-xs-perl libcloog-isl4 libcolord2 libcolorhug2 libcomerr2 libconfuse-common libconfuse0 libcpan-meta-perl libcroco3 libcrypt-smbhash-perl libcryptsetup4 libcups2 libcupsfilters1 libcupsimage2 libcurl3 libcurl3-gnutls libcwidget3 libdaemon0 libdata-optlist-perl libdata-section-perl libdatrie1 libdb5.3 libdbd-mysql-perl libdbi-perl libdbus-1-3 libdbus-c++-1-0 libdbus-glib-1-2 libdc1394-22 libdca0 libdconf1 libdebconfclient0 libdevmapper-event1.02.1 libdevmapper1.02.1 libdigest-md4-perl libdirectfb-1.2-9 libdiscid0 libdns-export100 libdns100 libdrm-amdgpu1 libdrm-freedreno1 libdrm-nouveau2 libdrm-radeon1 libdrm2 libdv4 libdvdnav4 libdvdread4 libecore1 libedit2 libegl1-mesa libeina1 libelf1 libenca0 libenchant1c2a libencode-locale-perl libepoxy0 libestr0 libevdev2 libevent-2.0-5 libexif12 libexiv2-13 libexosip2-11 libexpat1 libfaad2 libfcgi-perl libffi6 libfftw3-double3 libfftw3-single3 libfile-copy-recursive-perl libfile-listing-perl libflac8 libflite1 libfluidsynth1 libfont-afm-perl libfontconfig1 libfontenc1 libfreetype6 libfribidi0 libfstrcmp0 libfuse2 libgail18 libgbm1 libgcc1 libgck-1-0 libgconf-2-4 libgcr-3-common libgcr-base-3-1 libgcr-ui-3-1 libgcrypt20 libgd3 libgdbm3 libgdk-pixbuf2.0-0 libgdk-pixbuf2.0-common libgeoclue0 libgeoip1 libgfortran3 libgirepository-1.0-1 libgksu2-0 libgl1-mesa-dri libgl1-mesa-glx libglade2-0 libglapi-mesa libgles2-mesa libglew1.10 libglib2.0-0 libglib2.0-data libglu1-mesa libgme0 libgmp10 libgnome-2-0 libgnome-keyring-common libgnome-keyring0 libgnome2-common libgnomecanvas2-0 libgnomecanvas2-common libgnomeui-0 libgnomeui-common libgnomevfs2-0 libgnomevfs2-common libgnomevfs2-extra libgnutls-deb0-28 libgnutls-openssl27 libgomp1 libgpg-error0 libgphoto2-6 libgphoto2-l10n libgphoto2-port10 libgpm2 libgraphicsmagick3 libgraphite2-3 libgs9 libgs9-common libgsm1 libgssapi-krb5-2 libgstreamer-plugins-bad1.0-0 libgstreamer-plugins-base1.0-0 libgstreamer1.0-0 libgtk-3-0 libgtk-3-bin libgtk-3-common libgtk2.0-0 libgtk2.0-bin libgtk2.0-common libgtkglext1 libgtop2-7 libgtop2-common libgudev-1.0-0 libgusb2 libharfbuzz-icu0 libharfbuzz0b libhcrypto4-heimdal libhdb9-heimdal libheimbase1-heimdal libhogweed2 libhtml-form-perl libhtml-format-perl libhtml-parser-perl libhtml-tagset-perl libhtml-tree-perl libhttp-cookies-perl libhttp-daemon-perl libhttp-date-perl libhttp-message-perl libhttp-negotiate-perl libhunspell-1.3-0 libhx509-5-heimdal libical1a libice6 libicu52 libident libidl0 libidn11 libiec61883-0 libieee1284-3 libijs-0.35 libilmbase6 libio-html-perl libio-socket-inet6-perl libio-socket-ssl-perl libirs-export91 libisc-export95 libisc95 libisccc90 libisccfg-export90 libisccfg90 libisl10 libiso9660-8 libjack-jackd2-0 libjasper1 libjavascriptcoregtk-3.0-0 libjbig0 libjbig2dec0 libjpeg62-turbo libjpeg8 libjson-c2 libjson-glib-1.0-0 libjson-glib-1.0-common libk5crypto3 libkate1 libkeyutils1 libklibc libkmod2 libkrb5-26-heimdal libkrb5-3 libkrb5support0 liblapack3 liblcms2-2 libldap-2.4-2 libldb1 liblinear1 liblinphone5 liblircclient0 libllvm3.7 liblocale-gettext-perl liblockdev1 liblockfile-bin liblockfile1 liblog-message-perl liblog-message-simple-perl liblogging-stdlog0 liblognorm1 liblqr-1-0 libltdl7 liblua5.2-0 liblvm2app2.2 liblwp-mediatypes-perl liblwp-protocol-https-perl liblwres90 liblzma5 liblzo2-2 libmad0 libmagic1 libmagickcore-6.q16-2 libmagickwand-6.q16-2 libmailtools-perl libmediastreamer-base3 libmimic0 libmjpegutils-2.1-0 libmms0 libmng1 libmodplug1 libmodule-build-perl libmodule-pluggable-perl libmodule-signature-perl libmount1 libmp3lame0 libmpc3 libmpcdec6 libmpdec2 libmpeg2-4 libmpeg2encpp-2.1-0 libmpfr4 libmpg123-0 libmplex2-2.1-0 libmro-compat-perl libmspack0 libmtdev1 libmulticobex1 libmusicbrainz-discid-perl libmysqlclient18 libmyth-0.27-0 libmyth-python libmythtv-perl libncurses5 libncursesw5 libnet-http-perl libnet-smtp-ssl-perl libnet-ssleay-perl libnet-upnp-perl libnettle4 libnewt0.52 libnfnetlink0 libnfsidmap2 libnih-dbus1 libnih1 libnotify-bin libnotify4 libnss-mdns libntdb1 libobexftp0 libofa0 libogg0 libopenal-data libopenal1 libopencore-amrnb0 libopencore-amrwb0 libopencv-calib3d2.4 libopencv-contrib2.4 libopencv-core2.4 libopencv-features2d2.4 libopencv-flann2.4 libopencv-highgui2.4 libopencv-imgproc2.4 libopencv-legacy2.4 libopencv-ml2.4 libopencv-objdetect2.4 libopencv-video2.4 libopenexr6 libopenjpeg5 libopenobex1 libopts25 libopus0 liborbit-2-0 liborbit2 liborc-0.4-0 libortp9 libosip2-11 libp11-kit0 libpackage-constants-perl libpam-gnome-keyring libpam-modules libpam-modules-bin libpam-runtime libpam-systemd libpam0g libpango-1.0-0 libpangocairo-1.0-0 libpangoft2-1.0-0 libpangox-1.0-0 libpaper-utils libpaper1 libparams-util-perl libparted2 libpcap0.8 libpci3 libpciaccess0 libpcre3 libpipeline1 libpixman-1-0 libpng12-0 libpod-latex-perl libpod-readme-perl libpolkit-agent-1-0 libpolkit-backend-1-0 libpolkit-gobject-1-0 libpopt0 libpostproc52 libprocps3 libproxy1 libpsl0 libpulse0 libpython-stdlib libpython2.7 libpython2.7-minimal libpython2.7-stdlib libpython3-stdlib libpython3.4-minimal libpython3.4-stdlib libqt4-dbus libqt4-network libqt4-script libqt4-sql libqt4-sql-mysql libqt4-xml libqt4-xmlpatterns libqt5core5a libqt5dbus5 libqt5gui5 libqt5multimedia5 libqt5multimediaquick-p5 libqt5network5 libqt5opengl5 libqt5qml5 libqt5quick5 libqt5quickparticles5 libqt5script5 libqt5widgets5 libqt5xml5 libqtcore4 libqtdbus4 libqtgui4 libqtwebkit4 libquicktime2 libraspberrypi-bin libraspberrypi0 libraw1394-11 libreadline6 libregexp-common-perl librest-0.7-0 libroken18-heimdal librsvg2-2 librsvg2-common librtmp1 libsamplerate0 libsane libsane-common libsane-extras libsane-extras-common libsasl2-2 libsasl2-modules libsasl2-modules-db libsbc1 libschroedinger-1.0-0 libsdl-gfx1.2-5 libsdl-image1.2 libsdl-sge libsdl-ttf2.0-0 libsdl1.2debian libsecret-1-0 libsecret-common libselinux1 libsemanage-common libsemanage1 libsepol1 libsgutils2-2 libshine3 libshout3 libsigc++-1.2-5c2 libsigc++-2.0-0c2a libsigsegv2 libslang2 libslp1 libsm6 libsmartcols1 libsmbclient libsndfile1 libsocket6-perl libsoftware-license-perl libsoundtouch0 libsoup-gnome2.4-1 libsoup2.4-1 libsox-fmt-alsa libsox-fmt-base libsox2 libsoxr0 libspandsp2 libspeex1 libspeexdsp1 libsqlite3-0 libsrtp0 libss2 libssh-4 libssh2-1 libssl1.0.0 libstartup-notification0 libstdc++6 libsub-exporter-perl libsub-install-perl libsub-name-perl libsvga1 libswresample-ffmpeg1 libswscale3 libsysfs2 libsystemd0 libtag1-vanilla libtag1c2a libtalloc2 libtasn1-6 libtdb1 libterm-readkey-perl libterm-ui-perl libtevent0 libtext-charwidth-perl libtext-iconv-perl libtext-soundex-perl libtext-template-perl libtext-wrapi18n-perl libthai-data libthai0 libtheora0 libtiff5 libtimedate-perl libtinfo5 libtirpc1 libtwolame0 libtxc-dxtn-s2tc0 libudev0 libudev1 libupnp6 liburi-perl libusb-0.1-4 libusb-1.0-0 libustr-1.0-1 libutempter0 libuuid1 libv4l-0 libv4lconvert0 libva-glx1 libva-x11-1 libva1 libvcdinfo0 libvdpau1 libvisual-0.4-0 libvisual-0.4-plugins libvo-aacenc0 libvo-amrwbenc0 libvorbis0a libvorbisenc2 libvorbisfile3 libvpx1 libwavpack1 libwayland-client0 libwayland-cursor0 libwayland-server0 libwbclient0 libwebkitgtk-3.0-0 libwebkitgtk-3.0-common libwebp5 libwebpdemux1 libwebpmux1 libwebservice-musicbrainz-perl libwildmidi-config libwildmidi1 libwind0-heimdal libwmf0.2-7 libwnck-common libwnck22 libwrap0 libwww-perl libwww-robotrules-perl libx11-6 libx11-data libx11-xcb1 libx264-142 libx265-43 libxapian22 libxau6 libxaw7 libxcb-dri2-0 libxcb-dri3-0 libxcb-glx0 libxcb-icccm4 libxcb-image0 libxcb-keysyms1 libxcb-present0 libxcb-randr0 libxcb-render-util0 libxcb-render0 libxcb-shape0 libxcb-shm0 libxcb-sync1 libxcb-util0 libxcb-xfixes0 libxcb-xkb1 libxcb-xv0 libxcb1 libxcomposite1 libxcursor1 libxdamage1 libxdmcp6 libxext6 libxfce4ui-1-0 libxfce4util-bin libxfce4util-common libxfce4util6 libxfconf-0-2 libxfixes3 libxfont1 libxft2 libxi6 libxine2 libxine2-bin libxine2-doc libxine2-ffmpeg libxine2-misc-plugins libxine2-plugins libxine2-x libxinerama1 libxkbcommon-x11-0 libxkbcommon0 libxkbfile1 libxml-libxml-perl libxml-namespacesupport-perl libxml-parser-perl libxml-sax-base-perl libxml-sax-expat-perl libxml-sax-perl libxml-simple-perl libxml-xpath-perl libxml2 libxmu6 libxmuu1 libxosd2 libxpm4 libxrandr2 libxrender1 libxres1 libxshmfence1 libxslt1.1 libxt6 libxtables10 libxtst6 libxv1 libxvidcore4 libxvmc1 libxxf86dga1 libxxf86vm1 libzbar0 libzvbi-common libzvbi0 linphone-common lmce-avwizard-skin-basic lmce-cec-adaptor lmce-core-locator lmce-install-scripts lmce-launch-manager lmce-md-meta lmce-mythtv-scripts lmce-nbd-client-wrapper lmce-omx-player lmce-picture-viewer lmce-qorbiter lmce-runtime-dependencies lmce-squeezeslave lmce-usb-gamepad lmce-windowutils locales login logrotate lsb-base lsb-release lsscsi make makedev man-db manpages mawk mdadm mime-support mount mountall multiarch-support mysql-client mysql-client-5.5 mysql-common mythtv-backend mythtv-common mythtv-dbg mythtv-frontend mythtv-transcode-utils nano nbd-client nbd-server nbtscan ncurses-base ncurses-bin ncurses-term ndiff net-tools netbase netcat netcat-openbsd netcat-traditional nfs-common nfs-kernel-server nis nmap notification-daemon ntfs-3g ntp obexftp omxplayer openobex-apps openssh-client openssh-server openssh-sftp-server openssl p11-kit p11-kit-modules parallel passwd pastebinit pciutils perl perl-base perl-modules pluto-app-server pluto-avwizard pluto-bluetooth-dongle pluto-boot-scripts pluto-capture-card-scripts pluto-confirm-dependencies pluto-dcecommon pluto-disc-drive-functions pluto-disk-drive pluto-disk-monitor pluto-hal-device-finder pluto-hald pluto-irbase pluto-libbd pluto-libresolution pluto-libserial pluto-mcr-remote pluto-messagesend pluto-messagetrans pluto-mythtv-player pluto-orbiter pluto-orbitergen pluto-photo-screen-saver pluto-pluto-main-db pluto-pluto-media-db pluto-plutoutils pluto-pnp-detection pluto-raid-tools pluto-sdl-helpers pluto-serializeclass pluto-shiftstate pluto-simplephone pluto-sound-card-scripts pluto-storage-devices pluto-test-serial-port pluto-updateentarea pluto-vipshared pluto-x-scripts pluto-xine-player plymouth policykit-1 poppler-data powermgmt-base procps psmisc pwgen python python-cairo python-crypto python-dbus python-dbus-dev python-dnspython python-gconf python-gi python-gnome2 python-gobject-2 python-gtk2 python-imaging python-imdbpy python-ldb python-lxml python-minimal python-mysqldb python-ntdb python-numpy python-pil python-pycurl python-pyorbit python-pyorbit-omg python-samba python-support python-talloc python-tdb python-urlgrabber python2.7 python2.7-minimal python3 python3-minimal python3.4 python3.4-minimal qdbus qml-module-qtgraphicaleffects qml-module-qtmultimedia qml-module-qtquick-controls qml-module-qtquick-dialogs qml-module-qtquick-layouts qml-module-qtquick-particles2 qml-module-qtquick-privatewidgets qml-module-qtquick-window2 qml-module-qtquick2 qtchooser qtcore4-l10n qttranslations5-l10n raspberrypi-bootloader raspberrypi-kernel raspbian-archive-keyring raspi2png readline-common rename rpcbind rpi-update rsyslog samba samba-common samba-common-bin samba-dsdb-modules samba-libs samba-vfs-modules sane-utils screen sed sensible-utils sgml-base shared-mime-info smbclient sox squeezelite ssh startpar sudo systemd systemd-sysv sysv-rc sysvinit-utils tar tasksel tasksel-data tcpd tdb-tools tee-pluto traceroute transcode transcode-doc ttf-dejavu ttf-dejavu-core ttf-dejavu-extra ttf-mscorefonts-installer twolame tzdata ucf udev udisks update-inetd util-linux va-driver-all vdpau-va-driver vim-common vim-tiny vorbis-tools wget whiptail winbind wmctrl x11-apps x11-common x11-session-utils x11-utils x11-xkb-utils x11-xserver-utils xauth xbitmaps xcompmgr xdg-user-dirs xfce-keyboard-shortcuts xfconf xfonts-100dpi xfonts-75dpi xfonts-base xfonts-encodings xfonts-scalable xfonts-utils xfwm4 xinit xkb-data xloadimage xml-core xorg xorg-docs-core xosd-bin xserver-common xserver-xorg xserver-xorg-core xserver-xorg-input-all xserver-xorg-input-evdev xserver-xorg-input-synaptics xserver-xorg-input-wacom xserver-xorg-video-fbdev xterm xz-utils zenity zenity-common zlib1g
			;;
	esac

	#Install nfs-common and openssh-server
	LC_ALL=C chroot "$TEMP_DIR" apt-get -y install nfs-common openssh-server
	VerifyExitCode "nfs-common or openssh-server programs install failed"

	LC_ALL=C chroot "$TEMP_DIR" apt-get -y install xinit
	VerifyExitCode "xinit"

	LC_ALL=C chroot "$TEMP_DIR" apt-get -y install pastebinit
	VerifyExitCode "pastebinit"

	##implement external_media_identifier fix
	#LC_ALL=C chroot $TEMP_DIR ln -s /usr/lib/libdvdread.so.4 /usr/lib/libdvdread.so.3
}

MD_Populate_Debcache () {
	/usr/pluto/bin/UpdateDebCache.sh
}

MD_Cleanup () {
	StatsMessage "Cleaning up from package installations..."

	[[ -f "$TEMP_DIR/sbin/start.orig" ]] && rm -f "$TEMP_DIR/sbin/start"
	[[ -f "$TEMP_DIR/sbin/start-stop-daemon.orig" ]] && rm -f "$TEMP_DIR/sbin/start-stop-daemon"
	[[ -f "$TEMP_DIR/usr/sbin/invoke-rc.d.orig" ]] && rm -f "$TEMP_DIR/usr/sbin/invoke-rc.d"
	[[ -f "$TEMP_DIR/sbin/restart.orig" ]] && rm -f "$TEMP_DIR/sbin/restart"
	[[ -f "$TEMP_DIR/sbin/initctl.orig" ]] && rm -f "$TEMP_DIR/sbin/initctl"
	[[ -f "$TEMP_DIR/bin/systemctl.orig" ]] && rm -f "$TEMP_DIR/bin/systemctl"
	LC_ALL=C chroot "$TEMP_DIR" dpkg-divert --rename --remove /sbin/start
	LC_ALL=C chroot "$TEMP_DIR" dpkg-divert --rename --remove /sbin/start-stop-daemon
	LC_ALL=C chroot "$TEMP_DIR" dpkg-divert --rename --remove /usr/sbin/invoke-rc.d
	LC_ALL=C chroot "$TEMP_DIR" dpkg-divert --rename --remove /sbin/restart
	LC_ALL=C chroot "$TEMP_DIR" dpkg-divert --rename --remove /sbin/initctl
	LC_ALL=C chroot "$TEMP_DIR" dpkg-divert --rename --remove /bin/systemctl

	PIDS=$(lsof ${TEMP_DIR} 2>/dev/null | awk '{print $2}' | tail -n+2 | uniq )
	until [[ -z "${PIDS}" ]] ; do
		kill ${PIDS}
		sleep 1
		PIDS=$(lsof ${TEMP_DIR} 2>/dev/null | awk '{print $2}' | tail -n+2 | uniq )
	done

	MNTS=$(mount | grep ${TEMP_DIR} | awk '{print $3;}' | sort -r)
	until [[ -z "${MNTS}" ]] ; do
		umount ${MNTS}
		sleep 1
		MNTS=$(mount | grep ${TEMP_DIR} | awk '{print $3;}' | sort -r)
	done

	#Make sure there are Packages files on the MD so apt-get update does not fail
	mkdir -p $TEMP_DIR/usr/pluto/deb-cache/$DEB_CACHE
	echo "" > $TEMP_DIR/usr/pluto/deb-cache/$DEB_CACHE/Packages
	gzip -9c < $TEMP_DIR/usr/pluto/deb-cache/$DEB_CACHE/Packages > $TEMP_DIR/usr/pluto/deb-cache/$DEB_CACHE/Packages.gz
	echo "" > $TEMP_DIR/usr/pluto/deb-cache/Packages
	gzip -9c < $TEMP_DIR/usr/pluto/deb-cache/Packages > $TEMP_DIR/usr/pluto/deb-cache/Packages.gz

	rm -f ${TEMP_DIR}/etc/dpkg/dpkg.cfg.d/02apt-speedup

	#Copy the orbiter activation command to the MD's desktop
	mkdir -p "$TEMP_DIR"/root/Desktop
	cp -r "$TEMP_DIR"/etc/skel/Desktop/* "$TEMP_DIR"/root/Desktop

	#Remove the xorg file(s) from installation, need to start with a fresh slate
	rm -f "$TEMP_DIR"/etc/X11/xorg.conf*

	#Clean up apt from the installs
	LC_ALL=C chroot "$TEMP_DIR" apt-get -y clean
	VerifyExitCode "APT Clean"

	#Create a list of installed packages
	#COLUMNS=1024 chroot "$TEMP_DIR" dpkg -l | awk '/^ii/ {print $2}' >/tmp/pkglist-diskless.txt # used for deb-cache cleanup in the builder
}

Create_Diskless_Tar () {
	StatsMessage "Creating the compressed tar image file, this could take up to 1 hour depending on your system..."
	mkdir -p "$ARCHIVE_DIR"
	[[ -f "$ARCHIVE_DIR/$DisklessFS" ]] && rm -f "$ARCHIVE_DIR/$DisklessFS"
	[[ -f "$ARCHIVE_DIR/$DisklessFS.version" ]] && rm -f "$ARCHIVE_DIR/$DisklessFS.version"
	pushd "$TEMP_DIR" >/dev/null
#	tar -cJf "$ARCHIVE_DIR/$DisklessFS" *
	tar -czf "$ARCHIVE_DIR/$DisklessFS" *
	VerifyExitCode "create tar file failed"
	echo "$PlutoVersion" > "$ARCHIVE_DIR/$DisklessFS.version"
	popd >/dev/null

	rm -rf "$TEMP_DIR"

	case "$TARGET_DISTRO" in
		"ubuntu")
			# HACK'O'MATIC And make sure, we have files for both architectures.
			if [ -f /usr/pluto/install/PlutoMD-i386.tar.xz ]; then
				FILENEEDED=PlutoMD-amd64
			else
				FILENEEDED=PlutoMD-i386
			fi
			if [ ! -f /usr/pluto/install/$FILENEEDED.tar.xz ]; then
				ln -sf /usr/pluto/install/$DisklessFS /usr/pluto/install/$FILENEEDED.tar.xz
			fi
			if [ ! -f /usr/pluto/install/$FILENEEDED.tar.xz.version ]; then
				ln -sf /usr/pluto/install/$DisklessFS.version /usr/pluto/install/$FILENEEDED.tar.xz.version
			fi
			;;
	esac
}

Create_PXE_Initramfs_Vmlinuz () {
	case "$TARGET_DISTRO" in
		"ubuntu")
			StatsMessage "Building the initial initramfs and vmlinuz files for PXE booting"
			# Let's create the default files needed for successful PXE boot.
			/usr/pluto/bin/Diskless_BuildDefaultImage.sh
			VerifyExitCode "PXE vmlinuz and initramfs"
			;;
		"raspbian")
			# TODO: create/build initial image in chroot from an installed md??
			StatsMessage "No initramfs/boot files at this time"
			;;
	esac
}

###########################################################
### Main execution area
###########################################################

#Set up logging
Setup_Logfile

#Setup trap
trap "Trap_Exit" EXIT

StatsMessage "BEGIN: Running diskless create for :$TARGET_TYPES:"
#TODO get as much of this from database as possible
for TARGET in $TARGET_TYPES ; do
	TARGET_DISTRO=$(echo "$TARGET" | cut -d'-' -f1)
	TARGET_ARCH=$(echo "$TARGET" | cut -d'-' -f2)
	case "$TARGET_DISTRO" in
		"ubuntu")
			if [[ "$HOST_ARCH" == "armhf" ]] ; then
				echo "Cannot build x86/64 images on ${HOST_ARCH} core."
				exit 1
			fi
			#TARGET_DISTRO="ubuntu"
			#TARGET_ARCH="i386"
			TARGET_RELEASE="$HOST_RELEASE"
			TARGET_REPO="http://archive.ubuntu.com/ubuntu/"
			DBST_ARCHIVE="PlutoMD_Debootstraped.tar.bz2"
			DisklessFS="PlutoMD-${TARGET_ARCH}.tar.xz"
			case "$TARGET_RELEASE" in
				lucid)
					TARGET_DISTRO_ID=18
					TARGET_REPO_DISTRO_SRC=20
					TARGET_REPO_LMCE_SRC=21
					;;
				precise)
					TARGET_DISTRO_ID=20
					TARGET_REPO_DISTRO_SRC=24
					TARGET_REPO_LMCE_SRC=25
					;;
				trusty)
					TARGET_DISTRO_ID=21
					TARGET_REPO_DISTRO_SRC=24
					TARGET_REPO_LMCE_SRC=25
					;;
				xenial)
					TARGET_DISTRO_ID=23
					TARGET_REPO_DISTRO_SRC=24
					TARGET_REPO_LMCE_SRC=25
					;;
			esac
			;;
		"raspbian")  # rpi2/3 and up
			#TARGET_DISTRO="raspbian"
			#TARGET_RELEASE="wheezy"
			TARGET_RELEASE="jessie"
			TARGET_ARCH="armhf"
			TARGET_REPO="http://mirrordirector.raspbian.org/raspbian/"
			DBST_ARCHIVE="LMCEMD_Debootstraped-raspbian-armhf.tar.xz"
			DisklessFS="LMCEMD-$TARGET_DISTRO-$TARGET_ARCH.tar.xz"
			TARGET_DISTRO_ID=22
			TARGET_REPO_DISTRO_SRC=22
			TARGET_REPO_LMCE_SRC=23
			;;
	esac

	# install cross arch utilities if target_arch is different from host_arch
	if [ x"$TARGET_ARCH" != x"$HOST_ARCH" ]; then
		StatsMessage "Installing cross-arch utilities for $TARGET_RELEASE"
		apt-get -y install binfmt-support qemu debootstrap qemu-user-static
	fi

	DEB_CACHE="$TARGET_DISTRO-$TARGET_RELEASE-$TARGET_ARCH"

	StatsMessage "BEGIN: Host: $HOST_DISTRO - $HOST_RELEASE - $HOST_ARCH"
	StatsMessage "BEGIN: Target: $TARGET_DISTRO - $TARGET_RELEASE - $TARGET_ARCH"

	TEMP_DIR=$(mktemp -d /opt/Diskless_CreateTBZ.XXXXXXXXX)

	#Function execution
	#MD_Populate_Debcache
	MD_Create_And_Populate_Temp_Dir
	MD_System_Level_Prep
	MD_Seamless_Compatability
	MD_Preseed
	MD_Install_Packages
	#MD_Populate_Debcache
	MD_Cleanup
	Create_Diskless_Tar
	Create_PXE_Initramfs_Vmlinuz

	StatsMessage "END: $TARGET_DISTRO - $TARGET_RELEASE - $TARGET_ARCH"

done

StatsMessage "END: Running diskless create for :$TARGET_TYPES:"

#Disable trap, everything was umounted and removed in the cleanup function
trap - EXIT

StatsMessage "Diskless media director images setup completed without a detected issue!"

#Exit successfully
exit 0
