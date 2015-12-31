// UI1 Screen Computing (39)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
	}
	ButtonSq {
		id: button3315_434
		x: 18
		y: 54
		width: 794
		height: 1015
		buttonbackground: "skins/Basic/4"
		buttontext: ""
		clickHandler.onClicked: onActivate3315()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_435
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
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
		id: button1785_436
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	ButtonSq {
		id: button4320_437
		x: 821
		y: 54
		width: 848
		height: 1015
		buttonbackground: "skins/Basic/4"
		buttontext: ""
		clickHandler.onClicked: onActivate4320()
	}
	function onActivate4322() 
	{
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "Remote control"
	}
	ButtonSq {
		id: button4322_438
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Remote control"
		clickHandler.onClicked: onActivate4322()
	}
	function onActivate3557() 
	{
	}
	ButtonSq {
		id: button3557_439
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3557()
	}
}