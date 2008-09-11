#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

# 2005-04-11 * Disabled. Considered not needed anymore
#echo "Converting all tables except 'mysql' to InnoDB (this may take a while)"
#DBs=$(RunSQL 'SHOW DATABASES')
#for DB in $DBs; do
#	[[ "$DB" == mysql ]] && continue
#	Tables=$(RunSQL "SHOW TABLES FROM $DB")
#	for Table in $Tables; do
#		RunSQL "ALTER TABLE \`$DB\`.\`$Table\` TYPE=$DefTableType"
#	done
#done
#echo "Finished converting tables to InnoDB"

# When we're getting a new software version, be sure we do a full regen 
# on all the orbiters

device="$PK_Device"
code="$Activation_Code"

PrevVersion="$2"

DEVICETEMPLATE_HAL=1808

echo "setting up dce router2"
hasRecords=$(RunSQL "SELECT count(PK_Installation) FROM Installation")
if [[ $hasRecords -ne 0 ]]; then
	echo "Database already setup";
	SkipDatabase="yes"
fi

RunSQL "update Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate SET Device.Description=replace(Device.Description,':','-') WHERE FK_DeviceCategory in (157,171,155,158)"

/usr/pluto/bin/SetupUsers.sh
/usr/pluto/bin/Update_StartupScrips.sh
/usr/pluto/bin/generateRcScripts.sh

mkdir -p /tftpboot/pxelinux.cfg
cp /usr/lib/syslinux/pxelinux.0 /tftpboot

if [[ "$TestInstallation" == 1 ]]; then 
	sed -i 's/#log..= \/var\/log\/mysql\/mysql.log/log = \/var\/log\/mysql\/mysql.log/g' /etc/mysql/my.cnf
	sed -i 's/#log.= \/var\/log\/mysql\/mysql.log/log = \/var\/log\/mysql\/mysql.log/g' /etc/mysql/my.cnf
fi

# update atftp entry in inet.d
update-inetd --remove tftp
update-inetd --group BOOT --add "tftp        dgram   udp wait    nobody /usr/sbin/tcpd /usr/sbin/in.tftpd --tftpd-timeout 300 --retry-timeout 5     --mcast-port 1758 --mcast-addr 239.255.0.0-255 --maxthread 100 --verbose=5 --no-blksize /tftpboot"

bash_flag="# Pluto - bash root prompt"
bash_prompt="$bash_flag
if [[ -f /usr/pluto/bin/Config_Ops.sh ]]; then
	. /usr/pluto/bin/Config_Ops.sh
fi

export PS1='\h_'\$PK_Installation':\w\\$ '"

if ! grep -q nullglob /root/.profile; then
	echo "shopt -u nullglob # Pluto disable nullglob for shell" >>/root/.profile
fi

if ! grep -qF "$bash_flag" /root/.bashrc; then
	awk '/PS1=/ {next} {print}' /root/.bashrc >/root/.bashrc.$$
	echo "$bash_prompt" >>/root/.bashrc.$$
	mv -f /root/.bashrc.$$ /root/.bashrc
fi

echo "Adding user_xattr to filesystems that support it and remounting them"
awk '/^#/ || ($3 != "ext2" && $3 != "ext3") || /user_xattr/ {print; next}
	{printf "%-15s %-15s %-7s %-15s %-7s %-7s\n", $1, $2, $3, $4 ",user_xattr", $5, $6}' /etc/fstab >/etc/fstab.$$
mv /etc/fstab.$$ /etc/fstab
awk '!/^#/ && ($3 == "ext2" || $3 == "ext3") {print $2}' /etc/fstab | while read line; do
	echo -n "$line "
	mount -o remount $line && echo "Ok" || echo "Failed"
done

[[ -z "$RA_CheckRemotePort" ]] && ConfSet RA_CheckRemotePort 1

modules="ip_conntrack ip_conntrack_ftp ip_conntrack_irc ip_nat_ftp ip_nat_irc"

for module in $modules; do
	if ! grep -q "$module" /etc/modules; then
		echo "$module" >>/etc/modules
	fi
	modprobe $module || :
done

echo "Updating IR codes for all devices"
Q="SELECT DISTINCT FK_DeviceTemplate FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_InfraredGroup is not null"
Templates=$(RunSQL "$Q")

for Template in $Templates; do
	/usr/pluto/bin/WebDB_GetIR.sh 0 "$Template"
done

if [[ -n "$PrevVersion" ]]; then
	Q="
		SELECT PK_Device
		FROM Device
		JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_DeviceCategory IN (7,8) AND FK_Device_ControlledVia IS NULL
	"
	Computers=$(RunSQL "$Q")

	for Comp in $Computers; do
		NewHAL=$(/usr/pluto/bin/CreateDevice -d "$DEVICETEMPLATE_HAL" -C "$Comp" 2>/dev/null)
		if [[ -n "$NewHAL" ]]; then
			echo "Created HAL device '$NewHAL' under computer '$Comp'"
		fi
	done
fi

#<-mkr_b_ubuntu_b->
File="/etc/event.d/media-center-startup"
Content="
start on runlevel 2

stop on shutdown
stop on runlevel 3
stop on runlevel 4
stop on runlevel 5

script
/usr/pluto/bin/startup-script.sh
end script
"
echo "${Content}" > ${File} || :
#<-mkr_b_ubuntu_e->
