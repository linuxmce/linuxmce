#
# This file is called by /etc/init.d/vdr
#

# check wich remote config files to load
activeremotes() 
{
    REMOTES=""
    if [ -e "/var/run/lircd.pid" ]; then
        if ps --no-headers --pid `cat /var/run/lircd.pid` | grep -q "lircd" ; then
            REMOTES="LIRC"
        fi
    fi
    if [ "$KEYB_TTY" != "" ]; then
        REMOTES="$REMOTES KBD"
    fi
    if [ -e "$PLUGIN_CFG_DIR/plugin.remote.conf" ]; then
        REMOTES="$REMOTES remote-event"
    fi
}

# merge remote.<name>.conf into one file
getremotes()
{
    echo -ne "\nChecking remotes"
    echo "" > $CFG_DIR/remote.conf
    activeremotes
    for i in $REMOTES; do
        echo -n " $i"
        if [ -e "$CFG_DIR/remote.$i.conf" ]; then
            if  [ $i = "remote-event" ]; then
                for device in {"event0","event1","event2","event3"} ; do
                    cat "$CFG_DIR/remote.$i.conf" | \
                        sed "s/remote-[^.]*/remote-$device/" | \
                        sort | uniq >>$CFG_DIR/remote.conf
                done
            else
                cat "$CFG_DIR/remote.$i.conf" >>$CFG_DIR/remote.conf
            fi
            echo "" >> $CFG_DIR/remote.conf
        fi
    done
}

# rewrite probably changed remote config file
putremotes ()
{
    proc_devices="/proc/bus/input/devices"
    if [ -e $proc_devices ]; then
       device=`cat $proc_devices | \
           awk -v FS="\n" -v RS="\n\n+" '$2 ~ /N: Name="DVB/ { print $4 }' | \
           cut -d " " -f3`
    else
        # be sure to catch the right device if relearned
        device=`grep "^remote-event" $CFG_DIR/remote.conf | head -1 | cut -c13`
        device="event$device"
    fi

    echo -ne "\nRewriting remote changes"
    activeremotes
    for i in $REMOTES; do
        echo -n " $i"
        if  [ $i = "remote-event" ] ; then
            pattern="remote-$device"
        else
            pattern="$i"
        fi
        if grep -q "$pattern" $CFG_DIR/remote.conf ; then
            grep "$pattern" $CFG_DIR/remote.conf > "$CFG_DIR/remote.$i.conf"
        fi
    done
}
