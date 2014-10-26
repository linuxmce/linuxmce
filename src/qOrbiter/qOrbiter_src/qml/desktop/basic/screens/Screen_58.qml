// UI1 Screen DVDOptions (58)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
	}
	function onActivate2050() 
	{
	 // Calling Command 141 (Subtitle) with CommandGroup_Parameters from 13649
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 141 5 \"<%=12%>\" ")
	}
	function onActivate2048() 
	{
	 // Calling Command 140 (Audio Track) with CommandGroup_Parameters from 13650
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 140 5 \"<%=13%>\" ")
	}
	function onActivate2378() 
	{
	 // Calling Command 142 (Angle) with CommandGroup_Parameters from 13669
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 142 5 \"<%=31%>\" ")
	}
	function onActivate2377() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17009
		setCurrentScreen("Screen_220.qml")
	}
	StyledText {
		x: 1680
		y: 0
		height: 240
		width: 180
		textLabel: "DVD Remote"
	}
	ButtonSq {
		id: button2377_693
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "DVD Remote"
		clickHandler.onClicked: onActivate2377()
	}
}