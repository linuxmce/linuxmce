// UI1 Screen ShowInfo (102)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
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
		id: button1785_1305
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	StyledText {
		x: 9
		y: 541
		height: 213
		width: 698
		textLabel: "Record only this episode without turning on the TV."
	}
	ButtonSq {
		id: button4384_1306
		x: 9
		y: 541
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Record only this episode without turning on the TV."
		clickHandler.onClicked: onActivate4384()
	}
	StyledText {
		x: 956
		y: 541
		height: 213
		width: 698
		textLabel: "Record this show every time it airs without turning on this TV."
	}
	ButtonSq {
		id: button4385_1307
		x: 956
		y: 541
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Record this show every time it airs without turning on this TV."
		clickHandler.onClicked: onActivate4385()
	}
	StyledText {
		x: 9
		y: 717
		height: 213
		width: 698
		textLabel: "Record this show only when new episodes air.  Do not record re-runs."
	}
	ButtonSq {
		id: button4386_1308
		x: 9
		y: 717
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Record this show only when new episodes air.  Do not record re-runs."
		clickHandler.onClicked: onActivate4386()
	}
	StyledText {
		x: 956
		y: 717
		height: 213
		width: 698
		textLabel: "When this show airs, turn this TV on, tune to this show & record it."
	}
	ButtonSq {
		id: button4387_1309
		x: 956
		y: 717
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "When this show airs, turn this TV on, tune to this show & record it."
		clickHandler.onClicked: onActivate4387()
	}
	StyledText {
		x: 9
		y: 907
		height: 213
		width: 698
		textLabel: "Find Other Broadcasts of this Show"
	}
	ButtonSq {
		id: button4388_1310
		x: 9
		y: 907
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Find Other Broadcasts of this Show"
		clickHandler.onClicked: onActivate4388()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_1311
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
}