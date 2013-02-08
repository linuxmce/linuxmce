#!/bin/bash


. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

startid="0"
range="1000"
webhost="$DCERouter"
wgetlist="/tmp/wget-list"
pidfile="/tmp/coverart_is_running"
wegotlynx=$(dpkg -l lynx | tail -1 | awk '{print $1}')

if [[ "$wegotlynx" != "ii" ]];then
        apt-get -f -y install lynx 1>/dev/null 2>/dev/null
fi

case "$1" in
"background")

		trap "rm -rf $pidfile" EXIT

		if [ -f $pidfile ]; then
			pidid=$(cat $pidfile)
			echo "Daemon allready running, PID $pidid !"
			exit 0
		fi
	
		echo $$ > $pidfile
		UseDB "pluto_media"
  		maxfiles=$(RunSQL "SELECT PK_File AS Filename FROM File LEFT JOIN Picture_File ON Picture_File.FK_File=PK_File LEFT JOIN CoverArtScan ON CoverArtScan.FK_File=PK_File WHERE EK_MediaType IN (3,4,5,27,28) AND (Scanned IS NULL OR Scanned=0) AND Missing = 0 AND IsDirectory=0 GROUP BY PK_File HAVING count(FK_Picture)=0 ;" | wc -w)
	
		while [ $startid -lt $maxfiles ]; do
			lynx -dump -width=300 "http://$webhost/lmce-admin/genwget.php?from=$startid&range=$range" > $wgetlist
			
			cat $wgetlist | 
			while read line ;do
				wget "http://$webhost/lmce-admin/$line" --header="User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.4) Gecko/20060508 Firefox/1.5.0.4" -O -
				sleep 2
			done
			
			startid=$(( $startid + $range ))
		done

	;;

"start")
	echo "Starting automatic daemon."
	screen -m -d -S grabcoverart /bin/bash /usr/pluto/bin/grabcoverart.sh background
	
	;;
"stop")
	if [ -f $pidfile ]; then
		pidid=$(cat $pidfile)
		echo "Killing PID $pidid !"
		kill -9 $pidid 1>/dev/null 2>/dev/null
		rm -rf $pidfile 1>/dev/null 2>/dev/null
	else 
		echo "Coverart daemon is stopped."
	fi
	


	;;
"status")	

	if [ -f $pidfile ]; then
	pidid=$(cat $pidfile)
	echo "Daemon allready running, PID $pidid !"
	else
	echo "Coverart daemon is stopped."
	fi

	;;
*)
	echo "Usage $0 [start|status|stop]"	
	;;
esac
