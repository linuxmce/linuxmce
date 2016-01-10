BEGIN {
	IGNORECASE = 1;
	
	if (vOpenGL == "")
	{
		print "vOpenGL was not set";
		exit 2;
	}

	if (vAlphaBlending == "")
	{
		print "vAlphaBlending was not set";
		exit 2;
	}

	if (vOpenGL == 0)
		vUI = "UI1"
	else if (vAlphaBlending == 0)
		vUI = "UI2mask"
	else vUI = "UI2alpha";

	SectionScreenOrDevice = 0;
	SectionDevice = 0;
	Driver = 0;
	
	XvmcUsesTextures = 0;
	AllowGLXWithComposite = 0;
	AddARGBGLXVisuals = 0;
	RenderAccel = 0;

	AccelMethod = 0;
	ExaScratchSize = 0;
	MaxDRIMem = 0;
	MigrationHeuristic = 0;
	
	Composite = 1;

	warncount = 0;
	errcount = 0;
}

/Section..*"Screen"/ { SectionScreenOrDevice = 1; next; }
/Section..*"Device"/ { SectionScreenOrDevice = 1; SectionDevice = 1; next; }
SectionScreenOrDevice == 1 && /EndSection/ { SectionScreenOrDevice = 0; SectionDevice = 0; next; }
SectionScreenOrDevice == 1 && /Option..*"XvmcUsesTextures"/ { XvmcUsesTextures = 1; }
SectionScreenOrDevice == 1 && /Option..*"AllowGLXWithComposite"/ { AllowGLXWithComposite = 1; }
SectionScreenOrDevice == 1 && /Option..*"AddARGBGLXVisuals"/ { AddARGBGLXVisuals = 1; }
SectionScreenOrDevice == 1 && /Option..*"RenderAccel"/ { RenderAccel = 1; }

SectionScreenOrDevice == 1 && /Option..*"AccelMethod"/ { AccelMethod = 1; }
SectionScreenOrDevice == 1 && /Option..*"ExaScratchSize"/ { ExaScratchSize = 1; }
SectionScreenOrDevice == 1 && /Option..*"MaxDRIMem"/ { MaxDRIMem = 1; }
SectionScreenOrDevice == 1 && /Option..*"MigrationHeuristic"/ { MigrationHeuristic = 1; }

SectionDevice == 1 && /Driver..*/ { Driver = $2; }

/Section..*"Extensions"/ { SectionExtension = 1; next; }
SectionExtension == 1 && /EndSection/ { SectionExtension = 0; next; }
SectionExtension == 1 && /Option..*"Composite"/ { if ($2 ~ "false") Composite = 0;}

END {
	if (Driver == "\"vesa\"")
		warnings[warncount++] = "Warning: you are using the vesa driver. Performance will be affected";
	if (Driver == "\"nv\"")
		warnings[warncount++] = "Warning: you are using the nv driver. It is recommended that you use the nvidia driver.";
	
	if (vUI == "UI2mask")
	{
		if (Driver != "\"nvidia\"" && Driver != "\"via\"" && Driver != "\"ati\"")
			warnings[warncount++] = "Warning: you try to run UI2 with mask on the " Driver " driver. Performance will be affected.";
		if (XvmcUsesTextures != 1 && Driver == "\"nvidia\"")
			errors[errcount++] = "Error: UI2 with mask needs XvmcUsesTextures";
		if (AllowGLXWithComposite != 1 && Driver == "\"nvidia\"")
			errors[errcount++] = "Error: UI2 with mask needs AllowGLXWithComposite";
		if (RenderAccel != 1 && Driver == "\"nvidia\"")
			errors[errcount++] = "Error: UI2 with mask needs RenderAccel";
	}
	
	if (vUI == "UI2alpha")
	{
#<-mkr_B_via_b->
		if (Driver != "\"nvidia\"")
			errors[errcount++] = "Error: Can't run UI2 with alpha blending on other drivers than nvidia."
#<-mkr_B_via_e->
		if (XvmcUsesTextures != 1 && Driver == "\"nvidia\"")
			errors[errcount++] = "Error: UI2 with alpha blending needs XvmcUsesTextures";
		if (AllowGLXWithComposite != 1 && Driver == "\"nvidia\"")
			errors[errcount++] = "Error: UI2 with alpha blending needs AllowGLXWithComposite";
		if (RenderAccel != 1 && Driver == "\"nvidia\"")
			errors[errcount++] = "Error: UI2 with alpha blending needs RenderAccel";
		if (Composite != 1)
			errors[errcount++] = "Error: UI2 with alpha blending needs Composite";
	}

	for (i = 0; i < errcount; i++)
		print errors[i];
	for (i = 0; i < warncount; i++)
		print warnings[i];

	if (errcount > 0)
		exit 2;
	if (warncount > 0)
		exit 1;
	exit 0;
}
