BEGIN {
	SectionExtension = 0;
	SectionScreenOrDevice = 0;
	if (OpenGL == "")
		OpenGL = "0";
	if (AlphaBlending == "")
		AlphaBlending = "0";
	if (OpenGL != "0" && OpenGL != "1")
	{
		print "Invalid OpenGL variable value: " OpenGL;
		exit 1;
	}
	if (AlphaBlending != "0" && AlphaBlending != "1")
	{
		print "Invalid AlphaBlending variable value: " AlphaBlending;
		exit 1;
	}
}

# output the part that interests us in the Screen/Device section
function outputScreenOrDevice()
{
#<-mkr_b_via_b->
	# http://wiki.openchrome.org/tikiwiki/tiki-index.php?page=EXAAcceleration
	print "\tOption \"AccelMethod\" \"exa\"";
	print "\tOption \"ExaScratchSize\" \"8192\"";
	print "\tOption \"MaxDRIMem\" \"16384\"";
	print "\tOption \"MigrationHeuristic\" \"always\"";
#<-mkr_b_via_e->

	print "\tOption \"XvmcUsesTextures\" \"true\"";
	print "\tOption \"renderAccel\" \"true\"";
	print "\tOption \"NoDDCValue\"";
	print "\tOption \"UseEDID\" \"false\"";
	print "\tOption \"ExactModeTimingsDVI\" \"true\"";
	print "\tOption \"NoLogo\" \"true\"";
	print "\tOption \"NoBandWidthTest\" \"true\"";
	print "\tOption \"ModeValidation\" \"NoDFPNativeResolutionCheck, NoEdidMaxPClkCheck, NoMaxPClkCheck, AllowInterlacedModes, AllowNon60HzDFPModes\"";
	print "\tOption \"DynamicTwinView\" \"false\"";
	print "\tOption \"UseEvents\" \"true\"";

	if (OpenGL == 0)
		return;
	print "\tOption \"AllowGLXWithComposite\" \"true\"";
	print "\tOption \"AddARGBGLXVisuals\" \"true\"";
}

function outputExtensions()
{
	print "Section \"Extensions\"";
#<-mkr_B_via_b->
	if (AlphaBlending == 1)
	{
		print "\tOption \"Composite\" \"true\"";
	}
	else
	{
		print "\tOption \"Composite\" \"false\"";
	}
#<-mkr_B_via_e->
#<-mkr_b_via_b->
	print "\tOption \"Composite\" \"false\"";
#<-mkr_b_via_e->
	print "\tOption \"RENDER\" \"true\"";
	print "EndSection";
}

# update Screen and Device sections
/Section..*"Screen"/ || /Section..*"Device"/ { SectionScreenOrDevice = 1; print; next; }
SectionScreenOrDevice == 1 && (/Option..*"XvmcUsesTextures"/ || /Option..*"AllowGLXWithComposite"/ || /Option..*"AddARGBGLXVisuals"/ || /Option..*"renderAccel"/ \
	|| /Option..*"AccelMethod"/ || /Option..*"ExaScratchSize"/ || /Option..*"MaxDRIMem"/ || /Option..*"MigrationHeuristic"/ \
	|| /Option..*"NoDDCValue"/ || /Option..*"UseEDID"/ || /Option..*"ExactModeTimingsDVI"/ || /Option..*"NoLogo"/ || /Option..*"NoBandWidthTest"/ || /Option..*"ModeValidation"/ || /Option..*"DynamicTwinView"/ || /Option..*"UseEvents"/ \
) { next; }
SectionScreenOrDevice == 1 && /EndSection/ { SectionScreenOrDevice = 0; outputScreenOrDevice(); print; next; }
SectionScreenOrDevice == 1 { print; next; }

# update (as in "delete+rewrite") Extensions section
/Section..*"Extensions"/ { SectionExtension = 1; next; }
SectionExtension == 1 && /EndSection/ { SectionExtension = 0; next; }
SectionExtension == 1 { next; }
{ print; }

END { outputExtensions(); }
