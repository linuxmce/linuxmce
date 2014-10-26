// UI1 Screen OtherFolders (31)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	ButtonSq {
		id: button1903_345
		x: 478
		y: 68
		width: 677
		height: 947
		buttonbackground: "skins/Basic/2"
		buttontext: ""
		clickHandler.onClicked: onActivate1903()
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
		id: button1785_346
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
		id: button1784_347
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
		id: button1780_348
		x: 1680
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
		id: button1781_349
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1781()
	}
	StyledText {
		x: 1191
		y: 846
		height: 240
		width: 180
		textLabel: "Save"
	}
	ButtonSq {
		id: button1904_350
		x: 1191
		y: 846
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/roundblack.png"
		buttontext: "Save"
		clickHandler.onClicked: onActivate1904()
	}
}