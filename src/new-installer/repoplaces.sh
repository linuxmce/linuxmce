#!/bin/bash
env 
sad=$SUDO_USER
	if ! grep -q LMCE_Data /home/$(sad)/.kde/share/apps/kfileplaces/bookmarks.xml; then  #Checks to see if instance exists and if not creates
wget http://i52.tinypic.com/2zs8nye.png -nc -O /usr/share/icons/oxygen/128x128/places/folder-lmce.png
wget http://i54.tinypic.com/1isjq.png -nc -O /usr/share/icons/oxygen/64x64/places/folder-lmce.png
wget http://i55.tinypic.com/15phpag.png -nc -O /usr/share/icons/oxygen/48x48/places/folder-lmce.png
wget http://i52.tinypic.com/1tpx68.png -nc -O /usr/share/icons/oxygen/32x32/places/folder-lmce.png
wget http://i51.tinypic.com/357ke8y.png -nc -O /usr/share/icons/oxygen/22x22/places/folder-lmce.png
wget http://i52.tinypic.com/2jdjd5l.png -nc -O /usr/share/icons/oxygen/16x16/places/folder-lmce.png
 	n=$(grep \<ID /home/$sad/.kde/share/apps/kfileplaces/bookmarks.xml | wc -l)   #Counts instances of ID
cat /home/$sad/.kde/share/apps/kfileplaces/bookmarks.xml > /home/$sad/.kde/share/apps/kfileplaces/oldbookmarks.xml  #Makes a backup copy
sed -i '/<[\/]xbel>/d' /home/$sad/.kde/share/apps/kfileplaces/bookmarks.xml  #Removes the close tag at the end of xml file
while IFS='<>' read _ starttag value endtag; do	 #Seeks out the ID number
   case "$starttag" in 
       ID) Ids+=("$value") ;      
	e=$(echo $Ids | sed 's/\(.\)/\1 /g') ; 	#Makes a variable to be separated by char
	a=($e)
   esac
done < /home/$sad/.kde/share/apps/kfileplaces/bookmarks.xml  
echo '<bookmark href="file:///home/public/data" > 
 <title>LMCE_Data</title>
 <info>
  <metadata owner="http://freedesktop.org" >
   <bookmark:icon name="folder-lmce"/>
  </metadata>
  <metadata owner="http://www.kde.org" >
   <ID>'"${a[0]}${a[1]}${a[2]}${a[3]}${a[4]}${a[5]}${a[6]}${a[7]}${a[8]}${a[9]}${a[10]}${n}"'</ID>
   <isSystemItem>true</isSystemItem>
  </metadata>
 </info> 
</bookmark> 
</xbel>' >> /home/$sad/.kde/share/apps/kfileplaces/bookmarks.xml  #Appends entry into the file replacing end tag.
fi
echo "Dolphin's Places updated"
repoclean() {
n1=$(grep -e 'security.' -e 'archive.' /etc/apt/sources.list | wc -l) #looks for references that shouldn't be there and counts them
o1=0
	if [ "$n1" -gt "$o1" ] ; then echo "Changing repositories..."
sed -i.backup -e 's/\/.*.archive/\/\/old-releases/' -e 's/security.ubuntu/old-releases.ubuntu/' -e 's/\/archive/\/old-releases/' /etc/apt/sources.list; #changes us.archive, security.ubuntu, and /archive to old-releases. 
grep -v '^#' /etc/apt/sources.list |  cat > /etc/apt/sources.tmp #removes duplicates in temp file
cat /etc/apt/sources.tmp > /etc/apt/sources.list; #rewrites sources.list without comments
else echo "Repositories clean"
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
