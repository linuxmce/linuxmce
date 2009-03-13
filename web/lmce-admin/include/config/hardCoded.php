<?
	// hard-coded values
	// PK_DeviceCategory from table DeviceCategory
	
	$GLOBALS['rootOrbiterID'] = 5; 				// Orbiter
	$GLOBALS['rootComputerID'] = 6; 			// Computers
	$GLOBALS['CategoryCore']=7;					// Core
	$GLOBALS['rootMediaDirectors'] = 8; 		// Media Directors
	$GLOBALS['rootInterfaces'] = 71; 			// Interfaces
	$GLOBALS['rootLights'] = 73; 				// Lighting devices
	$GLOBALS['rootAVEquipment'] = 76; 			// AV Equipment
	$GLOBALS['rootClimate'] = 83; 				// Climate devices
	$GLOBALS['rootGenericIO'] = 79; 			// Generic I/O
	$GLOBALS['rootSecurity'] = 84; 				// Security devices
	$GLOBALS['IntercomCategory']=94;			// Intercom 
	$GLOBALS['rootCameras']=93;					// Surveillance Cameras
	$GLOBALS['rootLightsInterfaces'] = 81;		// Light Interfaces
	$GLOBALS['rootClimateInterfaces'] = 82;		// Climate Interfaces
	$GLOBALS['rootSecurityInterfaces'] = 80;	// Security Interfaces
	$GLOBALS['rootPhones'] = 89;				// Phones
	$GLOBALS['specialized']=97;					// Specialized
	$GLOBALS['InfraredInterface']=114;			// Infrared Interface
	$GLOBALS['TVdevices']=77;					// TV's
	$GLOBALS['CableBoxes']=105;					// Cable boxes
	$GLOBALS['SatelliteBoxes']=108;				// Satellite Boxes
	$GLOBALS['TVVCRCombo']=106;					// TV VCR Combo
	$GLOBALS['AmpsReceivers']=103;				// Amps/Receivers
	$GLOBALS['RemoteControlls']=117;			// Remote Controlls
	$GLOBALS['GenericSerialDevices']=119;		// GenericSerialDevices
	$GLOBALS['PVRCaptureCards']=75;				// PVR Capture Cards
	$GLOBALS['SoundCards']=124;					// Sound Cards
	$GLOBALS['VideoCards']=125;					// Video Cards
	$GLOBALS['InfraredReceivers']=132;			// Infrared receivers
	$GLOBALS['DigitalTVCards']=143;				// Digital TV Cards
	$GLOBALS['IrrigationDevices']=153;			// Irrigation Devices
	$GLOBALS['CaptureCardsPorts']=162;			// Capture cards ports
	$GLOBALS['ScreenSavers']=163;				// Screen Savers
	$GLOBALS['RaidCategory']=165;				// RAID devices
	$GLOBALS['RaidDrives']=167;					// RAID drives
	$GLOBALS['StorageDevices']=155;				// Storage devices
	$GLOBALS['NetworkStorage']=158;				// Network Storage devices
	$GLOBALS['call_before_timeout']=247;		// No. of sec to ring before IVR
	$GLOBALS['call_before_phones']=248;			// Ring ext before IVR
	
	
	$GLOBALS['TVdevicesArray']=array($GLOBALS['TVdevices'],$GLOBALS['TVVCRCombo'],$GLOBALS['AmpsReceivers']);
	
	// PK_DeviceTemplate from table DeviceTemplate
	
	$GLOBALS['rootDCERouter'] = 1; 				// DCE Router
	$GLOBALS['rootMediaPlugin']=2;				// Media Plug-in
	$GLOBALS['GeneralInfoPlugin']=27;			// General Info Plugin
	$GLOBALS['rootCoreID'] = 7; 				// Core
	$GLOBALS['deviceTemplateOrbiter']=8;		// Orbiter
	$GLOBALS['OrbiterPlugIn'] = 12; 			// Orbiter Plug In
	$GLOBALS['XinePlayer'] = 5; 				// Xine Player
	$GLOBALS['BluetoothDongle'] = 13; 			// BluetoothDongle
	$GLOBALS['rootMediaDirectorsID'] = 28; 		// Media Directors
	$GLOBALS['SecurityPlugin'] = 33;	 		// Security plugin
	$GLOBALS['TelecomPlugin'] = 34;	 			// Telecom plugin
	$GLOBALS['LightSwitchOnOff']=37;			// Light Switch (on/off)
	$GLOBALS['LightSwitchDimmable']=38;			// Light Switch (dimmable)
	$GLOBALS['InfraredPlugIn'] = 39; 			// Infrared Plug In
	$GLOBALS['EIB']=49;							// EIB
	$GLOBALS['GenericInputOuput']=43;			// Generic Input Ouput 
	$GLOBALS['DrapesSwitch']=68;				// Drapes Switch
	$GLOBALS['StandardThermostat']=4;			// Standard Thermostat
	$GLOBALS['Tuner']=75;						// Generic Tuner
	$GLOBALS['ONScreenOrbiter']=62;				// On screen orbiter
	$GLOBALS['EventPlugIn']=52;					// Event plugin
	$GLOBALS['Asterisk']=45;					// Asterisk
	$GLOBALS['Cisco7970Orbiter']=1727;			// Cisco 7970 Orbiter
	$GLOBALS['ProxyOrbiter']=1749;				// Generic Web Orbiter
	$GLOBALS['GenericWebDevice']=1748;			// Generic Web Device
	$GLOBALS['PowerfileC200']=1737;				// Powerfile C200
	$GLOBALS['VirtDevTelecomPlugIn']=-107;		// VirtDev - Telecom Plug-in
	$GLOBALS['CanBus']=1746;					// Can Bus
	$GLOBALS['AppServer']=26;					// App server
	$GLOBALS['RaidHardDrive']=1850;				// Raid Hard Drive
	$GLOBALS['Raid5']=1849;						// Raid 5
	$GLOBALS['Raid1']=1851;						// Raid 1
	$GLOBALS['Raid0']=1854;						// Raid 0
	$GLOBALS['VDR_Plugin']=1704;				// VDR plugin
	$GLOBALS['SlimServer_Streamer']=53;			// SlimServer Streamer
	$GLOBALS['PhotoScreenSaver']=1825;			// Photo Screen Saver
	
	// PK_Template from table Template
	
	$GLOBALS['LightingScenariosTemplate']=9;		// Lighting Scenarios
	$GLOBALS['LightingAutomaticTemplate']=31;		// Lighting Automatic Scenarios
	$GLOBALS['ClimateScenariosTemplate']=10;		// Climate Scenarios
	$GLOBALS['securityScenariosTemplate']=12;		// Security Scenarios
	$GLOBALS['MediaScenariosTemplate']=32;			// Media Scenarios
	$GLOBALS['EventsHandlerTemplate']=11;			// Events Handler
	$GLOBALS['SecurityArmDisarmTemplate']=13;		// Security - Arm Disarm
	$GLOBALS['SecuritySOSTemplate']=14;				// Security - SOS
	$GLOBALS['SecurityViewCamerasTemplate']=15;		// Security - View Cameras
	$GLOBALS['SecurityViewOneCameraTemplate']=16;	// Security - View Camera
	$GLOBALS['TelecomScenariosTemplate']=17;		// Telecom Scenarios
	$GLOBALS['IrrigationScenariosTemplate']=42;		// Irrigation Scenarios Template
	
	// PK_Array from table Array
	
	$GLOBALS['ArrayIDForLight'] = 1;				// Lighting Scenarios
	$GLOBALS['ArrayIDForClimate'] = 2;				// Climate Scenarios
	$GLOBALS['ArrayIDForSecurity'] = 3;				// Security scenarios
	$GLOBALS['ArrayIDCommunicationScenarios']=4; 	// Communication scenarios
	$GLOBALS['ArrayIDForMedia'] = 5;				// Media Scenarios
	$GLOBALS['EventsHandlerArray']=20;				// Events Handler
	$GLOBALS['ArrayIDForMobileOrbiterScenarios']=23;// Mobile Orbiter Scenarios	
	$GLOBALS['ArrayIDSleepingScenarios']=21;		// Sleeping Scenarios
	$GLOBALS['ArrayIDForIrrigation']=27;			// Irrigation Scenarios
	
	// PK_Command from Command table
	
	$GLOBALS['inputSelectCommand']=91;				// Input select
	$GLOBALS['DSPModeCommand']=116;					// DSP Mode
	$GLOBALS['genericONCommand']=192;				// Generic On
	$GLOBALS['genericOFFCommand']=193;				// Generic OFF
	$GLOBALS['powerCommand']=194;					// toggle power
	$GLOBALS['genericSetLevelCommand']=184;			// Set Level
	$GLOBALS['genericHeatCommand']=197;				// Generic Heat
	$GLOBALS['genericCoolCommand']=196;				// Generic Cool
	$GLOBALS['setCoolHeat']=280;					// Set cool/Heat, with parm C for Cool and H for Heat
	$GLOBALS['MediaScenariosCommand']=43;			// MH Play Media
	$GLOBALS['commandSetUserMode']=58;				// Set User Mode	
	$GLOBALS['commandSetVar']=27;					// Set Variable
	$GLOBALS['commandGotoScreen']=5;				// Go to screen
	$GLOBALS['commandMHPlayMedia']=43;				// MH Play Media
	$GLOBALS['commandPL_Originate']=232;			// PL_Originate		// deprecated
	$GLOBALS['outputSelectCommand']=244;			// Output select
	$GLOBALS['tuner_1']=166;						// Tuner 1
	$GLOBALS['tuner_2']=175;						// Tuner 2
	$GLOBALS['tuner_3']=326;						// Tuner 3
	$GLOBALS['AM_Command']=169;						// AM
	$GLOBALS['FM_Command']=170;						// FM
	$GLOBALS['SW_Command']=328;						// SW
	$GLOBALS['WB_Command']=327;						// WB
	$GLOBALS['DelayCommand']=257;					// delay
	$GLOBALS['GoToScreen']=741;						// Go to screen
	$GLOBALS['command_MakeCall']=921;			// Make Call
	
	// PK_CommandParameters from table CommandParameters
	
	$GLOBALS['commandParamPK_Device']=2;			// PK_Device
	$GLOBALS['commandParamPK_DesignObj']=3;			// PK_DesignObj
	$GLOBALS['commandParameterVariableNumber']=4;	// Variable Number (PK_Variable)
	$GLOBALS['commandParameterValueToAsign']=5;		// Value to assign
	$GLOBALS['commandParameterOnOff']=8;			// On/Off
	$GLOBALS['commandParamID']=10;					// ID
	$GLOBALS['commandParamFilename']=13;			// Filename
	$GLOBALS['commandParamDesignObjCurrentScreen']=16;	// PK_DesignObj_CurrentScreen
	$GLOBALS['commandParamPKUsers']=17;				// PK_Users
	$GLOBALS['commandParamStoreVariables']=22;		// Store Variables
	$GLOBALS['commandParamPK_MediaType']=29;		// PK_MediaType
	$GLOBALS['commandParamPK_DeviceTemplate']=44;	// PK_DeviceTemplate
	$GLOBALS['commandParamPK_EntertainArea']=45;	// PK_EntertainArea
	$GLOBALS['commandParamAbsoluteLevel']=76;		// Absolute Level
	$GLOBALS['commandPhoneExtension']=83;			// PhoneExtension
	$GLOBALS['delayTime']=102;						// Delay time
	$GLOBALS['commandParamPK_DeviceTo']=263;		// PK_DeviceTo
	$GLOBALS['commandParamPK_Screen']=159;			// PK_Screen
	$GLOBALS['commandParameterObjectScreen']=16;	// Object(Screen)
	$GLOBALS['commandParameterListDevice']=103;		// List PK Device (string)
	
	// PK_DeviceData from table DeviceData
	$GLOBALS['Path']=2;							// the Path
	$GLOBALS['rootPK_Distro']=7;				// the PK_Distro
	$GLOBALS['FloorplanInfo']=10;				// FloorplanInfo (coordinates)
	$GLOBALS['FloorplanObjectType']=11;			// FloorplanObjectType
	$GLOBALS['Channel']=12;						// Channel
	$GLOBALS['InputOrOutput']=18;				// InputOrOutput
	$GLOBALS['Size']=25;						// Resolution (Size)
	$GLOBALS['Language']=26;					// Language
	$GLOBALS['DHCPDeviceData']=28;				// DHCP
	$GLOBALS['PhoneNumber']=31;					// Phone Number
	$GLOBALS['NetworkInterfaces']=32;			// Network Interfaces
	$GLOBALS['securityAlert']=33;				// Alert 
	$GLOBALS['MobileOrbiterNotification']=34;	// Mobile Orbiter Notification
	$GLOBALS['OtherPhoneNotifications']=35;		// Other Phone Notifications 
	$GLOBALS['NeighborstoCall']=36;				// Neighbors to Call 
	$GLOBALS['Port']=37;						// Port
	$GLOBALS['securityAlertType']=45;			// Alert type 
	$GLOBALS['InfraredPort']=12;				// Infrared Port
	$GLOBALS['PK_Device']=77;					// PK_Device
	$GLOBALS['AudioSettings']=88;				// Audio settings
	$GLOBALS['VideoSettings']=89;				// Video settings
	$GLOBALS['EmergencyCalls']=96;				// Emergency Calls
	$GLOBALS['MobileOrbiterPhone']=97;			// Mobile Orbiter Phone
	$GLOBALS['ScreenWidth']=100;				// Screen Width
	$GLOBALS['ScreenHeight']=101;				// Screen Height
	$GLOBALS['MediaProvider']=103;				// Media Provider
	$GLOBALS['PK_City']=107;					// City
	$GLOBALS['Longitude']=108;					// Longitude
	$GLOBALS['Latitude']=109;					// Latitude
	$GLOBALS['RipFormat']=47;					// Rip format (device data name is Type)
	$GLOBALS['RemotePhoneIP']=118;				// Remote Phone IP
	$GLOBALS['ListenPort']=119;					// Listen Port
	$GLOBALS['SupportIframes']=122;				// Support iframes
	$GLOBALS['RefreshInterval']=123;			// Refresh Interval
	$GLOBALS['AvailableSerialPorts']=133;		// Available Serial Ports
	$GLOBALS['OutputSpeakerArrangement']=71;	// Output Speaker arrangement
	$GLOBALS['TelecomLocalPrefix']=141;			// Telecom Local Prefix
	$GLOBALS['TelecomPrependDigit']=142;		// Telecom Prepend Digit
	$GLOBALS['TelecomLocalNumberLength']=143;	// Telecom Local Number Length
	$GLOBALS['PortChannel']=12;					// Port Channel
	$GLOBALS['PortChannelChoices']=151;			// Port Channel Choices
	$GLOBALS['DisklessBoot']=9;					// Diskless boot
	$GLOBALS['hardware_acceleration_dd']=168;	// Harware acceleration
	$GLOBALS['UsealphablendedUI']=169;			// Use alpha blended UI
	$GLOBALS['DeinterlacingMode']=170;			// Deinterlacing Mode
	$GLOBALS['UseOpenGLeffects']=172;			// Use OpenGL effects
	$GLOBALS['CaptureCardPort']=173;			// FK_Device_Capture_Card_Port
	$GLOBALS['Skin']=24;						// PK_Skin
	$GLOBALS['Lineup']=181;						// Lineup
	$GLOBALS['PK_MediaAttribute']=182;			// PK_MediaAttribute
	$GLOBALS['ComputerDomain']=187;				// Computer domain
	$GLOBALS['ComputerName']=188;				// Computer name
	$GLOBALS['DD_PK_UI']=104;					// UI
	$GLOBALS['State']=200;						// State
	$GLOBALS['DriveSize']=201;					// Drive size
	$GLOBALS['RAIDStatus']=204;					// Raid status
	$GLOBALS['NoofDisks']=199;					// No of Disks
	$GLOBALS['Spare']=202;						// Spare
	$GLOBALS['BlockDevice']=152;				// Block device
	$GLOBALS['use_automaticly_dd']=134;			// Use automaticly
	$GLOBALS['use_pluto_directory_structure_dd']=130;	// use pluto directory structure
	$GLOBALS['DD_OS_ON_HARD_DRIVE']=211;		// OS on hard drive
	$GLOBALS['DD_PK_Users']=3;					// PK_Users
	$GLOBALS['DD_RemoteAssistancePorts']=212;	// Remote Assistance Ports
	$GLOBALS['DD_CONNECTOR']=68;				// Connector
	$GLOBALS['DD_TV_STANDARD']=229;				// TV standard
	$GLOBALS['DD_ManualConfiguration']=237;		// Phones manual configuration
	$GLOBALS['DD_updateErrors']=259;			// Update errors 
	$GLOBALS['DD_model']=233;					// model
	$GLOBALS['DD_LastUpdate']=234;				// Last Update
	$GLOBALS['DD_provider']=265;				// Provider
	$GLOBALS['rootDevelopment']=8;				// the PK_DeviceData for description Development from table DeviceData
	$GLOBALS['rootDisklessBoot']=9;				// the PK_DeviceData for Diskless Boot from table DeviceData

	
	// PK_FloorplanType from table FloorplanType 

	$GLOBALS['SecurityFoorplanType']=1;		// Security Zone
	$GLOBALS['LightingFoorplanType']=2;		// Lighting Zone
	$GLOBALS['ClimateFoorplanType']=3;		// Climate Zone
	$GLOBALS['CameraFoorplanType']=4;		// Camera Zone
	$GLOBALS['EntertainmentZone']=5;		// Entertainment Zone
	$GLOBALS['AVEquipmentFlorplanType']=6;	// AV Equipment
	$GLOBALS['PhonesFoorplanType']=7;		// Phones
	$GLOBALS['RoomsZone']=8;				// Rooms
	
	// PK_DesignObj from table DesignObj
	
	$GLOBALS['mnuSecurityCamerasDesignObj']=1414;		// Security cameras
	$GLOBALS['mnuNoMediaInDrive']=1696;					// mnuNoMediaInDrive
	$GLOBALS['mnuSingleCameraViewOnlyDesignObj']=1405;	// Single Camera View Only
	$GLOBALS['mnuMediaFileListDesignObj']=2071;			// mnuMediaFileList
	$GLOBALS['mnuMediaFileList2DesignObj']=3226;		// mnuMediaFileList2
	$GLOBALS['butViewDesignObj']=3283;					// butView
	$GLOBALS['TelecomMenu']=1323;						// Telecom Menu mnuMakeCallFavorites
	$GLOBALS['mnuPicturesDocumentsList']=3282;			// mnuPicturesDocumentsList
	$GLOBALS['mnuSecurityPanel']=1389;					// mnuSecurityPanel for Arm/Disarm
	
	// PK_CriteriaList from table CriteriaList
	
	$GLOBALS['EventCriteriaList']=2;			// Events

	// PK_CriteriaParmList from table CriteriaParmList
	
	$GLOBALS['TimeOfDayParmList']=11;					// Interval and also Time of Day
	$GLOBALS['DayOfWeekParmList']=17;					// Day Of Week
	$GLOBALS['DayOfMonthParmList']=18;					// Day Of Month
	$GLOBALS['SpecificDateParmList']=19;				// Specific Date or Absolute
	$GLOBALS['PK_EventParameterParmList']=24;			// PK_EventParameter

	// PK_Variable from table Variable
	
	$GLOBALS['camerasVariableNumbersArray']=array(18,19,20,21);
	$GLOBALS['cameraVariable']=1;
	$GLOBALS['PK_DeviceGroup']=32;
	
	// PK_StartupScript from table StartupScript
	$GLOBALS['ProcessLogs']=45;					// Process_Logs.sh
	$GLOBALS['ShareIRCodes']=49;				// Share IR codes
	$GLOBALS['UpdateMediaDaemon']=61;			// UpdateMediaDaemon
	
	// PK_CommandCategory
	$GLOBALS['AVCommandCategory']=12;			// AV commands
	$GLOBALS['DSPModeCommandCategory']=21;		// DSP Modes
	$GLOBALS['InputsCommandCategory']=22;		// Inputs
	$GLOBALS['OutputsCommandCategory']=27;		// Outputs
	$GLOBALS['GeneralInternal']=55;				// General/Internal
	
	// TextCategory table
	$GLOBALS['PlutoAdminCategory']=208;			// Pluto admin category
	
	// options to display in Media Wizard
	
	$GLOBALS['mediaOptionsArray']=array('TV', 'playlists', 'music', 'movies', 'videos', 'pictures', 'documents', 'Disc CD/DVD');
	
	// DeviceData displayed as pulldown menu
	
	$GLOBALS['DeviceDataLinkedToTables']=array('PK_Distro','PK_FloorplanObjectType','PK_Skin','PK_Size','PK_Language','PK_Users','PK_AlertType','PK_UI');
	
	// PK_Device for Local Orbiter
	
	$GLOBALS['localOrbiter']=-300;
	
	// PK_Pipe from table Pipe
	
	$GLOBALS['AudioPipe']=1;		// Audio Pipe
	$GLOBALS['VideoPipe']=2;		// Video Pipe
	
	// PK_PageSetup from table PageSetup
	$GLOBALS['InstallationPage']=11;	// Installation Settings

	// PK_Package from table Package
	$GLOBALS['orbiterInstallerPackage']=363;

	// DeviceCommandGroups
	$GLOBALS['ThermostatCommands']=36;
	
	// date format
	$GLOBALS['defaultDateFormat']='Y-m-d H:i:s';

	$GLOBALS['GenericSerialDeviceCommandLine']='Generic_Serial_Device';		// the command line used to identify generic serial devices
	
	$GLOBALS['SecurityPicsPath']='/home/securitypic/';
	
	// criticals device templates; user will be notified not to delete them
	$GLOBALS['CriticalDeviceTemplates']=array(1,2,6,7,8,9,10,12,27,28,31,32,33,34,36,39);

  	$GLOBALS['HTTPorFTP']=array(2,5,6);  	// the PK_RepositoryType for HTTP FTP and both from table RepositoryType
	$GLOBALS['CVSorSVN']=array(2,5,6);  	// the PK_RepositoryType for CVS or SVN  from table RepositoryType
	
	$GLOBALS['hardware_acceleration']=array(
	'cle266x11'=>'cle266x11',
	'xv'=>'xv',
	'xxmc'=>'xxmc',
	'opengl'=>'opengl',
	'sdl'=>'sdl',
	'xshm'=>'xshm');
	
	$GLOBALS['vdr_from_path']='/usr/pluto/templates/vdr/';
?>
