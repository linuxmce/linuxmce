// UI1 Screen Rip format options (286)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	StyledText {
		x: 720
		y: 540
		height: 240
		width: 180
		textLabel: "MP3"
	}
	ButtonSq {
		id: button5642_3007
		x: 720
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "MP3"
		clickHandler.onClicked: onActivate5642()
	}
	StyledText {
		x: 960
		y: 540
		height: 240
		width: 180
		textLabel: "FLAC"
	}
	ButtonSq {
		id: button5643_3008
		x: 960
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "FLAC"
		clickHandler.onClicked: onActivate5643()
	}
	StyledText {
		x: 720
		y: 720
		height: 240
		width: 180
		textLabel: "OGG"
	}
	ButtonSq {
		id: button5644_3009
		x: 720
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "OGG"
		clickHandler.onClicked: onActivate5644()
	}
	StyledText {
		x: 960
		y: 720
		height: 240
		width: 180
		textLabel: "WAV"
	}
	ButtonSq {
		id: button5645_3010
		x: 960
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "WAV"
		clickHandler.onClicked: onActivate5645()
	}
}