#!/bin/sh

upgrade_log="/tmp/log.upgrade"
log() {
	logger -s -t 'fiire-upgrade' $*
}

# Test if the image is available
if [[ ! -f /tmp/firmware.img ]] ;then
	log "Firmware image not found"
    echo " `date` ==fu== Firmware image not found! Exit... " >> $upgrade_log
	exit 1
fi

# Stop services
log "Closing down services."
echo " `date` ==fu== Closing down services: httpd & Pluto." >> $upgrade_log

/etc/init.d/httpd stop
/etc/init.d/Pluto stop

sleep 2

# Backup configuration
log "Creating a backup of the config files."
echo " `date` ==fu== Creating a backup of the config files: /etc/pluto /etc/config /etc/fiire-ra" >> $upgrade_log
tar zcf /tmp/backup.tar.gz /etc/pluto /etc/config /etc/fiire-ra

# Flash new image
log "Started flashing new image."
echo " `date` ==fu== Started flashing new image." >> $upgrade_log
echo " `date` ==fu== Free space available = `free | tr -s ' ' | tail -1 | cut -d" " -f 4` " >> $upgrade_log

cd /tmp
mtd write /tmp/firmware.img linux

echo " `date` ==fu== Finished flashing new image." >> $upgrade_log
echo " `date` ==fu== Removing firmware image /tmp/firmware.img." >> $upgrade_log

rm -f /tmp/firmware.img

# Restore config files
log "Restoring configuration files."
echo " `date` ==fu== Restoring configuration files." >> $upgrade_log
cd /
tar zxvf /tmp/backup.tar.gz

# Rebooting
echo " `date` ==fu== REBOOTING the system..." >> $upgrade_log
reboot
