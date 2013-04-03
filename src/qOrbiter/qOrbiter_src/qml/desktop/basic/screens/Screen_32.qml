// UI1 Screen ConfirmDelete (32)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate1910() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 11151
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	StyledText {
		x: 347
		y: 643
		height: 213
		width: 698
		textLabel: "Delete"
	}
	ButtonSq {
		id: button1910_351
		x: 347
		y: 643
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Delete"
		clickHandler.onClicked: onActivate1910()
	}
	function onActivate1911() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2736
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	StyledText {
		x: 347
		y: 829
		height: 213
		width: 698
		textLabel: "Cancel"
	}
	ButtonSq {
		id: button1911_352
		x: 347
		y: 829
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Cancel"
		clickHandler.onClicked: onActivate1911()
	}
}