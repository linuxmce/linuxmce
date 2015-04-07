// UI1 Screen Security (5)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/security.png"
		width: 1920
		height: 1080
	}
	ButtonSq {
		id: button1831_35
		x: 239
		y: 0
		width: 0
		height: 0
		buttonbackground: "skins/Basic/3"
		buttontext: ""
		clickHandler.onClicked: onActivate1831()
	}
	StyledText {
		x: 239
		y: 0
		height: 240
		width: 180
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button1259_36
		x: 239
		y: 0
		width: 240
		height: 177
		buttonbackground: "skins/Basic/Buttons/Security/squareb.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate1259()
	}
	ButtonSq {
		id: button3390_37
		x: 526
		y: 14
		width: 144
		height: 108
		buttonbackground: "skins/Basic/../../scenarios/<%=5:0%>.png|<%=IM%>"
		buttontext: ""
		clickHandler.onClicked: onActivate3390()
	}
	function onActivate1785() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17174
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 0
		y: 0
		height: 1
		width: 1
		textLabel: "Main menu"
	}
	ButtonSq {
		id: button1785_38
		x: 0
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
}