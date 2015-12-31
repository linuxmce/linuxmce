// UI1 Screen GenericDvdMenu (80)
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
		id: button1785_966
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
		id: button1784_967
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate3389() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14560
		setCurrentScreen("Screen_77.qml")
	}
	StyledText {
		x: 1680
		y: 360
		height: 240
		width: 180
		textLabel: "DVD Remote"
	}
	ButtonSq {
		id: button3389_968
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_1.png"
		buttontext: "DVD Remote"
		clickHandler.onClicked: onActivate3389()
	}
	function onActivate1664() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 12512
		setCurrentScreen("Screen_1.qml")
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13956
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 ")
	}
	ButtonSq {
		id: button1664_969
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1664()
	}
	function onActivate3363() 
	{
	 // Calling Command 200 (Move Up) with CommandGroup_Parameters from 14411
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 200 ")
	}
	function onActivate3364() 
	{
	 // Calling Command 201 (Move Down) with CommandGroup_Parameters from 14412
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 201 ")
	}
	function onActivate3365() 
	{
	 // Calling Command 202 (Move Left) with CommandGroup_Parameters from 14413
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 202 ")
	}
	function onActivate3366() 
	{
	 // Calling Command 203 (Move Right) with CommandGroup_Parameters from 14414
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 203 ")
	}
	function onActivate3367() 
	{
	 // Calling Command 190 (Enter/Go) with CommandGroup_Parameters from 14415
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 190 ")
	}
}