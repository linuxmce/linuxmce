// UI1 Screen popLights (147)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/popup.png"
		width: 1920
		height: 1080
	}
	StyledText {
		x: 120
		y: 90
		height: 240
		width: 180
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button1257_1599
		x: 120
		y: 90
		width: 240
		height: 177
		buttonbackground: "skins/Basic/Buttons/Lights/squareb.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate1257()
	}
	ButtonSq {
		id: button3390_1600
		x: 267
		y: 200
		width: 180
		height: 135
		buttonbackground: "skins/Basic/../../scenarios/<%=5:0%>.png|<%=IM%>"
		buttontext: ""
		clickHandler.onClicked: onActivate3390()
	}
	function onActivate3468() 
	{
	 // Calling Command 398 (Remove Popup) with CommandGroup_Parameters from 14916
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 398 ")
	}
	StyledText {
		x: 120
		y: 670
		height: 240
		width: 180
		textLabel: "Hide
Popup"
	}
	ButtonSq {
		id: button3468_1601
		x: 120
		y: 670
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Hide
Popup"
		clickHandler.onClicked: onActivate3468()
	}
	function onActivate2431() 
	{
	 // Calling Command 408 (Show Floorplan) with CommandGroup_Parameters from 15022
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 408 11 \"0\" 12 \"0\" 14 \"light\" ")
	}
	ButtonSq {
		id: button2431_1602
		x: 1068
		y: 670
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Lights/mainmenu.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2431()
	}
}