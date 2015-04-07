// UI1 Screen Sensors Viewed By Camera (206)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate4785() 
	{
	}
	StyledText {
		x: 1680
		y: 900
		height: 240
		width: 180
		textLabel: "Ok"
	}
	ButtonSq {
		id: button4785_2347
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Ok"
		clickHandler.onClicked: onActivate4785()
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
		id: button1785_2348
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	ButtonSq {
		id: button4784_2349
		x: 18
		y: 338
		width: 1534
		height: 609
		buttonbackground: "skins/Basic/3"
		buttontext: ""
		clickHandler.onClicked: onActivate4784()
	}
}