# external variables: DisplayDriver

BEGIN {
	if (DisplayDriver == "")
	{
		print "DisplayDriver variable is empty" >"/dev/stderr";
		exit;
	}
	DeviceSection=0;
}
/Section..*"Device"/ { DeviceSection=1; }
DeviceSection == 1 && /Driver/ && !/###/ { print "\tDriver\t\t\"" DisplayDriver "\""; next; }
DeviceSection == 1 && /EndSection/ { DeviceSection = 0; }
{ print; }
