#!/bin/bash

. /usr/pluto/install/Common.sh
. /usr/pluto/install/AptSources.sh
if [[ -f /usr/pluto/bin/Config_Ops.sh ]]; then
	. /usr/pluto/bin/Config_Ops.sh
fi
if [[ -f /usr/pluto/bin/SQL_Ops.sh ]]; then
	. /usr/pluto/bin/SQL_Ops.sh
else
	RunSQL () {
		mysql -A -N "$SQL_DB" $MYSQL_DB_CRED -e "$1;" | tr '\n\t ' $'\x20'$'\x01'$'\x02' | sed 's/ *$//'
	}

	Field () {
		local Row FieldNumber
		FieldNumber="$1"; shift
		Row="$*"
		echo "$Row" | cut -d$'\x01' -f"$FieldNumber" | tr $'\x02' ' '
	}
fi



set -x

DISTRO=$(lsb_release -is)
case "$DISTRO" in
	Debian)
		SECTIONS="main non-free contrib"
		;;
	Ubuntu)
		SECTIONS="main restricted universe multiverse"
esac

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
		Version=$(awk "/^Package: $Pkg\$/,/^\$/" /var/lib/dpkg/status | grep '^Version: ' | cut -d' ' -f2-)
		#Version=$(dpkg -s "$PKG_NAME" | grep ^Version: | cut -d' ' -f2-)
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
		## Display a bootsplash message
		echo "Installing: $PKG_NAME"
		/usr/pluto/bin/BootMessage.sh "Installing: $PKG_NAME" 2>/dev/null || /bin/true
		if PackageIsInstalled "$PKG_NAME"; then
			AddPackageDevice "$PK_Device" "$PK_PACKAGE"
			exit 0
		fi

		dpkg --configure -a --force-confold

		if [[ -z "$REPOS" ]]; then
			echo "$0: Blank repository entry" >&2
			exit $ERR_APT
		fi

		TMP_SECT=$(echo -n "$REPOS" | cut -d' ' -sf2-)
		if [ -n "$TMP_SECT" ]; then
			SECTIONS="$TMP_SECT"
			REPOS=$(echo -n "$REPOS" | cut -d' ' -f1)
		fi

		lsbrelease=$(lsb_release -cs)
		if [ "$REPOS" = "20dev_ubuntu" ] || [ "$REPOS" = "20dev_debian" ]; then 
			REPOS="$lsbrelease"
		fi

		SingleEndSlash='s!//*!/!g; s!/*$!/!g; s!^http:/!http://!g; s!^ftp:/!ftp://!g'
#		FilteredRepos=$(echo "$REPOS_SRC" | sed 's/[^A-Za-z0-9_./+=:-]/-/g; '"$SingleEndSlash")
		EndSlashRepos=$(echo "$REPOS_SRC" | sed "$SingleEndSlash")

#		echo "Repository test string: '$FilteredRepos.+$REPOS.+$SECTIONS'"
#		results=$(cat /etc/apt/sources.list | sed "$SPACE_SED" | egrep -v "^#" | egrep -c -- "$FilteredRepos.+$REPOS.+$SECTIONS" 2>/dev/null)

		AptSrc_ParseSourcesList
		AptSrc_SourceExists "deb $EndSlashRepos $REPOS $SECTIONS"
		retval=$?
		if [ "$retval" -ne 0 ]; then
			# Query for current RepositorySource
			# Use Repository ID to get other RepositorySource_URLs for this RepositorySource
			# If any of them match, consider the source to be found.
			Q="SELECT FK_RepositorySource FROM RepositorySource_URL WHERE URL LIKE 'deb $EndSlashRepos' ORDER BY FK_RepositorySource DESC LIMIT 1"
			R=$(RunSQL "$Q")
			PK_RepositorySource=$(Field 1 "$R")
			Q="SELECT URL from RepositorySource_URL WHERE FK_RepositorySource='$PK_RepositorySource'"
			R=$(RunSQL "$Q")
			for URL_SRC in $R ; do
				SRC=$(Field 1 "$URL_SRC")
				URL="$(echo $SRC | sed "$SPACE_SED; s/^deb *//")"
				EndSlashURL=$(echo "$URL" | sed "$SingleEndSlash")
				AptSrc_SourceExists "deb $EndSlashURL $REPOS $SECTIONS"
				retval=$?
				if [ "$retval" -eq 0 ]; then
					break;
				fi
			done
		fi

		if [ "$retval" -ne 0 ]; then
			if [ "$REPOS" = "$lsbrelease" ]; then
				if AptSrc_AddSource "deb $EndSlashRepos $REPOS $SECTIONS"; then
					if ! BlacklistConfFiles '/etc/apt/sources.list' ;then
						AptSrc_WriteSourcesList >/etc/apt/sources.list
					fi
					if [[ "$InternetConnection" != NoInternetConnection ]]; then
						apt-get update
					fi
				fi
			else
				echo "Repository does not match current distro - skipping update of the sources.list file"
			fi
		else
			echo "Repository already exists in sources.list file."
		fi

		if ! PackageIsInstalled "$PKG_NAME"; then

			apt-get -y install "$PKG_NAME"
			apt_err=$?

			## If fails the first time try once more
			if [[ "$apt_err" != "0" ]] ;then
				apt-get -f -y install
				apt-get -y --reinstall install "$PKG_NAME"
                                apt_err=$?
			fi


			if [[ "$apt_err" != "0" ]] ;then
				echo "Problem installing $PKG_NAME ($apt_err)"
				exit $apt_err
			fi

			#unset http_proxy
			#apt-get clean
		fi

		AddPackageDevice "$PK_Device" "$PK_PACKAGE"
	;;

	direct)
		if [ "$REPOS_TYPE" -eq 1 ] && PackageIsInstalled "${PKG_NAME/_*}"; then
			true
		else
			mkdir -p /usr/pluto/download
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
		exit 0
		/usr/pluto/install/Download_SVN.sh "$@"
		Ret=$?
		if [[ "$Ret" -eq 0 ]]; then
			AddPackageDevice "$PK_Device" "$PK_PACKAGE"
		fi
		exit $Ret
	;;

	cvs)
		exit 0
		/usr/pluto/install/Download_CVS.sh "$@"
		Ret=$?
		if [[ "$Ret" -eq 0 ]]; then
			AddPackageDevice "$PK_Device" "$PK_PACKAGE"
		fi
		exit $Ret
	;;
esac

/usr/pluto/bin/BootMessage.sh ""

exit 0
