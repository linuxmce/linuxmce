// UI1 Screen Demo Media (209)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate1597() 
	{
	}
	ButtonSq {
		id: button1597_2388
		x: 0
		y: 491
		width: 1660
		height: 541
		buttonbackground: "skins/Basic/7"
		buttontext: ""
		clickHandler.onClicked: onActivate1597()
	}
	StyledText {
		x: 1680
		y: 271
		height: 240
		width: 180
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button3412_2389
		x: 1680
		y: 271
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Lights/squareb.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate3412()
	}
	ButtonSq {
		id: button3390_2390
		x: 3408
		y: 555
		width: 144
		height: 108
		buttonbackground: "skins/Basic/../../scenarios/<%=5:0%>.png|<%=IM%>"
		buttontext: ""
		clickHandler.onClicked: onActivate3390()
	}
	function onActivate4831() 
	{
	 // Calling Command 231 (Load Playlist) with CommandGroup_Parameters from 16788
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 231 45 \"<%=E%>\" 78 \"<%=2%>\" ")
	}
	ButtonSq {
		id: button4831_2391
		x: 240
		y: 271
		width: 1440
		height: 180
		buttonbackground: "skins/Basic/4"
		buttontext: ""
		clickHandler.onClicked: onActivate4831()
	}
	function onActivate2454() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16961
		setCurrentScreen("Screen_220.qml")
	}
	StyledText {
		x: 9
		y: 277
		height: 222
		width: 166
		textLabel: "<%=NP%>"
	}
	ButtonSq {
		id: button2454_2392
		x: 0
		y: 271
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: "<%=NP%>"
		clickHandler.onClicked: onActivate2454()
	}
	function onActivate3442() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14846
		setCurrentScreen("Screen_44.qml")
	}
	StyledText {
		x: 1440
		y: 1029
		height: 68
		width: 180
		textLabel: "Advanced Options"
	}
	ButtonSq {
		id: button3442_2393
		x: 1440
		y: 1029
		width: 240
		height: 51
		buttonbackground: "skins/Basic/Buttons/Other/squareb.png"
		buttontext: "Advanced Options"
		clickHandler.onClicked: onActivate3442()
	}
	function onActivate4843() 
	{
	 // Calling Command 231 (Load Playlist) with CommandGroup_Parameters from 16798
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 231 45 \"<%=E%>\" 78 \"1\" ")
	}
	function onActivate4846() 
	{
	 // Calling Command 231 (Load Playlist) with CommandGroup_Parameters from 16799
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 231 45 \"<%=E%>\" 78 \"4\" ")
	}
	function onActivate4844() 
	{
	 // Calling Command 231 (Load Playlist) with CommandGroup_Parameters from 16800
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 231 45 \"<%=E%>\" 78 \"2\" ")
	}
	function onActivate4845() 
	{
	 // Calling Command 231 (Load Playlist) with CommandGroup_Parameters from 16801
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 231 45 \"<%=E%>\" 78 \"3\" ")
	}
}