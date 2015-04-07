// UI1 Screen Remote Assistance (255)
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
		id: button1785_2677
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	StyledText {
		x: 226
		y: 915
		height: 211
		width: 346
		textLabel: "Enable Remote Assistance"
	}
	ButtonSq {
		id: button5387_2678
		x: 226
		y: 915
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Enable Remote Assistance"
		clickHandler.onClicked: onActivate5387()
	}
	StyledText {
		x: 924
		y: 915
		height: 211
		width: 346
		textLabel: "Disable Remote Assistance"
	}
	ButtonSq {
		id: button5389_2679
		x: 924
		y: 915
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Disable Remote Assistance"
		clickHandler.onClicked: onActivate5389()
	}
}