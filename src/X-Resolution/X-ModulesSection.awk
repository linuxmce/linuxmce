BEGIN {
	SectionModule = 0;
	Modules="";
	if (ModList == "")
		Mode = "read";
	else
		Mode = "update";
}
/Section..*"Module"/ {
	SectionModule = 1;
	if (Mode != "read")
		print;
	next;
}
SectionModule == 1 && /EndSection/ {
	SectionModule = 0;
	if (Mode != "read")
		print;
	next;
}
Mode == "read" && SectionModule == 1 && /Load/ {
	gsub(/"/, "", $2);
	Modules = Modules " " $2;
	next;
}
Mode == "update" && SectionModule == 1 {
	count = split(ModList, ModArray);
	for (i = 1; i <= count; i++)
	{
		print "\tLoad\t\"" ModArray[i] "\"";
	}
	Mode="done";
	next;
}
Mode != "read" && SectionModule == 0 { print; }
END { if (Mode == "read") print Modules; }
