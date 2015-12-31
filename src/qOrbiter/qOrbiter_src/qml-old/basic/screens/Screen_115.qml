// UI1 Screen Angles (115)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/mobile/menu/empty.png"
		width: 1920
		height: 1080
	}
	function onActivate2378() 
	{
	 // Calling Command 142 (Angle) with CommandGroup_Parameters from 13669
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 142 5 \"<%=31%>\" ")
	}
}