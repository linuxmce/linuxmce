#!/bin/bash

exec &>/dev/null </dev/null
export DISPLAY=:0
echo ""
echo "Linux MCE Installer Starting ... please wait"
echo ""

# create a temp directory to extract to.
export WRKDIR=`mktemp -d /tmp/selfextract.XXXXXX`

SKIP=`awk '/^__ARCHIVE_FOLLOWS__/ { print NR + 1; exit 0; }' $0`

# Take the TGZ portion of this file and pipe it to tar.
tail -n +$SKIP $0 | tar xz -C $WRKDIR

# execute the installation script

PREV=`pwd`
cd $WRKDIR
export LD_LIBRARY_PATH=$WRKDIR
export PATH=$PATH:$WRKDIR
if [[ $UID != "0" ]] ;then
	mv ./mce-installer ./mce-installer-user
	echo "#!/bin/bash
	      export PATH=$PATH:$WRKDIR
              LD_LIBRARY_PATH=$WRKDIR ./mce-installer-user
	      " > ./mce-installer
	chmod +x ./mce-installer
	./mce-installer-user
else
	mkdir -p /home/public/data/samples || :
	cp ./sample.mpg /home/public/data/samples/ || :
	./mce-installer
fi


# delete the temp files
cd $PREV
rm -rf $WRKDIR

exit 0

__ARCHIVE_FOLLOWS__
