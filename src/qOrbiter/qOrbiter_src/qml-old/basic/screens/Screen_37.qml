// UI1 Screen RecordedTVShows (37)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/PVR/resultsB.png"
		width: 1920
		height: 1080
	}
	function onActivate1596() 
	{
	 // Calling Command 43 (MH Play Media) with CommandGroup_Parameters from 13879
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 43 13 \"*<%=7%>\" 45 \"<%=E%>\" ")
	}
	StyledText {
		x: 1462
		y: 846
		height: 240
		width: 162
		textLabel: "Q"
	}
	ButtonSq {
		id: button1596_415
		x: 1462
		y: 846
		width: 217
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/play.png"
		buttontext: "Q"
		clickHandler.onClicked: onActivate1596()
	}
	function onActivate1597() 
	{
	}
	ButtonSq {
		id: button1597_416
		x: 77
		y: 257
		width: 1353
		height: 744
		buttonbackground: "skins/Basic/7"
		buttontext: ""
		clickHandler.onClicked: onActivate1597()
	}
	function onActivate1785() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17174
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 1696
		y: 0
		height: 1
		width: 1
		textLabel: "Main menu"
	}
	ButtonSq {
		id: button1785_417
		x: 1696
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
		id: button1784_418
		x: 1696
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
		id: button1780_419
		x: 1696
		y: 360
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
		id: button1781_420
		x: 1696
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1781()
	}
	function onActivate1912() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2818
		setCurrentScreen("Screen_32.qml")
	}
	StyledText {
		x: 1462
		y: 677
		height: 240
		width: 162
		textLabel: "Delete"
	}
	ButtonSq {
		id: button1912_421
		x: 1462
		y: 677
		width: 217
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Delete"
		clickHandler.onClicked: onActivate1912()
	}
	function onActivate1687() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14091
		setCurrentScreen("Screen_24.qml")
	}
	StyledText {
		x: 1462
		y: 244
		height: 240
		width: 270
		textLabel: "Select
Current
User"
	}
	ButtonSq {
		id: button1687_422
		x: 1462
		y: 244
		width: 360
		height: 180
		buttonbackground: "skins/Basic/Buttons/user_room.png"
		buttontext: "Select
Current
User"
		clickHandler.onClicked: onActivate1687()
	}
	StyledText {
		x: 1462
		y: 508
		height: 240
		width: 162
		textLabel: "Sort"
	}
	ButtonSq {
		id: button1961_423
		x: 1462
		y: 508
		width: 217
		height: 180
		buttonbackground: "skins/Basic/Buttons/roundblack.png"
		buttontext: "Sort"
		clickHandler.onClicked: onActivate1961()
	}
}