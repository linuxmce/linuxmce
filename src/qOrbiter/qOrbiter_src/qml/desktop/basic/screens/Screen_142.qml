// UI1 Screen GenericAppFullScreen (142)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate3558() 
	{
	}
	function onActivate1785() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17174
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 0
		y: 1015
		height: 1
		width: 1
		textLabel: "Main menu"
	}
	ButtonSq {
		id: button1785_1581
		x: 0
		y: 1015
		width: 240
		height: 68
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate3557() 
	{
	}
	ButtonSq {
		id: button3557_1582
		x: 1680
		y: 1015
		width: 240
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3557()
	}
	function onActivate5767() 
	{
	 // Calling Command 5 (Goto DesignObj) with CommandGroup_Parameters from 17860
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 5 3 \"5768\" ")
	}
	StyledText {
		x: 292
		y: 1015
		height: 72
		width: 1015
		textLabel: "Keyboard"
	}
	ButtonSq {
		id: button5767_1583
		x: 292
		y: 1015
		width: 1353
		height: 54
		buttonbackground: "skins/Basic/menu2/main%20menu%20room%20long.png"
		buttontext: "Keyboard"
		clickHandler.onClicked: onActivate5767()
	}
}