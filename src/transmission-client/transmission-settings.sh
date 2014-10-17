#/bin/bash

USAGE="Usage: `basename $0` getSettings|createLink Target Linkname|getrar file|extractRar rar-file folder-to-extract-to|deleteLink link-Location|getRarVolume rar-file|extractRarfiles rar-file folder-to-extract-to {IDs-in-database}|startstoptransmission start/stop/restart/status|changeSetting Setting-Name Setting-Value|torrentComplete torrent-file-name"

transmissionSettings="/etc/transmission-daemon/settings.json"

getSettings()
{
	grep "$1" $transmissionSettings|awk -F\: '{print $2}'|sed -e 's|\"||g' -e 's|\,||g' -e 's|^ *||g' -e 's| *$||g'
}

changeSetting()
{
        #create a back-up of the current settings file
        backup="_"`date '+%Y%m%d%k%M%S'`".bak"
        oldSetting=$(grep "\"$1\":" $transmissionSettings|awk -F\: '{print $2}'|sed -e 's|\,||g' -e 's|^ *||g' -e 's| *$||g' -e 's|"|\\"|g')

        if [ `echo ${oldSetting:1:1}` == '"' ]
        then
                sed -i$backup "s|\"$1\": $oldSetting,|\"$1\": \"$2\",|" $transmissionSettings
        else
                sed -i$backup "s|\"$1\": $oldSetting,|\"$1\": $2,|" $transmissionSettings
        fi

        echo $transmissionSettings$backup
}

deleteLink()
{
	if [ `echo $1|wc -c` -gt 0 ]
	then
		if [ -L "$1" ]
		then
			rm "$1"
		fi
	fi
}

createLink()
{
	ln -s "$1" "$2"
}

getRar()
{
	unrar lt -v "$1"|grep Volume|sed -e 's|Volume\ ||g'
}

getRarVolume()
{
	unrar l "$1"
}

extractRar()
{
	unrar x -o+ "$1" "$2"

	mysql -e "UPDATE Torrent_Links SET Link_Status=NULL WHERE Link_ID=$3;" transmission_links
}

extractRarfiles()
{
	rararchive=$1
	shift
	extractDir=$1
	shift

	local IDs={ $@ }

	for ID in ${IDs[@]}
	do
		#Get the original file name from the database
		fileName=`mysql -e "SELECT Rar_File FROM Torrent_Links WHERE Link_ID=$ID;" transmission_links|tail -1`
		#Get the required file name from the database
		newName=`mysql -e "SELECT Link_Name FROM Torrent_Links WHERE Link_ID=$ID;" transmission_links|tail -1`

		#Extract the required file
		unrar x -o+ "$rararchive" "$extractDir" "$fileName"
		#Re-name the file to the new name
		mv "$extractDir$fileName" "$extractDir$newName"

		#update the database entry to show the rar extract has completed.
		mysql -e "UPDATE Torrent_Links SET Link_Status=NULL WHERE Link_ID=$ID;" transmission_links
	done
}

startstoptransmission()
{
	case "$1" in
		"start")
			service transmission-daemon start;
			;;
		"stop")
			service transmission-daemon stop;
			;;
		"restart")
			service transmission-daemon restart;
			;;
		"status")
			ps -ef|grep transmission-daemon|grep -v grep|awk '{print $2}';
			;;
		*)
			echo "Invalid Service argument";
			;;
	esac
}

case "$1" in
	"getSettings")
		getSettings "$2";
		;;
	"createLink")
		createLink "$2" "$3";
		;;
	"deleteLink")
		deleteLink "$2";
		;;
	"getRar")
		getRar "$2";
		;;
	"getRarVolume")
		getRarVolume "$2";
		;;
	"extractRar")
		extractRar "$2" "$3" $4;
		;;
	"extractRarfiles")
		shift;
		rarArchive=$1
		shift;
		extractDir=$1
		shift;

		extractRarfiles "$rarArchive" "$extractDir" "$@";
		;;
	"startstoptransmission")
		startstoptransmission "$2";
		;;
	"changeSetting")
		changeSetting "$2" "$3";
		;;
	"torrentComplete")
		/usr/pluto/bin/MessageSend dcerouter -targetType category -bl 5 1 5 1 809 251 0 9 "Transmission Torrent Download completed\n$2" 182 15 70 "Alert!"
		;;
	*)
		echo $USAGE >&2
		exit 1
		;;
esac
