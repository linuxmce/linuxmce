// UI1 Screen Intercom (16)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Security/singlecameraB.png"
		width: 1920
		height: 1080
	}
	function onActivate1410() 
	{
	}
	ButtonSq {
		id: button1410_200
		x: 90
		y: 68
		width: 1507
		height: 744
		buttonbackground: "skins/Basic/<%=59:1%>"
		buttontext: ""
		clickHandler.onClicked: onActivate1410()
	}
	function onActivate1413() 
	{
	}
	StyledText {
		x: 722
		y: 893
		height: 240
		width: 180
		textLabel: "Ask to Leave"
	}
	ButtonSq {
		id: button1413_201
		x: 722
		y: 893
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/roundblack.png"
		buttontext: "Ask to Leave"
		clickHandler.onClicked: onActivate1413()
	}
	function onActivate1285() 
	{
	 // Calling Command 234 (PL_Transfer) with CommandGroup_Parameters from 16547
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 234 83 \"100\" 265 \"<%=64%>\" ")
	}
	StyledText {
		x: 63
		y: 893
		height: 240
		width: 180
		textLabel: "Send to Voice Mail"
	}
	ButtonSq {
		id: button1285_202
		x: 63
		y: 893
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Send to Voice Mail"
		clickHandler.onClicked: onActivate1285()
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
		id: button1785_203
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_204
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
}