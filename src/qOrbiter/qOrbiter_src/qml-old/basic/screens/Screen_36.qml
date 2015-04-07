// UI1 Screen SensorsNotReady (36)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Security/sensorsB.png"
		width: 1920
		height: 1080
	}
	function onActivate1933() 
	{
	 // Calling Command 19 (Set House Mode) with CommandGroup_Parameters from 14801
		manager.sendDceMessage(srouterip + " "+deviceid + " -108 1 19 5 \"<%=25%>\" 17 \"<%=U%>\" 99 \"<%=24%>\" 100 \"<%=32%>\" 101 \"B\" ")
	}
	StyledText {
		x: 23
		y: 643
		height: 213
		width: 698
		textLabel: "Bypass These Sensors and Arm Now"
	}
	ButtonSq {
		id: button1933_411
		x: 23
		y: 643
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Bypass These Sensors and Arm Now"
		clickHandler.onClicked: onActivate1933()
	}
	function onActivate1934() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14802
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 970
		y: 846
		height: 213
		width: 698
		textLabel: "Cancel"
	}
	ButtonSq {
		id: button1934_412
		x: 970
		y: 846
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Cancel"
		clickHandler.onClicked: onActivate1934()
	}
	function onActivate3403() 
	{
	 // Calling Command 19 (Set House Mode) with CommandGroup_Parameters from 14804
		manager.sendDceMessage(srouterip + " "+deviceid + " -108 1 19 5 \"<%=25%>\" 17 \"<%=U%>\" 99 \"<%=24%>\" 100 \"<%=32%>\" 101 \"R\" ")
	}
	StyledText {
		x: 970
		y: 643
		height: 213
		width: 698
		textLabel: "Try Again"
	}
	ButtonSq {
		id: button3403_413
		x: 970
		y: 643
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Try Again"
		clickHandler.onClicked: onActivate3403()
	}
	function onActivate3402() 
	{
	 // Calling Command 19 (Set House Mode) with CommandGroup_Parameters from 14803
		manager.sendDceMessage(srouterip + " "+deviceid + " -108 1 19 5 \"<%=25%>\" 17 \"<%=U%>\" 99 \"<%=24%>\" 100 \"<%=32%>\" 101 \"W\" ")
	}
	StyledText {
		x: 23
		y: 846
		height: 213
		width: 698
		textLabel: "Arm other sensors now, arm these when they clear"
	}
	ButtonSq {
		id: button3402_414
		x: 23
		y: 846
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Arm other sensors now, arm these when they clear"
		clickHandler.onClicked: onActivate3402()
	}
}