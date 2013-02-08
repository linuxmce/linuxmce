#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh

nice -n 19 mysqldump $MYSQL_DB_CRED --skip-extended-insert --quote-names --allow-keywords --add-drop-table pluto_main CommandGroup CommandGroup_Command CommandGroup_Command_CommandParameter CommandGroup_EntertainArea CommandGroup_Room Criteria CriteriaParm CriteriaParmNesting Device DeviceGroup Device_Command Device_CommandGroup Device_DeviceData Device_DeviceGroup Device_Device_Pipe Device_Device_Related Device_EntertainArea Device_HouseMode Device_Orbiter Device_StartupScript Device_Users EntertainArea EventHandler Floorplan Household Household_Installation Image Installation Installation_RepositorySource_URL Installation_Users Orbiter Orbiter_Users_PasswordReq Orbiter_Variable Room SetupStep UnknownDevices Users Firewall Room_Users InfraredGroup_Command_Preferred PaidLicense Package_Device RemoteControl Device_MRU Device_QuickStart > /var/log/pluto/pluto_main.dump.log

