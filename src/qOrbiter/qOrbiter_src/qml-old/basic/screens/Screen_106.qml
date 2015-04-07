// UI1 Screen WhatModelMobileOrbiter (106)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate4415() 
	{
	 // Calling Command 78 (New Orbiter) with CommandGroup_Parameters from 16162
		manager.sendDceMessage(srouterip + " "+deviceid + " -101 1 78 17 \"<%=13%>\" 44 \"<%=2%>\" 47 \"<%=12%>\" ")
	 // Calling Command 8 (Remove Screen From History) with CommandGroup_Parameters from 16711
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 8 10 \"<%=ID%>\" 159 \"106\" ")
	}
	function onActivate4416() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16161
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 0
		y: 0
		height: 240
		width: 180
		textLabel: "Cancel"
	}
	ButtonSq {
		id: button4416_1321
		x: 0
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/roundblack.png"
		buttontext: "Cancel"
		clickHandler.onClicked: onActivate4416()
	}
}