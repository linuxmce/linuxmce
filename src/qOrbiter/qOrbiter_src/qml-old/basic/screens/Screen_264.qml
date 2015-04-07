// UI1 Screen PNP Generic Options (264)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/wizard_noXine.png"
		width: 1920
		height: 1080
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_2709
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate3419() 
	{
	 // Calling Command 389 (Send Message) with CommandGroup_Parameters from 14828
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 389 9 \"<%=46%>\" 144 \"1\" ")
	}
	StyledText {
		x: 1444
		y: 863
		height: 211
		width: 346
		textLabel: "<%=0:1%>"
	}
	ButtonSq {
		id: button3419_2710
		x: 1444
		y: 863
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "<%=0:1%>"
		clickHandler.onClicked: onActivate3419()
	}
}