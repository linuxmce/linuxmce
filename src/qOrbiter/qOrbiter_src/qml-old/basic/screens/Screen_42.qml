// UI1 Screen FavoritesPDA (42)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Misc/favoritesPDA.png"
		width: 1920
		height: 1080
	}
	StyledText {
		x: 776
		y: 812
		height: 240
		width: 180
		textLabel: "Go"
	}
	ButtonSq {
		id: button2012_502
		x: 776
		y: 812
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Go"
		clickHandler.onClicked: onActivate2012()
	}
}