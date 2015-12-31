// UI1 Screen NewPhoneDetected (67)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate5144() 
	{
	 // Calling Command 804 (Set Pnp Options) with CommandGroup_Parameters from 17218
		manager.sendDceMessage(srouterip + " "+deviceid + " -111 1 804 5 \"<%=2%>\" 52 \"3\" 224 \"<%=ID%>\" ")
	 // Calling Command 5 (Goto DesignObj) with CommandGroup_Parameters from 17219
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 5 3 \"4749\" ")
	}
	ButtonSq {
		id: button5144_844
		x: 54
		y: 609
		width: 1805
		height: 474
		buttonbackground: "skins/Basic/5"
		buttontext: ""
		clickHandler.onClicked: onActivate5144()
	}
}