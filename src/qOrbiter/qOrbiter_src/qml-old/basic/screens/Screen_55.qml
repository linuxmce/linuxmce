// UI1 Screen LinPhones (55)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
	}
	ButtonSq {
		id: button2357_681
		x: 90
		y: 68
		width: 1516
		height: 900
		buttonbackground: "skins/Basic/19"
		buttontext: ""
		clickHandler.onClicked: onActivate2357()
	}
	function onActivate2358() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 12092
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 21 \"1\" ")
	}
	StyledText {
		x: 90
		y: 68
		height: 236
		width: 160
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button2358_682
		x: 90
		y: 68
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate2358()
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
		id: button1785_683
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
		id: button1784_684
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
}