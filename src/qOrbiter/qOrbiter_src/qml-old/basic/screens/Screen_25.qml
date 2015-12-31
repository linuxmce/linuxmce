// UI1 Screen CDTrackCopy (25)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate1722() 
	{
	 // Calling Command 337 (Rip Disk) with CommandGroup_Parameters from 14811
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 337 13 \"<%=17%>\" 17 \"<%=12%>\" 20 \"<%=13%>\" 50 \"<%=17%>\" 121 \"A\" 131 \"<%=31%>\" 151 \"<%=33%>\" 233 \"<%=45%>\" 234 \"<%=9%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14812
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 1263
		y: 41
		height: 240
		width: 180
		textLabel: "All Tracks"
	}
	ButtonSq {
		id: button1722_306
		x: 1263
		y: 41
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/roundblack.png"
		buttontext: "All Tracks"
		clickHandler.onClicked: onActivate1722()
	}
	function onActivate1723() 
	{
	 // Calling Command 337 (Rip Disk) with CommandGroup_Parameters from 14813
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 337 13 \"<%=17%>\" 17 \"<%=12%>\" 20 \"<%=13%>\" 50 \"<%=17%>\" 121 \"<%=2%>\" 131 \"<%=31%>\" 151 \"<%=33%>\" 233 \"<%=45%>\" 234 \"<%=9%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14814
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 1011
		y: 41
		height: 240
		width: 180
		textLabel: "Next"
	}
	ButtonSq {
		id: button1723_307
		x: 1011
		y: 41
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/roundblack.png"
		buttontext: "Next"
		clickHandler.onClicked: onActivate1723()
	}
	function onActivate2418() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 14816
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	StyledText {
		x: 1680
		y: 41
		height: 240
		width: 180
		textLabel: "Cancel"
	}
	ButtonSq {
		id: button2418_308
		x: 1680
		y: 41
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/roundblack.png"
		buttontext: "Cancel"
		clickHandler.onClicked: onActivate2418()
	}
}