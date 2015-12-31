// UI1 Screen Choose Folder (240)
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
		id: button1785_2536
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
		id: button1784_2537
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	ButtonSq {
		id: button5148_2538
		x: 45
		y: 102
		width: 1588
		height: 720
		buttonbackground: "skins/Basic/7"
		buttontext: ""
		clickHandler.onClicked: onActivate5148()
	}
	function onActivate5149() 
	{
	}
	StyledText {
		x: 180
		y: 900
		height: 211
		width: 346
		textLabel: "Choose"
	}
	ButtonSq {
		id: button5149_2539
		x: 180
		y: 900
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Choose"
		clickHandler.onClicked: onActivate5149()
	}
	StyledText {
		x: 1038
		y: 900
		height: 211
		width: 346
		textLabel: "Create Directory"
	}
	ButtonSq {
		id: button5150_2540
		x: 1038
		y: 900
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Create Directory"
		clickHandler.onClicked: onActivate5150()
	}
}