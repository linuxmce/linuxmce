#!/bin/bash

set -e
#set -x

#PROXY="http://10.10.42.99:3142"
#http_proxy="http://10.10.42.99:3142"

###########################################################
### Setup global variables
###########################################################
DEVICEDATA_Operating_System=209
DEVICEDATA_Architecture=112

###TARGET_TYPES="ubuntu-i386 raspbian-armhf"
#TARGET_TYPES="raspbian-armhf"
TARGET_TYPES="ubuntu-i386"

INSTALL_KUBUNTU_DESKTOP="yes"
#INSTALL_KUBUNTU_DESKTOP="no"

HOST_DISTRO=$(lsb_release -i -s | tr '[:upper:]' '[:lower:]')
HOST_RELEASE=$(lsb_release -c -s)
HOST_ARCH=$(apt-config dump | grep 'APT::Architecture' | sed 's/.*"\(.*\)".*/\1/g' | head -1)

#Assign latest installed kernel and not running kernel
HOST_KVER=$(find /lib/modules/* -maxdepth 0 -type d |sort -r |head -1)

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
	umount -fl $TEMP_DIR/var/cache/apt || :
	umount -fl $TEMP_DIR/usr/pluto/deb-cache
	umount -fl $TEMP_DIR/dev/pts
	umount -fl $TEMP_DIR/dev || :
	umount -fl $TEMP_DIR/proc
	umount -fl $TEMP_DIR/sys

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
				deb http://security.ubuntu.com/ubuntu/ $TARGET_RELEASE-security main restricted universe multiverse
				EOF
			;;
		"raspbian")
			StatsMessage "Setting up /etc/apt/sources.list for raspbian"
			cat <<-EOF > $TEMP_DIR/etc/apt/sources.list
				#deb http://10.10.42.99/raspbian/ ./
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
	LC_ALL=C chroot $TEMP_DIR apt-get -f -y install sysv-rc screen
	case "$TARGET_DISTRO" in
		"ubuntu")
			LC_ALL=C chroot $TEMP_DIR apt-get -f -y install upstart
		;;
	esac

	echo "Disable all service invocation"
	# disable service invocation and tools
	mv -f $TEMP_DIR/sbin/start{,.orig} || :
	mv -f $TEMP_DIR/usr/sbin/invoke-rc.d{,.orig} || :
	mv -f $TEMP_DIR/sbin/restart{,.orig} || :
	mv -f $TEMP_DIR/sbin/initctl{,.orig} || :

	cat <<-EOF > $TEMP_DIR/sbin/start
		#!/bin/bash
		exit 0
		EOF
	chmod +x $TEMP_DIR/sbin/start
	cp $TEMP_DIR/sbin/start $TEMP_DIR/sbin/invoke-rc.d
	cp $TEMP_DIR/sbin/start $TEMP_DIR/sbin/restart
	cp $TEMP_DIR/sbin/start $TEMP_DIR/sbin/initctl

	### Disable invoke-rc.d scripts
	mv "$TEMP_DIR"/sbin/start-stop-daemon{,.pluto-install}
	[[ -f "$TEMP_DIR"/sbin/initctl ]] && mv "$TEMP_DIR"/sbin/initctl{,.pluto-install}
	echo -en '#!/bin/bash\necho "WARNING: we dont want invoke-rc.d to run right now"\n' >"$TEMP_DIR"/usr/sbin/invoke-rc.d
	echo -en '#!/bin/bash\necho "WARNING: fake start called"\n' >"$TEMP_DIR"/sbin/start
	echo -en '#!/bin/bash\necho "WARNING: fake start-stop-daemon called"\n' >"$TEMP_DIR"/sbin/start-stop-daemon
	echo -en '#!/bin/bash\necho "WARNING: fake initctl called"\n' >"$TEMP_DIR"/sbin/initctl
	chmod +x "$TEMP_DIR/usr/sbin/invoke-rc.d"
	chmod +x "$TEMP_DIR/sbin/start-stop-daemon"
	chmod +x "$TEMP_DIR/sbin/start"
	chmod +x "$TEMP_DIR/sbin/initctl"

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
			echo "LTS_HES=$TARGET_KVER_LTS_HES" >> $TEMP_DIR/etc/pluto.conf
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
			# raspbian doesn't come with lsb-release by default???
			LC_ALL=C chroot "$TEMP_DIR" apt-get -y install lsb-release
			cat <<-EOF > $TEMP_DIR/etc/lsb-release
				DISTRIB_ID=Raspbian
				DISTRIB_CODENAME=$TARGET_RELEASE
				EOF

			# HACK: copy the foundation kernel.img to a normal linux kernal name with version
			# FIXME: is there a better way to do this?  the raspbian kernels are missing the fdt.
			#LC_ALL=C chroot "$TEMP_DIR" apt-get -y install linux-image-3.6-trunk-rpi
			LC_ALL=C chroot "$TEMP_DIR" apt-get -y install libraspberrypi-bin raspberrypi-bootloader
			#LC_ALL=C chroot "$TEMP_DIR" apt-get -y install rpi-update
			#LC_ALL=C chroot "$TEMP_DIR" rpi-update
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
			#DEVICE_LIST="2216 2278 2259 11 26 1808 2122"
			;;
	esac

	# Determine if MythTV is installed by looking for MythTV_Plugin...
	# Don't install mythtv into raspbian
	if [[ "$TARGET_DISTRO" != "raspbian" ]]; then
		Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=36"
		MythTV_Installed=$(RunSQL "$Q")
		if [ $MythTV_Installed ];then
			#MythTV_Plugin is installed, so install MythTV_Player on MD
			DEVICE_LIST="$DEVICE_LIST 35"
		fi
	fi
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
	case "$TARGET_ARCH" in
		i386|amd64)
			## Packages that are marked as dependencies only in the database
			LC_ALL=C chroot $TEMP_DIR apt-get -y install id-my-disc
			;;
	esac

	## Put back discover
	#mv "$TEMP_DIR"/sbin/discover.disabled "$TEMP_DIR"/sbin/discover

	## Install additional packages
	case "$TARGET_DISTRO" in
		"ubuntu")
			## If libdvdcss2 is installed on the hybrid/core
			if [[ -d /usr/share/doc/libdvdcss2 ]] ;then
				pushd $TEMP_DIR >/dev/null
				LC_ALL=C chroot $TEMP_DIR apt-get -y install libdvdcss2
				VerifyExitCode "Installation of libdvdcss2 failed"
				popd >/dev/null
			fi

			if [[ "$INSTALL_KUBUNTU_DESKTOP" != "no" ]]; then
				LC_ALL=C chroot $TEMP_DIR apt-get -y install kubuntu-desktop
				#LC_ALL=C chroot $TEMP_DIR apt-get -y install kde-minimal
			fi
			echo '/bin/false' >"$TEMP_DIR/etc/X11/default-display-manager"
			mkdir -p $TEMP_DIR/etc/init/
			echo 'manual' > "$TEMP_DIR/etc/init/kdm.override"
			echo 'manual' > "$TEMP_DIR/etc/init/sddm.override"
			echo 'manual' > "$TEMP_DIR/etc/init/lightdm.override"

			# Update startup to remove kdm and network manager
			LC_ALL=C chroot $TEMP_DIR update-rc.d -f kdm remove || :
			LC_ALL=C chroot $TEMP_DIR update-rc.d -f sddm remove || :
			LC_ALL=C chroot $TEMP_DIR update-rc.d -f NetworkManager remove

			#Install ancillary programs
			LC_ALL=C chroot $TEMP_DIR apt-get -y install xserver-xorg"$TARGET_KVER_LTS_HES"
			VerifyExitCode "xserver-xorg$TARGET_KVER_LTS_HES install failed"
			LC_ALL=C chroot $TEMP_DIR apt-get -y install xserver-xorg-video-all"$TARGET_KVER_LTS_HES" linux-firmware
			VerifyExitCode "Ancillary programs install failed"

			## Install plymouth theme on MD in Ubuntu
			#LC_ALL=C chroot $TEMP_DIR apt-get -y install lmce-plymouth-theme
			#VerifyExitCode "MCE plymouth theme install failed"

	                # upstart fix to prevent waiting on MD boot
	                sed -i".pbackup" 's/ and static-network-up//g' "$TEMP_DIR/etc/init/rc-sysinit.conf"
			;;
		"raspbian")
			#FIXME: why is this required, something missing?
			#LC_ALL=C chroot $TEMP_DIR addgroup --force-badname Debian-exim
			#VerifyExitCode "addgroup Debian-Exim failed"
			LC_ALL=C chroot "$TEMP_DIR" sed -i '/Debian-exim/d' /var/lib/dpkg/statoverride
			;;
	esac

	#Install nfs-common and openssh-server
	LC_ALL=C chroot "$TEMP_DIR" apt-get -y install nfs-common openssh-server
	VerifyExitCode "nfs-common or openssh-server programs install failed"

	LC_ALL=C chroot "$TEMP_DIR" apt-get -y install xinit
	VerifyExitCode "xinit"

	LC_ALL=C chroot "$TEMP_DIR" apt-get -y install pastebinit
	VerifyExitCode "pastebinit"

	#implement external_media_identifier fix
	LC_ALL=C chroot $TEMP_DIR ln -s /usr/lib/libdvdread.so.4 /usr/lib/libdvdread.so.3
}

MD_Populate_Debcache () {
	/usr/pluto/bin/UpdateDebCache.sh
}

MD_Cleanup () {
	StatsMessage "Cleaning up from package installations..."
	umount $TEMP_DIR/var/cache/apt || :
	umount $TEMP_DIR/usr/pluto/deb-cache
	umount $TEMP_DIR/dev/pts
	umount $TEMP_DIR/sys
	umount $TEMP_DIR/proc

	#Make sure there is are Packages files on the MD so apt-get update does not fail
	mkdir -p $TEMP_DIR/usr/pluto/deb-cache/$DEB_CACHE
	echo "" > $TEMP_DIR/usr/pluto/deb-cache/$DEB_CACHE/Packages
	gzip -9c < $TEMP_DIR/usr/pluto/deb-cache/$DEB_CACHE/Packages > $TEMP_DIR/usr/pluto/deb-cache/$DEB_CACHE/Packages.gz
	echo "" > $TEMP_DIR/usr/pluto/deb-cache/Packages
	gzip -9c < $TEMP_DIR/usr/pluto/deb-cache/Packages > $TEMP_DIR/usr/pluto/deb-cache/Packages.gz

	mv -f $TEMP_DIR/sbin/invoke-rc.d{.orig,} || :
	mv -f $TEMP_DIR/sbin/start{.orig,} || :
	mv -f $TEMP_DIR/sbin/restart{.orig,} || :
	mv -f $TEMP_DIR/sbin/initctl{.orig,} || :
	mv -f "$TEMP_DIR"/sbin/start-stop-daemon{.pluto-install,} || :
	#mv -f "$TEMP_DIR"/sbin/initctl{.pluto-install,}
	#rm -f "$TEMP_DIR"/usr/sbin/policy-rc.d

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
	pushd "$TEMP_DIR" >/dev/null
	tar -cJf "$ARCHIVE_DIR/$DisklessFS" *
#	tar -czf "$ARCHIVE_DIR/$DisklessFS" *
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

#TODO get as much of this from database as possible
for TARGET in "$TARGET_TYPES" ; do
	case "$TARGET" in
		"ubuntu-i386")
			if [[ "$HOST_ARCH" == "armhf" ]] ; then
				echo "Cannot build i386 image on ${HOST_ARCH} core."
				exit 1
			fi
			TARGET_DISTRO="ubuntu"
			TARGET_RELEASE="$HOST_RELEASE"
			TARGET_ARCH="i386"
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
		"raspbian-armhf")
			TARGET_DISTRO="raspbian"
			TARGET_RELEASE="wheezy" #TODO: get from ?
			TARGET_ARCH="armhf"
			TARGET_REPO="http://archive.raspbian.org/raspbian/"
			DBST_ARCHIVE="LMCEMD_Debootstraped-raspbian-armhf.tar.xz"
			DisklessFS="LMCEMD-$TARGET_DISTRO-$TARGET_ARCH.tar.xz"
			TARGET_DISTRO_ID=19
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

#Disable trap, everything was umounted and removed in the cleanup function
trap - EXIT

StatsMessage "Diskless media director images setup completed without a detected issue!"

#Exit successfully
exit 0
