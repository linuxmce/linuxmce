// UI1 Screen Need Reload Router (204)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate4764() 
	{
	 // Calling Command 272 (Restart DCERouter) with CommandGroup_Parameters from 16657
		manager.sendDceMessage(srouterip + " "+deviceid + " -103 1 272 ")
	}
	StyledText {
		x: 722
		y: 406
		height: 211
		width: 346
		textLabel: "Yes, do it now"
	}
	ButtonSq {
		id: button4764_2341
		x: 722
		y: 406
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Yes, do it now"
		clickHandler.onClicked: onActivate4764()
	}
	function onActivate4765() 
	{
	 // Calling Command 8 (Remove Screen From History) with CommandGroup_Parameters from 17228
		manager.sendDceMessage(srouterip + " "+deviceid + " -305 1 8 159 \"204\" ")
	}
	StyledText {
		x: 722
		y: 677
		height: 211
		width: 346
		textLabel: "No, do it later"
	}
	ButtonSq {
		id: button4765_2342
		x: 722
		y: 677
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "No, do it later"
		clickHandler.onClicked: onActivate4765()
	}
}