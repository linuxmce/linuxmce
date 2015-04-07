// UI1 Screen Power (76)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
	}
	function onActivate2436() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 12457
		setCurrentScreen("Screen_64.qml")
	}
	ButtonSq {
		id: button2436_926
		x: 240
		y: 135
		width: 360
		height: 180
		buttonbackground: "skins/Basic/Buttons/user_room.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2436()
	}
	function onActivate3345() 
	{
	 // Calling Command 323 (Halt Device) with CommandGroup_Parameters from 14349
		manager.sendDceMessage(srouterip + " "+deviceid + " -103 1 323 2 \"<%=MD%>\" 21 \"N\" ")
	}
	StyledText {
		x: 1200
		y: 0
		height: 240
		width: 270
		textLabel: "Start
As LinuxMCE"
	}
	ButtonSq {
		id: button3345_927
		x: 1200
		y: 0
		width: 360
		height: 180
		buttonbackground: "skins/Basic/Buttons/user_room.png"
		buttontext: "Start
As LinuxMCE"
		clickHandler.onClicked: onActivate3345()
	}
	function onActivate3344() 
	{
	 // Calling Command 323 (Halt Device) with CommandGroup_Parameters from 14348
		manager.sendDceMessage(srouterip + " "+deviceid + " -103 1 323 2 \"<%=MD%>\" 21 \"V\" ")
	}
	StyledText {
		x: 1200
		y: 180
		height: 240
		width: 270
		textLabel: "Restart
As <%=OS%>"
	}
	ButtonSq {
		id: button3344_928
		x: 1200
		y: 180
		width: 360
		height: 180
		buttonbackground: "skins/Basic/Buttons/user_room.png"
		buttontext: "Restart
As <%=OS%>"
		clickHandler.onClicked: onActivate3344()
	}
	function onActivate3333() 
	{
	 // Calling Command 322 (Wake Device) with CommandGroup_Parameters from 14257
		manager.sendDceMessage(srouterip + " "+deviceid + " -103 1 322 2 \"<%=MD%>\" ")
	 // Calling Command 192 (On) with CommandGroup_Parameters from 17372
		manager.sendDceMessage(srouterip + " "+deviceid + " -200 1 192 ")
	}
	StyledText {
		x: 240
		y: 540
		height: 240
		width: 270
		textLabel: "Display
On"
	}
	ButtonSq {
		id: button3333_929
		x: 240
		y: 540
		width: 360
		height: 180
		buttonbackground: "skins/Basic/Buttons/user_room.png"
		buttontext: "Display
On"
		clickHandler.onClicked: onActivate3333()
	}
	function onActivate3335() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 14258
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	}
	StyledText {
		x: 720
		y: 540
		height: 240
		width: 270
		textLabel: "<%=NP%>"
	}
	ButtonSq {
		id: button3335_930
		x: 720
		y: 540
		width: 360
		height: 180
		buttonbackground: "skins/Basic/Buttons/user_room.png"
		buttontext: "<%=NP%>"
		clickHandler.onClicked: onActivate3335()
	}
	function onActivate3338() 
	{
	 // Calling Command 3 (Display On/Off) with CommandGroup_Parameters from 14839
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 3 8 \"0\" ")
	}
	StyledText {
		x: 1080
		y: 540
		height: 240
		width: 270
		textLabel: "Display
Off"
	}
	ButtonSq {
		id: button3338_931
		x: 1080
		y: 540
		width: 360
		height: 180
		buttonbackground: "skins/Basic/Buttons/user_room.png"
		buttontext: "Display
Off"
		clickHandler.onClicked: onActivate3338()
	}
	function onActivate3339() 
	{
	 // Calling Command 323 (Halt Device) with CommandGroup_Parameters from 14260
		manager.sendDceMessage(srouterip + " "+deviceid + " -103 1 323 2 \"<%=MD%>\" ")
	}
	StyledText {
		x: 1080
		y: 720
		height: 240
		width: 270
		textLabel: "Power OFF m/d"
	}
	ButtonSq {
		id: button3339_932
		x: 1080
		y: 720
		width: 360
		height: 180
		buttonbackground: "skins/Basic/Buttons/user_room.png"
		buttontext: "Power OFF m/d"
		clickHandler.onClicked: onActivate3339()
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
		id: button1785_933
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_934
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate3340() 
	{
	 // Calling Command 193 (Off) with CommandGroup_Parameters from 14266
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 193 ")
	}
	StyledText {
		x: 1680
		y: 900
		height: 240
		width: 180
		textLabel: "Display
Off"
	}
	ButtonSq {
		id: button3340_935
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Display
Off"
		clickHandler.onClicked: onActivate3340()
	}
	function onActivate3341() 
	{
	 // Calling Command 7 (Terminate Orbiter) with CommandGroup_Parameters from 14347
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 7 ")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "Exit
LinuxMCE"
	}
	ButtonSq {
		id: button3341_936
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Exit
LinuxMCE"
		clickHandler.onClicked: onActivate3341()
	}
	StyledText {
		x: 1680
		y: 180
		height: 240
		width: 180
		textLabel: "Switch to computing desktop"
	}
	ButtonSq {
		id: button5434_937
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Switch to computing desktop"
		clickHandler.onClicked: onActivate5434()
	}
	function onActivate3348() 
	{
	 // Calling Command 322 (Wake Device) with CommandGroup_Parameters from 17619
		manager.sendDceMessage(srouterip + " "+deviceid + " -103 1 322 2 \"<%=MD%>\" ")
	}
	StyledText {
		x: 240
		y: 720
		height: 240
		width: 270
		textLabel: "Power ON m/d"
	}
	ButtonSq {
		id: button3348_938
		x: 240
		y: 720
		width: 360
		height: 180
		buttonbackground: "skins/Basic/Buttons/user_room.png"
		buttontext: "Power ON m/d"
		clickHandler.onClicked: onActivate3348()
	}
}