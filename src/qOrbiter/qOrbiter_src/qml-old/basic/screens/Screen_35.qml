// UI1 Screen ModeChanged (35)
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
		id: button1809_410
		x: 1083
		y: 34
		width: 271
		height: 203
		buttonbackground: "skins/Basic/Icons/modes/housedisarmed.png;Icons/modes/housesecure.png;Icons/modes/athomesecure.png;Icons/modes/housesleeping.png;Icons/modes/entertain.png;Icons/modes/extendedaway.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1809()
	}
}