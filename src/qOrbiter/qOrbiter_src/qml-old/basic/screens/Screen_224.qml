// UI1 Screen mnuSpeedControl (224)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/PVRremote.png"
		width: 1920
		height: 1080
	}
	function onActivate1785() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17174
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 1680
		y: 0
		height: 1
		width: 1
		textLabel: "Main menu"
	}
	ButtonSq {
		id: button1785_2415
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate1569() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13739
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18035
		setCurrentScreen("Screen_1.qml")
	}
	ButtonSq {
		id: button1569_2416
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
	ButtonSq {
		id: button3368_2417
		x: 1200
		y: 0
		width: 480
		height: 360
		buttonbackground: "skins/Basic/Backgrounds/logo.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3368()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_2418
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate2441() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14732
		manager.sendDceMessage(srouterip + " "+deviceid + " -104 1 184 76 \"+10\" ")
	}
	ButtonSq {
		id: button2441_2419
		x: 720
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2441()
	}
	function onActivate2443() 
	{
	 // Calling Command 408 (Show Floorplan) with CommandGroup_Parameters from 15042
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 408 11 \"100\" 12 \"100\" 14 \"light\" ")
	}
	StyledText {
		x: 720
		y: 540
		height: 5
		width: 3
		textLabel: "Lights"
	}
	ButtonSq {
		id: button2443_2420
		x: 720
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_map.png"
		buttontext: "Lights"
		clickHandler.onClicked: onActivate2443()
	}
	function onActivate2442() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14733
		manager.sendDceMessage(srouterip + " "+deviceid + " -104 1 184 76 \"-10\" ")
	}
	ButtonSq {
		id: button2442_2421
		x: 720
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2442()
	}
	function onActivate2289() 
	{
	 // Calling Command 97 (Mute) with CommandGroup_Parameters from 13657
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 97 ")
	}
	StyledText {
		x: 960
		y: 540
		height: 240
		width: 180
		textLabel: "Mute"
	}
	ButtonSq {
		id: button2289_2422
		x: 960
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_mute.png"
		buttontext: "Mute"
		clickHandler.onClicked: onActivate2289()
	}
	function onActivate1850() 
	{
	 // Calling Command 89 (Vol Up) with CommandGroup_Parameters from 13671
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 89 ")
	}
	ButtonSq {
		id: button1850_2423
		x: 960
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1850()
	}
	function onActivate1851() 
	{
	 // Calling Command 90 (Vol Down) with CommandGroup_Parameters from 13672
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 90 ")
	}
	ButtonSq {
		id: button1851_2424
		x: 960
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1851()
	}
	function onActivate5695() 
	{
	 // Calling Command 42 (Jump to Position in Stream) with CommandGroup_Parameters from 17790
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 42 5 \"+480\" ")
	}
	StyledText {
		x: 1560
		y: 900
		height: 240
		width: 90
		textLabel: "8 m"
	}
	ButtonSq {
		id: button5695_2425
		x: 1560
		y: 900
		width: 120
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: "8 m"
		clickHandler.onClicked: onActivate5695()
	}
	function onActivate5693() 
	{
	 // Calling Command 42 (Jump to Position in Stream) with CommandGroup_Parameters from 17788
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 42 5 \"+240\" ")
	}
	StyledText {
		x: 1440
		y: 899
		height: 240
		width: 90
		textLabel: "4 m"
	}
	ButtonSq {
		id: button5693_2426
		x: 1440
		y: 899
		width: 120
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: "4 m"
		clickHandler.onClicked: onActivate5693()
	}
	function onActivate5691() 
	{
	 // Calling Command 42 (Jump to Position in Stream) with CommandGroup_Parameters from 17786
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 42 5 \"+120\" ")
	}
	StyledText {
		x: 1320
		y: 899
		height: 240
		width: 90
		textLabel: "2 m"
	}
	ButtonSq {
		id: button5691_2427
		x: 1320
		y: 899
		width: 120
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: "2 m"
		clickHandler.onClicked: onActivate5691()
	}
	function onActivate5689() 
	{
	 // Calling Command 42 (Jump to Position in Stream) with CommandGroup_Parameters from 17784
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 42 5 \"+60\" ")
	}
	StyledText {
		x: 1200
		y: 900
		height: 240
		width: 90
		textLabel: "1 m"
	}
	ButtonSq {
		id: button5689_2428
		x: 1200
		y: 900
		width: 120
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: "1 m"
		clickHandler.onClicked: onActivate5689()
	}
	function onActivate5687() 
	{
	 // Calling Command 42 (Jump to Position in Stream) with CommandGroup_Parameters from 17782
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 42 5 \"+30\" ")
	}
	StyledText {
		x: 1080
		y: 900
		height: 240
		width: 90
		textLabel: "30 s"
	}
	ButtonSq {
		id: button5687_2429
		x: 1080
		y: 900
		width: 120
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: "30 s"
		clickHandler.onClicked: onActivate5687()
	}
	function onActivate5685() 
	{
	 // Calling Command 42 (Jump to Position in Stream) with CommandGroup_Parameters from 17780
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 42 5 \"+15\" ")
	}
	StyledText {
		x: 960
		y: 900
		height: 240
		width: 90
		textLabel: "15 s"
	}
	ButtonSq {
		id: button5685_2430
		x: 960
		y: 900
		width: 120
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: "15 s"
		clickHandler.onClicked: onActivate5685()
	}
	function onActivate1582() 
	{
	 // Calling Command 39 (Pause Media) with CommandGroup_Parameters from 14163
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 39 ")
	}
	ButtonSq {
		id: button1582_2431
		x: 720
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/enter.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1582()
	}
	function onActivate5694() 
	{
	 // Calling Command 42 (Jump to Position in Stream) with CommandGroup_Parameters from 17789
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 42 5 \"-480\" ")
	}
	StyledText {
		x: 0
		y: 900
		height: 240
		width: 90
		textLabel: "8 m"
	}
	ButtonSq {
		id: button5694_2432
		x: 0
		y: 900
		width: 120
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: "8 m"
		clickHandler.onClicked: onActivate5694()
	}
	function onActivate5692() 
	{
	 // Calling Command 42 (Jump to Position in Stream) with CommandGroup_Parameters from 17787
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 42 5 \"-240\" ")
	}
	StyledText {
		x: 120
		y: 900
		height: 240
		width: 90
		textLabel: "4 m"
	}
	ButtonSq {
		id: button5692_2433
		x: 120
		y: 900
		width: 120
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: "4 m"
		clickHandler.onClicked: onActivate5692()
	}
	function onActivate5690() 
	{
	 // Calling Command 42 (Jump to Position in Stream) with CommandGroup_Parameters from 17785
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 42 5 \"-120\" ")
	}
	StyledText {
		x: 240
		y: 900
		height: 240
		width: 90
		textLabel: "2 m"
	}
	ButtonSq {
		id: button5690_2434
		x: 240
		y: 900
		width: 120
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: "2 m"
		clickHandler.onClicked: onActivate5690()
	}
	function onActivate5688() 
	{
	 // Calling Command 42 (Jump to Position in Stream) with CommandGroup_Parameters from 17783
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 42 5 \"-60\" ")
	}
	StyledText {
		x: 360
		y: 900
		height: 240
		width: 90
		textLabel: "1 m"
	}
	ButtonSq {
		id: button5688_2435
		x: 360
		y: 900
		width: 120
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: "1 m"
		clickHandler.onClicked: onActivate5688()
	}
	function onActivate5687() 
	{
	 // Calling Command 42 (Jump to Position in Stream) with CommandGroup_Parameters from 17782
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 42 5 \"+30\" ")
	}
	StyledText {
		x: 480
		y: 900
		height: 240
		width: 90
		textLabel: "30 s"
	}
	ButtonSq {
		id: button5687_2436
		x: 480
		y: 900
		width: 120
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: "30 s"
		clickHandler.onClicked: onActivate5687()
	}
	function onActivate5684() 
	{
	 // Calling Command 42 (Jump to Position in Stream) with CommandGroup_Parameters from 17779
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 42 5 \"-15\" ")
	}
	StyledText {
		x: 600
		y: 900
		height: 240
		width: 90
		textLabel: "15 s"
	}
	ButtonSq {
		id: button5684_2437
		x: 600
		y: 900
		width: 120
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: "15 s"
		clickHandler.onClicked: onActivate5684()
	}
	function onActivate2338() 
	{
	 // Calling Command 65 (Jump Position In Playlist) with CommandGroup_Parameters from 13915
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 65 5 \"<%=35%>\" ")
	}
	StyledText {
		x: 729
		y: 7
		height: 208
		width: 346
		textLabel: "<%=NP%>"
	}
	StyledText {
		x: 720
		y: 169
		height: 164
		width: 360
		textLabel: "<%=NP_SEC%>"
	}
	StyledText {
		x: 720
		y: 292
		height: 90
		width: 271
		textLabel: "<%=NP_TIME_SHORT%>"
	}
	StyledText {
		x: 1081
		y: 292
		height: 90
		width: 89
		textLabel: "<%=NP_SPEED%>"
	}
	ButtonSq {
		id: button2455_2438
		x: 720
		y: 0
		width: 480
		height: 360
		buttonbackground: "skins/Basic/532"
		buttontext: "<%=NP_SPEED%>"
		clickHandler.onClicked: onActivate2455()
	}
}