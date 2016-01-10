// UI1 Screen Privacy options for rip (287)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	ButtonSq {
		id: button5646_3011
		x: 120
		y: 270
		width: 1444
		height: 724
		buttonbackground: "skins/Basic/13"
		buttontext: ""
		clickHandler.onClicked: onActivate5646()
	}
	StyledText {
		x: 120
		y: 270
		height: 240
		width: 270
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button5647_3012
		x: 120
		y: 270
		width: 360
		height: 180
		buttonbackground: "skins/Basic/Buttons/user_room.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate5647()
	}
	function onActivate2334() 
	{
	}
	StyledText {
		x: 1584
		y: 268
		height: 240
		width: 180
		textLabel: "Save To Public"
	}
	ButtonSq {
		id: button2334_3013
		x: 1584
		y: 268
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Save To Public"
		clickHandler.onClicked: onActivate2334()
	}
}