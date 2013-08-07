#!/bin/bash

#<-dceag-mp-b->
#DCE_Package
#DCE_TargetDir

if [ "$#" != 3 ]; then
	echo "Syntax: $0 <application name> <version> <device template id>"
	exit 1
fi

PACK_LOC=../Packages
ORIG_APPNAME="$1"
APPNAME=pluto-`echo "$ORIG_APPNAME" | tr 'A-Z_' 'a-z\-'`
APPVER="$2"
DEVTMPL_ID="$3"

PACK_NAME="$APPNAME-$APPVER"
PACK_DIR="$PACK_LOC/$PACK_NAME"
DISTRO=1 # 1=Debian

GetMySQLResult()
{
	local Q

	Q="$*"
	echo "$Q;" | mysql pluto_main | tail +2
	return "$?"
}

GetPackageDepends()
{
	local Package_Number Package_Name
	local Q R
	local Row

	Package_Number="$1"
	
	Q="SELECT PK_Package, Name
FROM Package_Package
JOIN Package ON Package.PK_Package = FK_Package_DependsOn
JOIN Package_Source ON Package_Source.FK_Package = PK_Package
JOIN RepositorySource ON FK_RepositorySource = PK_RepositorySource
JOIN Package_Source_Compat ON FK_Package_Source = PK_Package_Source
WHERE Package_Package.FK_Package='$Package_Number' AND FK_RepositoryType=1 AND Package_Source_Compat.FK_Distro='$DISTRO' AND OnlyToBuild=0"
	
	R=$(GetMySQLResult "$Q") # TODO: test result

	echo "$R" |
	while read Row; do
		[ -z "$Row" ] && break # weird thing: we have to force an exit, else it loops forever
		Package_Number=$(echo "$Row" | cut -f1)
		Package_Name=$(echo "$Row" | cut -sf2)
		GetPackageDepends "$Package_Number"
		echo -n "$Package_Name "
	done
}

# return the packages needed by the device plus their dependencies
GetDevicePackageList()
{
	local Package_Number Package_Name
	local Q R
	local Row
	
	Device_Number="$1"

	Q="SELECT Package_Source.FK_Package,Name
FROM DeviceTemplate
JOIN Package_Source ON Package_Source.FK_Package=DeviceTemplate.FK_Package
JOIN Package_Source_Compat ON FK_Package_Source=PK_Package_Source
WHERE PK_DeviceTemplate='$Device_Number' AND FK_Distro='$DISTRO'"

	R=$(GetMySQLResult "$Q") # TODO: test result
	
	echo "$R" |
	while read Row; do
		[ -z "$Row" ] && break # weird thing: we have to force an exit, else it loops forever
		Package_Number=$(echo "$Row" | cut -f1)
		Package_Name=$(echo "$Row" | cut -sf2)
		GetPackageDepends "$Package_Number"
		echo -n "$Package_Name "
	done
}

#GetDevicePackageList "$DEVTMPL_ID"; echo
PackageList=$(GetDevicePackageList "$DEVTMPL_ID")

if [ ! -d "$PACK_LOC" ]; then
	mkdir -p "$PACK_LOC"
fi

if [ ! -d "$PACK_DIR" ]; then
	cp -a "$PACK_LOC/_Package_template" "$PACK_DIR"
fi

BIN_LOC="../../bin"
LIB_LOC="../../lib"
BIN_FILE="$BIN_LOC/$ORIG_APPNAME"
PLUGIN_FILE="$BIN_LOC/$ORIG_APPNAME.so"
LIB_FILE="$LIB_LOC/lib$ORIG_APPNAME.a"

pushd "$PACK_DIR"

mkdir -p root/usr/pluto/bin
mkdir -p root/usr/pluto/lib

echo "-- $BIN_FILE"
if [ -f "$BIN_FILE" ]; then
	echo "Found bin file"
	cp "$BIN_FILE" root/usr/pluto/bin
fi

echo "-- $PLUGIN_FILE"
if [ -f "$PLUGIN_FILE" ]; then
	echo "Found plugin file"
	cp "$PLUGIN_FILE" root/usr/pluto/bin
fi

echo "-- $LIB_FILE"
if [ -f "$LIB_FILE" ]; then
	echo "Found lib file"
	cp "$LIB_FILE" root/usr/pluto/lib
fi

popd

pushd "$PACK_LOC"
./propagate.pl "$PACK_NAME" $PackageList
popd

#<-dceag-mp-e->

