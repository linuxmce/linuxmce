// UI1 Screen DeviceControl (38)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate1967() 
	{
	 // Calling Command 35 (Populate Datagrid) with CommandGroup_Parameters from 14155
		manager.sendDceMessage(srouterip + " "+deviceid + " -102 1 35 15 \"cmds_<%=!%>\" 38 \"24\" 39 \"<%=2%>,<%=!%>,670\" ")
	 // Calling Command 25 (Set Text) with CommandGroup_Parameters from 14156
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 25 25 \"670\" ")
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 14196
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"3298\" 8 \"<%=2%>\" ")
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
		id: button1785_424
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
		id: button1784_425
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate1798() 
	{
	}
	function onActivate1780() 
	{
	 // Calling Command 269 (Move Playlist entry Up) with CommandGroup_Parameters from 14223
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 269 48 \"<%=35%>\" ")
	}
	ButtonSq {
		id: button1780_426
		x: 1440
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1780()
	}
	function onActivate1781() 
	{
	 // Calling Command 270 (Move Playlist entry Down) with CommandGroup_Parameters from 14224
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 270 48 \"<%=35%>\" ")
	}
	ButtonSq {
		id: button1781_427
		x: 1440
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1781()
	}
	function onActivate2231() 
	{
	}
	StyledText {
		x: 1680
		y: 540
		height: 236
		width: 180
		textLabel: "Cancel"
	}
	ButtonSq {
		id: button2231_428
		x: 1680
		y: 540
		width: 240
		height: 177
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Cancel"
		clickHandler.onClicked: onActivate2231()
	}
	function onActivate2266() 
	{
	}
	StyledText {
		x: 1200
		y: 720
		height: 240
		width: 180
		textLabel: "33Khz"
	}
	ButtonSq {
		id: button2266_429
		x: 1200
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "33Khz"
		clickHandler.onClicked: onActivate2266()
	}
	function onActivate2267() 
	{
	}
	StyledText {
		x: 1440
		y: 720
		height: 240
		width: 180
		textLabel: "45Khz"
	}
	ButtonSq {
		id: button2267_430
		x: 1440
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "45Khz"
		clickHandler.onClicked: onActivate2267()
	}
	function onActivate2268() 
	{
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "60Khz"
	}
	ButtonSq {
		id: button2268_431
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "60Khz"
		clickHandler.onClicked: onActivate2268()
	}
	ButtonSq {
		id: button3294_432
		x: 614
		y: 68
		width: 559
		height: 947
		buttonbackground: "skins/Basic/5"
		buttontext: ""
		clickHandler.onClicked: onActivate3294()
	}
	function onActivate3298() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14194
		setCurrentScreen("Screen_73.qml")
	}
	StyledText {
		x: 1440
		y: 360
		height: 240
		width: 180
		textLabel: "Try other codes"
	}
	ButtonSq {
		id: button3298_433
		x: 1440
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Try other codes"
		clickHandler.onClicked: onActivate3298()
	}
}