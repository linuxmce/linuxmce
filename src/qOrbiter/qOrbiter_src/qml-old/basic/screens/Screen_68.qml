// UI1 Screen FirstTime (68)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate3222() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 13933
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 490
		y: 900
		height: 213
		width: 698
		textLabel: "Go to main menu"
	}
	ButtonSq {
		id: button3222_845
		x: 490
		y: 900
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Go to main menu"
		clickHandler.onClicked: onActivate3222()
	}
}