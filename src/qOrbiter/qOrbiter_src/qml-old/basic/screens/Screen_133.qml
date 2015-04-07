// UI1 Screen SetupMythTv01 (133)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/black.png"
		width: 1920
		height: 1080
	}
	function onActivate3216() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 13770
		setCurrentScreen("Screen_134.qml")
	}
	StyledText {
		x: 794
		y: 677
		height: 180
		width: 271
		textLabel: "Next"
	}
	ButtonSq {
		id: button3216_1512
		x: 794
		y: 677
		width: 361
		height: 135
		buttonbackground: "skins/Basic/200"
		buttontext: "Next"
		clickHandler.onClicked: onActivate3216()
	}
}