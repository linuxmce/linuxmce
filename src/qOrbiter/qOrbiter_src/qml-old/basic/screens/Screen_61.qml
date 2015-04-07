// UI1 Screen PreviewOptions (61)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
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
		id: button1785_712
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
		id: button1784_713
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate2401() 
	{
	}
	StyledText {
		x: 9
		y: 7
		height: 211
		width: 346
		textLabel: "Show channel history"
	}
	ButtonSq {
		id: button2401_714
		x: 9
		y: 7
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Show channel history"
		clickHandler.onClicked: onActivate2401()
	}
	function onActivate2404() 
	{
	}
	StyledText {
		x: 9
		y: 169
		height: 211
		width: 346
		textLabel: "Lock on this channel"
	}
	ButtonSq {
		id: button2404_715
		x: 9
		y: 169
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Lock on this channel"
		clickHandler.onClicked: onActivate2404()
	}
	ButtonSq {
		id: button2406_716
		x: 0
		y: 508
		width: 1200
		height: 575
		buttonbackground: "skins/Basic/7"
		buttontext: ""
		clickHandler.onClicked: onActivate2406()
	}
	function onActivate2405() 
	{
	}
	StyledText {
		x: 0
		y: 508
		height: 236
		width: 160
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button2405_717
		x: 0
		y: 508
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate2405()
	}
	function onActivate2407() 
	{
	}
	StyledText {
		x: 9
		y: 332
		height: 211
		width: 346
		textLabel: "Show Nothing"
	}
	ButtonSq {
		id: button2407_718
		x: 9
		y: 332
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Show Nothing"
		clickHandler.onClicked: onActivate2407()
	}
}