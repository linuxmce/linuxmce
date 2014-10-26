// UI1 Screen VideoCallInProgress (18)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/callinprogressb.png"
		width: 1920
		height: 1080
	}
	function onActivate1284() 
	{
	 // Calling Command 336 (Phone_Drop) with CommandGroup_Parameters from 16514
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 336 ")
	}
	StyledText {
		x: 1227
		y: 54
		height: 240
		width: 162
		textLabel: "Drop Call"
	}
	ButtonSq {
		id: button1284_214
		x: 1227
		y: 54
		width: 217
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Drop Call"
		clickHandler.onClicked: onActivate1284()
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
		id: button1785_215
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
		id: button1784_216
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
}