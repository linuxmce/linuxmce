// UI1 Screen ViewAttributes (81)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/View_Attributes.png"
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
		id: button1785_970
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
		id: button1784_971
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate3424() 
	{
	}
	function onActivate5712() 
	{
	 // Calling Command 5 (Goto DesignObj) with CommandGroup_Parameters from 17824
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 5 3 \"5711\" ")
	}
	StyledText {
		x: 1680
		y: 900
		height: 240
		width: 180
		textLabel: "More"
	}
	ButtonSq {
		id: button5712_972
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "More"
		clickHandler.onClicked: onActivate5712()
	}
}