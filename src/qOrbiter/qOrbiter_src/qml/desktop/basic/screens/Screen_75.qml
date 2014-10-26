// UI1 Screen Pick Room For Device (75)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate4319() 
	{
	 // Calling Command 274 (Set Room For Device) with CommandGroup_Parameters from 16047
		manager.sendDceMessage(srouterip + " "+deviceid + " -103 1 274 2 \"<%=31%>\" 57 \"<%=2%>\" ")
	}
	ButtonSq {
		id: button4319_924
		x: 18
		y: 528
		width: 1624
		height: 541
		buttonbackground: "skins/Basic/4"
		buttontext: ""
		clickHandler.onClicked: onActivate4319()
	}
	function onActivate4324() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 16073
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16072
		setCurrentScreen("Screen_97.qml")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "New Room"
	}
	ButtonSq {
		id: button4324_925
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "New Room"
		clickHandler.onClicked: onActivate4324()
	}
}