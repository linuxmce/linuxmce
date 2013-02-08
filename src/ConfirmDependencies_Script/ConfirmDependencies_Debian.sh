#!/bin/bash
echo $0 $*

. /usr/pluto/install/Common.sh
. /usr/pluto/bin/Utils.sh
if [[ -f /usr/pluto/bin/Config_Ops.sh ]]; then
	. /usr/pluto/bin/Config_Ops.sh
fi

set -x

SECTIONS="main non-free contrib"

#status=$(dpkg -l "$PKG_NAME" | grep '^ii' | awk '{print $1}') #| while read status; do break; done
#if [ "$status" == "ii" ]; then
#	echo "Package is installed"
#	exit 0
#fi

AddPackageDevice()
{
	local PK_Device="$1"
	local PK_PACKAGE="$2"

	if [[ -n "$PK_Device" && -n "$PK_PACKAGE" ]]; then
		Version=$(dpkg -s "$PKG_NAME" | grep ^Version: | cut -d' ' -f2-)
		DateTime=$(date +'%Y-%m-%d %k:%M:%S')
		Q="INSERT INTO Package_Device(FK_Package, FK_Device, Version, InstallDate) VALUES('$PK_PACKAGE', '$PK_Device', '$Version', '$DateTime')"
		echo "$Q;" | /usr/bin/mysql -h $MySqlHost -u $MySqlUser pluto_main &>/dev/null || /bin/true
	fi
}

case "$REPOS_TYPE" in
	# Package (APT, http/ftp: .deb)
	1) ;;
	
	# CVS (Concurrent Versioning System)
	3) URL_TYPE=cvs ;;
	
	# SVN (SubVersioN)
	4) URL_TYPE=svn ;;
	
	# HTTP/FTP download (archives: .tar.gz, .zip)
	6) ;;
	
	# Other (not supported)
	*)
		echo "TODO: implement repository type $REPOS_TYPE handling"
		exit $ERR_UNKNOWN_REP_TYPE
	;;
esac

if [ -z "$URL_TYPE" ]; then
	case "$REPOS_SRC" in
		deb*)
			URL_TYPE=apt;
			REPOS_SRC="$(echo $REPOS_SRC | sed "$SPACE_SED; s/^deb *//")"
		;;
		http://*|ftp://*) URL_TYPE=direct ;;
		*) [ -z "$URL_TYPE" ] && echo "Unknown URL form in '$REPOS_SRC'" && exit $ERR_UNKNOWN_REPOS_SRC_FORM ;;
	esac
fi

# could be replaced with a "</dev/zero" for each command...
keep_sending_enters()
{
	set +x
	while : ; do echo; done
	set -x
}

export DEBIAN_FRONTEND=noninteractive

case "$URL_TYPE" in
	apt)
		if [[ -z "$REPOS" ]]; then
			echo "$0: Blank repository entry" >&2
			exit $ERR_APT
		fi
		
		TMP_SECT=$(echo -n "$REPOS" | cut -d' ' -sf2-)
		if [ -n "$TMP_SECT" ]; then
			SECTIONS="$TMP_SECT"
			REPOS=$(echo -n "$REPOS" | cut -d' ' -f1)
		fi
		
		SingleEndSlash='s!//*!/!g; s!/*$!/!g; s!^http:/!http://!g; s!^ftp:/!ftp://!g'
		FilteredRepos=$(echo "$REPOS_SRC" | sed 's/[^A-Za-z0-9_./+=:-]/-/g; '"$SingleEndSlash")
		EndSlashRepos=$(echo "$REPOS_SRC" | sed "$SingleEndSlash")
#		echo "Repository test string: '$FilteredRepos.+$REPOS.+$SECTIONS'"
		results=$(cat /etc/apt/sources.list | sed "$SPACE_SED" | egrep -v "^#" | egrep -c -- "$FilteredRepos.+$REPOS.+$SECTIONS" 2>/dev/null)
		if [ "$results" -eq 0 ]; then
			if ! BlacklistConfFiles '/etc/apt/sources.list' ;then
				if [ ! -e /etc/apt/sources.list.pbackup ] ;then
					cp /etc/apt/sources.list /etc/apt/sources.list.pbackup
				fi
				echo "deb $FilteredRepos $REPOS $SECTIONS" >>/etc/apt/sources.list
				apt-get update
			fi
#			[ "$Type" == "router" ] && apt-proxy-import-simple /usr/pluto/install/deb-cache
		fi

		if ! PackageIsInstalled "$PKG_NAME"; then
			#export http_proxy="http://dcerouter:8123"
			echo "Installing $PKG_NAME from $REPOS" >>/dev/tty1
			if ! keep_sending_enters | apt-get -t "$REPOS" -y --reinstall install "$PKG_NAME"; then
				echo "$0: Apt error" >&2
				echo "$0: Apt error $ERR_APT">>/dev/tty1
				exit $ERR_APT
			fi

			#unset http_proxy
#			apt-get clean
		fi
		
		AddPackageDevice "$PK_Device" "$PK_PACKAGE"
	;;
	
	direct)
		if [ "$REPOS_TYPE" -eq 1 ] && PackageIsInstalled "${PKG_NAME/_*}"; then
			true
		else
			mkdir -p /usr/pluto/download
			echo "Downloading $@">>/dev/tty1
			/usr/pluto/install/Download_Direct.sh "$@" || exit $ERR_DOWNLOAD
		
			if [ "$REPOS_TYPE" -eq 1 ]; then
				#export http_proxy="http://dcerouter:8123"
				keep_sending_enters | dpkg -i /usr/pluto/download/"$PKG_NAME" || exit $ERR_DPKG_INSTALL
				#unset http_proxy
			else
				# about the finds: should be ran in the extracted package's directory
				mkdir -p "/usr/pluto/download/${PKG_NAME}"
				pushd "/usr/pluto/download/${PKG_NAME}"
				if [ "$PARAMETER" == ".zip" ]; then
					echo "Extracting '${PKG_NAME}_$MIN_VER$PARAMETER'"
					/usr/bin/unzip "../${PKG_NAME}_$MIN_VER$PARAMETER" || exit $ERR_UNPACK
					find -name 'mkr_preinst*' -exec chmod +x '{}' ';' -exec '{}' ';'
					find -name 'mkr_postinst*' -exec chmod +x '{}' ';' -exec '{}' ';'
				elif [ "$PARAMETER" == ".tar.gz" ]; then
					echo "Extracting '${PKG_NAME}_$MIN_VER$PARAMETER'"
					/bin/tar -xzvf "../${PKG_NAME}_$MIN_VER$PARAMETER" || exit $ERR_UNPACK
					find -name 'mkr_preinst*' -exec chmod +x '{}' ';' -exec '{}' ';'
					find -name 'mkr_postinst*' -exec chmod +x '{}' ';' -exec '{}' ';'
				else
					echo "Unknown parameter: '$PARAMETER'"
				fi
				popd
		
				if [ "$REPOS_TYPE" -eq 6 ]; then
					# source code ; how do I know where it unpacked?
					true
				fi
			fi
		fi
	;;

	svn)
		/usr/pluto/install/Download_SVN.sh "$@"
		Ret=$?
		if [[ "$Ret" -eq 0 ]]; then
			AddPackageDevice "$PK_Device" "$PK_PACKAGE"
		fi
		exit $Ret
	;;

	cvs)
		/usr/pluto/install/Download_CVS.sh "$@"
		Ret=$?
		if [[ "$Ret" -eq 0 ]]; then
			AddPackageDevice "$PK_Device" "$PK_PACKAGE"
		fi
		exit $Ret
	;;
esac

exit 0
