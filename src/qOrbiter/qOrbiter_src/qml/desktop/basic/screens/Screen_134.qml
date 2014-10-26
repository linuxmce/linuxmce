// UI1 Screen SetupMythTv02 (134)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/menu2/transparent.png"
		width: 1920
		height: 1080
	}
	function onActivate3214() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 13778
		setCurrentScreen("Screen_1.qml")
	}
}