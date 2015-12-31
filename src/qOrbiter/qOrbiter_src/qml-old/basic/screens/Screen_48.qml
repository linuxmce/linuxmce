// UI1 Screen PVR Remote (48)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/PVRremote.png"
		width: 1920
		height: 1080
	}
	function onActivate5658() 
	{
	 // Calling Command 187 (Tune to channel) with CommandGroup_Parameters from 17708
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 187 68 \"<%=35%>\" ")
	}
	function onActivate1785() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17174
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 1678
		y: 0
		height: 1
		width: 1
		textLabel: "Main menu"
	}
	ButtonSq {
		id: button1785_566
		x: 1678
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate2445() 
	{
	 // Calling Command 126 (Guide) with CommandGroup_Parameters from 16669
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 126 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17308
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 1678
		y: 180
		height: 240
		width: 180
		textLabel: "guide"
	}
	ButtonSq {
		id: button2445_567
		x: 1678
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "guide"
		clickHandler.onClicked: onActivate2445()
	}
	function onActivate2441() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14732
		manager.sendDceMessage(srouterip + " "+deviceid + " -104 1 184 76 \"+10\" ")
	}
	ButtonSq {
		id: button2441_568
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
		id: button2443_569
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
		id: button2442_570
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
		id: button2289_571
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
		id: button1850_572
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
		id: button1851_573
		x: 960
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1851()
	}
	function onActivate1569() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13739
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18035
		setCurrentScreen("Screen_1.qml")
	}
	ButtonSq {
		id: button1569_574
		x: 1678
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
	ButtonSq {
		id: button5659_575
		x: 1200
		y: 360
		width: 720
		height: 540
		buttonbackground: "skins/Basic/798"
		buttontext: ""
		clickHandler.onClicked: onActivate5659()
	}
	function onActivate1582() 
	{
	 // Calling Command 39 (Pause Media) with CommandGroup_Parameters from 14163
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 39 ")
	}
	ButtonSq {
		id: button1582_576
		x: 1440
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/enter.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1582()
	}
	function onActivate4283() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 15946
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"3\" 50 \"LeftArrow\" ")
	}
	ButtonSq {
		id: button4283_577
		x: 1200
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_move_left.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4283()
	}
	function onActivate4284() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 15947
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"4\" 50 \"RightArrow\" ")
	}
	ButtonSq {
		id: button4284_578
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_move_right.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4284()
	}
	function onActivate4282() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 15945
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"2\" 50 \"DownArrow\" ")
	}
	ButtonSq {
		id: button4282_579
		x: 1440
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_move_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4282()
	}
	function onActivate4281() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 15944
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"1\" 50 \"UpArrow\" ")
	}
	ButtonSq {
		id: button4281_580
		x: 1440
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_move_up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4281()
	}
	function onActivate4770() 
	{
	 // Calling Command 123 (Info) with CommandGroup_Parameters from 16671
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 123 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17312
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 1200
		y: 360
		height: 240
		width: 180
		textLabel: "info"
	}
	ButtonSq {
		id: button4770_581
		x: 1200
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "info"
		clickHandler.onClicked: onActivate4770()
	}
	function onActivate5681() 
	{
	 // Calling Command 409 (Save Bookmark) with CommandGroup_Parameters from 17776
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 409 17 \"<%=U%>\" 29 \"1\" 45 \"<%=E%>\" 163 \"Bookmark added from orbiter <%=!%>\" 188 \" CHAN:<%=35%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18208
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	ButtonSq {
		id: button5681_582
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/br.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5681()
	}
	function onActivate1566() 
	{
	 // Calling Command 102 (Record) with CommandGroup_Parameters from 13636
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 102 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18381
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	ButtonSq {
		id: button1566_583
		x: 1440
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/record.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1566()
	}
	function onActivate2028() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 9488
		setCurrentScreen("Screen_52.qml")
	}
	StyledText {
		x: 1200
		y: 900
		height: 240
		width: 180
		textLabel: "Resend
A/V
codes"
	}
	ButtonSq {
		id: button2028_584
		x: 1200
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Resend
A/V
codes"
		clickHandler.onClicked: onActivate2028()
	}
	function onActivate4771() 
	{
	 // Calling Command 548 (Menu) with CommandGroup_Parameters from 16672
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 548 9 \"old_recordings\" ")
	}
	StyledText {
		x: 720
		y: 900
		height: 240
		width: 180
		textLabel: "Recordings"
	}
	ButtonSq {
		id: button4771_585
		x: 720
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Recordings
"
		clickHandler.onClicked: onActivate4771()
	}
	function onActivate4772() 
	{
	 // Calling Command 548 (Menu) with CommandGroup_Parameters from 16673
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 548 9 \"future_recordings\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17309
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 960
		y: 900
		height: 240
		width: 180
		textLabel: "Schedule"
	}
	ButtonSq {
		id: button4772_586
		x: 960
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Schedule
"
		clickHandler.onClicked: onActivate4772()
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
		id: button2455_587
		x: 720
		y: 0
		width: 480
		height: 360
		buttonbackground: "skins/Basic/532"
		buttontext: "<%=NP_SPEED%>"
		clickHandler.onClicked: onActivate2455()
	}
	ButtonSq {
		id: button3368_588
		x: 1200
		y: 0
		width: 480
		height: 360
		buttonbackground: "skins/Basic/Backgrounds/logo.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3368()
	}
	function onActivate5661() 
	{
	 // Calling Command 9 (Scroll Grid) with CommandGroup_Parameters from 17709
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 9 30 \"1\" ")
	}
	StyledText {
		x: 0
		y: 968
		height: 120
		width: 270
		textLabel: "Previous"
	}
	ButtonSq {
		id: button5661_589
		x: 0
		y: 968
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Previous"
		clickHandler.onClicked: onActivate5661()
	}
	function onActivate5662() 
	{
	 // Calling Command 9 (Scroll Grid) with CommandGroup_Parameters from 17710
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 9 30 \"2\" ")
	}
	StyledText {
		x: 361
		y: 968
		height: 120
		width: 270
		textLabel: "Next"
	}
	ButtonSq {
		id: button5662_590
		x: 361
		y: 968
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Next"
		clickHandler.onClicked: onActivate5662()
	}
}