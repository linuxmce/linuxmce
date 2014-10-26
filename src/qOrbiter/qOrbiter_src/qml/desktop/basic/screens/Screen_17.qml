// UI1 Screen QuadViewCameras (17)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Security/4camerasBGB.png"
		width: 1920
		height: 1080
	}
	function onActivate1415() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14507
		setCurrentScreen("Screen_15.qml")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14544
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"1\" 5 \"<%=18%>\" ")
	}
	StyledText {
		x: 203
		y: 474
		height: 70
		width: 330
		textLabel: "Camera 1"
	}
	ButtonSq {
		id: button1415_205
		x: 203
		y: 474
		width: 440
		height: 53
		buttonbackground: "skins/Basic/Graphics/textlinesmall.png"
		buttontext: "Camera 1"
		clickHandler.onClicked: onActivate1415()
	}
	function onActivate1416() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14508
		setCurrentScreen("Screen_15.qml")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14546
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"1\" 5 \"<%=19%>\" ")
	}
	StyledText {
		x: 1083
		y: 474
		height: 70
		width: 330
		textLabel: "Camera 2"
	}
	ButtonSq {
		id: button1416_206
		x: 1083
		y: 474
		width: 440
		height: 53
		buttonbackground: "skins/Basic/Graphics/textlinesmall.png"
		buttontext: "Camera 2"
		clickHandler.onClicked: onActivate1416()
	}
	function onActivate1417() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14509
		setCurrentScreen("Screen_15.qml")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14548
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"1\" 5 \"<%=20%>\" ")
	}
	StyledText {
		x: 203
		y: 981
		height: 70
		width: 330
		textLabel: "Camera 3"
	}
	ButtonSq {
		id: button1417_207
		x: 203
		y: 981
		width: 440
		height: 53
		buttonbackground: "skins/Basic/Graphics/textlinesmall.png"
		buttontext: "Camera 3"
		clickHandler.onClicked: onActivate1417()
	}
	function onActivate1418() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14510
		setCurrentScreen("Screen_15.qml")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14550
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"1\" 5 \"<%=21%>\" ")
	}
	StyledText {
		x: 1083
		y: 981
		height: 70
		width: 330
		textLabel: "Camera 4"
	}
	ButtonSq {
		id: button1418_208
		x: 1083
		y: 981
		width: 440
		height: 53
		buttonbackground: "skins/Basic/Graphics/textlinesmall.png"
		buttontext: "Camera 4"
		clickHandler.onClicked: onActivate1418()
	}
	function onActivate1406() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2231
		setCurrentScreen("Screen_15.qml")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14538
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"1\" 5 \"<%=18%>\" ")
	}
	ButtonSq {
		id: button1406_209
		x: 90
		y: 54
		width: 686
		height: 379
		buttonbackground: "skins/Basic/<%=18%>"
		buttontext: ""
		clickHandler.onClicked: onActivate1406()
	}
	function onActivate1407() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2229
		setCurrentScreen("Screen_15.qml")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14536
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"1\" 5 \"<%=19%>\" ")
	}
	ButtonSq {
		id: button1407_210
		x: 902
		y: 54
		width: 686
		height: 379
		buttonbackground: "skins/Basic/<%=19%>"
		buttontext: ""
		clickHandler.onClicked: onActivate1407()
	}
	function onActivate1408() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2227
		setCurrentScreen("Screen_15.qml")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14540
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"1\" 5 \"<%=20%>\" ")
	}
	ButtonSq {
		id: button1408_211
		x: 90
		y: 568
		width: 686
		height: 379
		buttonbackground: "skins/Basic/<%=20%>"
		buttontext: ""
		clickHandler.onClicked: onActivate1408()
	}
	function onActivate1409() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2225
		setCurrentScreen("Screen_15.qml")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14542
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"1\" 5 \"<%=21%>\" ")
	}
	ButtonSq {
		id: button1409_212
		x: 902
		y: 568
		width: 686
		height: 379
		buttonbackground: "skins/Basic/<%=21%>"
		buttontext: ""
		clickHandler.onClicked: onActivate1409()
	}
	function onActivate1785() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17174
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 1680
		y: 0
		height: 1
		width: 1
		textLabel: "Main menu"
	}
	ButtonSq {
		id: button1785_213
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
}