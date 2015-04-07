// UI1 Screen AdjustScreenSettings (263)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/adjust%20rezolution.png"
		width: 1920
		height: 1080
	}
	function onActivate5468() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 17397
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"2\" 50 \"down\" ")
	}
	function onActivate5467() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 17396
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"1\" 50 \"up\" ")
	}
	function onActivate5465() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 17394
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"3\" 50 \"left\" ")
	}
	function onActivate5466() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 17395
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"4\" 50 \"right\" ")
	}
	function onActivate5464() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 17393
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"182\" 50 \"minus\" ")
	}
	function onActivate5463() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 17392
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"183\" 50 \"plus\" ")
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
		id: button1785_2706
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
		id: button1784_2707
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	StyledText {
		x: 1236
		y: 755
		height: 211
		width: 346
		textLabel: "Save settings and Regen Orbiter"
	}
	ButtonSq {
		id: button5501_2708
		x: 1236
		y: 755
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Save settings and Regen Orbiter"
		clickHandler.onClicked: onActivate5501()
	}
}