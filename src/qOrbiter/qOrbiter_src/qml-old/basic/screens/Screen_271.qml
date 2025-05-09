// UI1 Screen Network Settings (271)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	StyledText {
		x: 321
		y: 828
		height: 211
		width: 346
		textLabel: "Use DHCP"
	}
	ButtonSq {
		id: button5561_2803
		x: 321
		y: 828
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Use DHCP"
		clickHandler.onClicked: onActivate5561()
	}
	function onActivate5562() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17575
		setCurrentScreen("Screen_270.qml")
	}
	StyledText {
		x: 1101
		y: 828
		height: 211
		width: 346
		textLabel: "Static IP"
	}
	ButtonSq {
		id: button5562_2804
		x: 1101
		y: 828
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Static IP"
		clickHandler.onClicked: onActivate5562()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_2805
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
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
		id: button1785_2806
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
}