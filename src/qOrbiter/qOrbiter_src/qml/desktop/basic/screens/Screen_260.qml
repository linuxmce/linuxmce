// UI1 Screen Jukebox Manager (260)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
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
		id: button1785_2690
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	StyledText {
		x: 1682
		y: 360
		height: 120
		width: 180
		textLabel: "Cancel Ripping"
	}
	ButtonSq {
		id: button5453_2691
		x: 1682
		y: 360
		width: 240
		height: 90
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Cancel Ripping"
		clickHandler.onClicked: onActivate5453()
	}
	function onActivate5454() 
	{
	}
	ButtonSq {
		id: button5454_2692
		x: 1772
		y: 677
		width: 90
		height: 68
		buttonbackground: "skins/Basic/Icons/media/rip.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5454()
	}
	ButtonSq {
		id: button5455_2693
		x: 1682
		y: 677
		width: 90
		height: 68
		buttonbackground: "skins/Basic/Icons/media/eject.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5455()
	}
	StyledText {
		x: 1682
		y: 968
		height: 120
		width: 90
		textLabel: "One"
	}
	ButtonSq {
		id: button5456_2694
		x: 1682
		y: 968
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "One"
		clickHandler.onClicked: onActivate5456()
	}
	StyledText {
		x: 1802
		y: 968
		height: 120
		width: 90
		textLabel: "Many"
	}
	ButtonSq {
		id: button5457_2695
		x: 1802
		y: 968
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Many"
		clickHandler.onClicked: onActivate5457()
	}
	ButtonSq {
		id: button5460_2696
		x: 1682
		y: 744
		width: 90
		height: 68
		buttonbackground: "skins/Basic/Icons/media/id.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5460()
	}
	StyledText {
		x: 1682
		y: 450
		height: 120
		width: 180
		textLabel: "Refresh"
	}
	ButtonSq {
		id: button5461_2697
		x: 1682
		y: 450
		width: 240
		height: 90
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Refresh"
		clickHandler.onClicked: onActivate5461()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_2698
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
}