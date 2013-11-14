#!/bin/bash

# A request from a device already configured for the network, even if it's a new device for pluto system

#Dec 20 05:42:03 dcerouter dhcpd: DHCPREQUEST for 192.168.80.253 from 00:0f:23:fc:a4:ea via eth1
#Dec 20 05:42:03 dcerouter dhcpd: DHCPACK on 192.168.80.253 to 00:0f:23:fc:a4:ea (SEP000F23FCA4EA) via eth1

# This is a REAL dhcp offer
#Dec 20 05:44:43 dcerouter dhcpd: DHCPDISCOVER from 00:0f:23:fc:a4:ea (SEP000F23FCA4EA) via eth1
#Dec 20 05:44:43 dcerouter dhcpd: DHCPOFFER on 192.168.80.253 to 00:0f:23:fc:a4:ea (SEP000F23FCA4EA) via eth1
#Dec 20 05:44:43 dcerouter dhcpd: DHCPREQUEST for 192.168.80.253 (192.168.80.1) from 00:0f:23:fc:a4:ea (SEP000F23FCA4EA) via eth1
#Dec 20 05:44:43 dcerouter dhcpd: DHCPACK on 192.168.80.253 to 00:0f:23:fc:a4:ea (SEP000F23FCA4EA) via eth1

# This is a dhclient sequence
#May 22 20:59:21 dcerouter dhclient: DHCPREQUEST on eth0 to 10.0.0.150 port 67
#May 22 20:59:21 dcerouter dhclient: DHCPACK from 10.0.0.150
#May 22 20:59:21 dcerouter dhclient: bound to 10.0.0.84 -- renewal in 31 seconds.

# A case of DHCPACK, with a format different to the ACK for REQUEST
#Nov 13 08:12:16 dcerouter dhcpd: DHCPINFORM from 192.168.80.241 via eth1
#Nov 13 08:12:16 dcerouter dhcpd: DHCPACK to 192.168.80.241 (00:15:60:c8:0b:fa) via eth1

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

log_plugin()
{
	local Type="$1" Data="$2"

	case "$Type" in
		err) Type="01" ;;
		log) Type="00" ;;
	esac

	printf "%s\n" "$Type $(date -R) $Data" >>/var/log/pluto/dhcp_pnp.log
}

parse()
{
	local Line="$1"

	local Word Ret
	for Word in $Line; do
		Ret="$Ret,$Word"
	done

	echo "${Ret#,}"
}

extract_field()
{
	local Field="$1" Line="$2"

	echo "$Line" | cut -d',' -f"$Field"
}

log_plugin log "\n\n\n\nDhcp-Plugin Process Started"
log_plugin log "Dhcp-Plugin Started"

exec 3> >(bash -x /usr/pluto/bin/Dhcpd-Plugin-Queue.sh)

while :; do
	read inline
	found=0

	line=$(parse "$inline")
	tag=$(extract_field 5 "$line")

	case "$tag" in
		"dhcpd:")
			op=$(extract_field 6 "$line")
			mac_found=$(extract_field 10 "$line")
			ip_sent=$(extract_field 8 "$line")

			if [[ -z "$mac_found" ]]; then
				continue
			fi

			case "$op" in
				"DHCPACK")
					#DHCPACK on 192.168.80.249 to bc:0f:2b:a1:25:04 (android-d166338fe3c731cc) via eth1
					#not DHCPACK to 192.168.80.241 (00:15:60:c8:0b:fa) via eth1
					if [[ "$(extract_field 7 "$line")" != "on" ]]; then
						continue
					fi

					log_plugin log "DHCP : MAC '$mac_found' IP '$ip_sent'"

					Q="SELECT PK_Device FROM Device WHERE MACaddress='$mac_found' AND IPaddress='$ip_sent'"
					R=$(RunSQL "$Q")

					if [[ -z "$R" ]]; then
						Q="SELECT PK_UnknownDevices FROM UnknownDevices WHERE MACaddress='$mac_found'"
						R=$(RunSQL "$Q")

						if [[ -z "$R" ]]; then
							log_plugin log "NEW DEVICE !!!"

							echo "$mac_found $ip_sent" >&3
						else
							log_plugin log "Device already marked as unknown, ignore"
						fi
					else
						log_plugin log "Device already exists: $R"
					fi
				;;
			esac
		;;

		"dhclient:")
			text="$(extract_field 6,7 "$line")"
			if [[ "$text" == "bound,to" ]]; then
				/usr/pluto/bin/Network_DNS.sh dhclienthook
				/usr/pluto/bin/Network_NIS.sh
			fi
		;;
	esac
done
