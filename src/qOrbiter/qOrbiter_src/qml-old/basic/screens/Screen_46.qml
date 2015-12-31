// UI1 Screen MakingLinPhoneBroadcast (46)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate2037() 
	{
	 // Calling Command 927 (Add Extensions To Call) with CommandGroup_Parameters from 17555
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 927 87 \"<%=61%>\" ")
	}
	StyledText {
		x: 505
		y: 450
		height: 213
		width: 698
		textLabel: "End Broadcast"
	}
	ButtonSq {
		id: button2037_544
		x: 505
		y: 450
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "End Broadcast"
		clickHandler.onClicked: onActivate2037()
	}
}