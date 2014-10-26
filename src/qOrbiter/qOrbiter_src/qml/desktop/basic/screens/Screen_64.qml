// UI1 Screen CurrentLocation (64)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/locations.png"
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
		id: button1785_781
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	ButtonSq {
		id: button2438_782
		x: 240
		y: 180
		width: 0
		height: 0
		buttonbackground: "skins/Basic/14"
		buttontext: ""
		clickHandler.onClicked: onActivate2438()
	}
	function onActivate2439() 
	{
	 // Calling Command 88 (Set Current Location) with CommandGroup_Parameters from 13581
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 88 65 \"<%=5:0%>\" ")
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 14271
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	StyledText {
		x: 240
		y: 180
		height: 240
		width: 270
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button2439_783
		x: 240
		y: 180
		width: 360
		height: 180
		buttonbackground: "skins/Basic/Buttons/user_room.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate2439()
	}
	ButtonSq {
		id: button1804_784
		x: 597
		y: 374
		width: 144
		height: 108
		buttonbackground: "skins/Basic/../../rooms/<%=RIL%>.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1804()
	}
}