// UI1 Screen TransferAllUsers (28)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
	}
	function onActivate1287() 
	{
	}
	ButtonSq {
		id: button1287_335
		x: 120
		y: 90
		width: 1442
		height: 900
		buttonbackground: "skins/Basic/13"
		buttontext: ""
		clickHandler.onClicked: onActivate1287()
	}
	function onActivate1286() 
	{
	 // Calling Command 923 (Assisted Make Call) with CommandGroup_Parameters from 17526
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 923 17 \"<%=U%>\" ")
	}
	StyledText {
		x: 120
		y: 90
		height: 240
		width: 180
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button1286_336
		x: 120
		y: 90
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate1286()
	}
	ButtonSq {
		id: button2030_337
		x: 272
		y: 194
		width: 144
		height: 108
		buttonbackground: "skins/Basic/../../users/<%=5:0%>.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2030()
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
		id: button1785_338
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
		id: button1784_339
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
}