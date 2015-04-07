// UI1 Screen ManagePlaylist (20)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
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
		id: button1785_271
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
		id: button1784_272
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
		id: button1780_273
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
		id: button1781_274
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1781()
	}
	function onActivate2553() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 12682
		setCurrentScreen("Screen_19.qml")
	}
	StyledText {
		x: 1680
		y: 900
		height: 240
		width: 180
		textLabel: "Save Playlist"
	}
	ButtonSq {
		id: button2553_275
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Save Playlist"
		clickHandler.onClicked: onActivate2553()
	}
	function onActivate2338() 
	{
	 // Calling Command 65 (Jump Position In Playlist) with CommandGroup_Parameters from 13915
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 65 5 \"<%=35%>\" ")
	}
	function onActivate3323() 
	{
	 // Calling Command 271 (Remove playlist entry.) with CommandGroup_Parameters from 14225
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 271 48 \"<%=35%>\" ")
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "Delete
entry"
	}
	ButtonSq {
		id: button3323_276
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Delete
entry"
		clickHandler.onClicked: onActivate3323()
	}
}