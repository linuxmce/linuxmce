// UI1 Screen NewPnpDevice (116)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate4484() 
	{
	 // Calling Command 700 (Choose Pnp Device Template) with CommandGroup_Parameters from 17001
		manager.sendDceMessage(srouterip + " "+deviceid + " -111 1 700 150 \"<%=2%>\" 224 \"<%=ID%>\" ")
	}
	ButtonSq {
		id: button4484_1337
		x: 45
		y: 508
		width: 1805
		height: 541
		buttonbackground: "skins/Basic/3"
		buttontext: ""
		clickHandler.onClicked: onActivate4484()
	}
	function onActivate4483() 
	{
	 // Calling Command 805 (Ignore PNP Device) with CommandGroup_Parameters from 17007
		manager.sendDceMessage(srouterip + " "+deviceid + " -111 1 805 224 \"<%=ID%>\" 225 \"0\" ")
	}
	StyledText {
		x: 9
		y: 237
		height: 213
		width: 698
		textLabel: "No, ignore it this time"
	}
	ButtonSq {
		id: button4483_1338
		x: 9
		y: 237
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "No, ignore it this time"
		clickHandler.onClicked: onActivate4483()
	}
	function onActivate5059() 
	{
	 // Calling Command 805 (Ignore PNP Device) with CommandGroup_Parameters from 17006
		manager.sendDceMessage(srouterip + " "+deviceid + " -111 1 805 224 \"<%=ID%>\" 225 \"1\" ")
	}
	StyledText {
		x: 965
		y: 237
		height: 213
		width: 698
		textLabel: "No, and ignore it every time.  Don't ask again for this device."
	}
	ButtonSq {
		id: button5059_1339
		x: 965
		y: 237
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "No, and ignore it every time.  Don't ask again for this device."
		clickHandler.onClicked: onActivate5059()
	}
}