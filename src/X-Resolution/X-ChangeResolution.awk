BEGIN {
	if (ResX ~ /[^0-9]|^$/ || ResY ~ /[^0-9]|^$/ || Refresh ~ /[^0-9]|^$/)
	{
		print "Error in parameters" >"/dev/stderr";
		exit;
	}
	Monitor = 0;
	Screen = 0;
	Device = 0;
	IGNORECASE = 1;
}
/Modeline/ { next }
/Section..*"Monitor"/ { Monitor = 1; }
Monitor == 1 && (/HorizSync/ || /VertRefresh/ || /Modeline/) { next; }
Monitor == 1 && /EndSection/ {
	if (Modeline != "")
		print "\tModeline \t" Modeline;
	print "\tHorizSync\t20-500";
	print "\tVertRefresh\t" Refresh-1 "-" Refresh+1;
	Monitor = 0;
}
/SubSection..*"Display"/,/EndSubSection/ { next; }
/DefaultDepth/ {
	print;
	print "\tSubSection \"Display\"";
	print "\t\tModes\t\t\"" ResX "x" ResY "\"";
	print "\t\tVirtual\t\t" ResX " " ResY;
	print "\tEndSubSection";
	next;
}
/Section..*"Screen"/ { Screen = 1; }
Screen == 1 && /Option.*"TVStandard"/ { next }
Screen == 1 && /EndSection/ && nvHD != "" {
	print "\tOption\t\"TVStandard\"\t\"" nvHD "\"";
	Screen = 0;
}
/Section..*"Device"/ { Device = 1; }
/Option.*"ConnectedMonitor"/ || /Option.*"TVOutFormat"/ { next }
Device == 1 && /EndSection/ {
	if (ConnectedMonitor != "")
		print "\tOption\t\t\"ConnectedMonitor\" \"" ConnectedMonitor "\"";
	if (TVOutFormat != "")
		print "\tOption\t\t\"TVOutFormat\" \"" TVOutFormat "\"";
	Device = 0;
}

{ print }
