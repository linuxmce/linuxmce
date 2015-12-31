BEGIN {
	DeviceSection=0;
}
/Section..*"Device"/ { DeviceSection=1; }
DeviceSection == 1 && /Driver/ && !/###/ { gsub(/"/, "", $2); print $2; exit; }
DeviceSection == 1 && /EndSection/ { DeviceSection = 0; exit } # just in case the "Driver" option is missing
