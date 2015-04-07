// UI1 Screen Static IP settings (270)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate5563() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17584
		setCurrentScreen("Screen_97.qml")
	}
	StyledText {
		x: 1083
		y: 609
		height: 72
		width: 275
		textLabel: "Change"
	}
	ButtonSq {
		id: button5563_2796
		x: 1083
		y: 609
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/main%20menu%20acts.png"
		buttontext: "Change"
		clickHandler.onClicked: onActivate5563()
	}
	function onActivate5572() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17600
		setCurrentScreen("Screen_97.qml")
	}
	StyledText {
		x: 1083
		y: 677
		height: 72
		width: 275
		textLabel: "Change"
	}
	ButtonSq {
		id: button5572_2797
		x: 1083
		y: 677
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/main%20menu%20acts.png"
		buttontext: "Change"
		clickHandler.onClicked: onActivate5572()
	}
	function onActivate5573() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17601
		setCurrentScreen("Screen_97.qml")
	}
	StyledText {
		x: 1083
		y: 744
		height: 72
		width: 275
		textLabel: "Change"
	}
	ButtonSq {
		id: button5573_2798
		x: 1083
		y: 744
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/main%20menu%20acts.png"
		buttontext: "Change"
		clickHandler.onClicked: onActivate5573()
	}
	function onActivate5574() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17602
		setCurrentScreen("Screen_97.qml")
	}
	StyledText {
		x: 1083
		y: 812
		height: 72
		width: 275
		textLabel: "Change"
	}
	ButtonSq {
		id: button5574_2799
		x: 1083
		y: 812
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/main%20menu%20acts.png"
		buttontext: "Change"
		clickHandler.onClicked: onActivate5574()
	}
	StyledText {
		x: 635
		y: 915
		height: 211
		width: 346
		textLabel: "Apply Network Settings"
	}
	ButtonSq {
		id: button5575_2800
		x: 635
		y: 915
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Apply Network Settings"
		clickHandler.onClicked: onActivate5575()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_2801
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
		id: button1785_2802
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
}