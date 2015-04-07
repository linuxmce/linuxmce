// UI1 Screen Misc (7)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/miscellaneous.png"
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
		id: button1785_43
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	ButtonSq {
		id: button1829_44
		x: 239
		y: 180
		width: 0
		height: 0
		buttonbackground: "skins/Basic/16"
		buttontext: ""
		clickHandler.onClicked: onActivate1829()
	}
	StyledText {
		x: 239
		y: 180
		height: 240
		width: 180
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button1262_45
		x: 239
		y: 180
		width: 240
		height: 177
		buttonbackground: "skins/Basic/Buttons/Other/squareb.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate1262()
	}
	ButtonSq {
		id: button1804_46
		x: 492
		y: 360
		width: 0
		height: 0
		buttonbackground: "skins/Basic/../../rooms/<%=RIL%>.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1804()
	}
	function onActivate3442() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14846
		setCurrentScreen("Screen_44.qml")
	}
	StyledText {
		x: 239
		y: 0
		height: 240
		width: 180
		textLabel: "Advanced Options"
	}
	ButtonSq {
		id: button3442_47
		x: 239
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Other/squareb.png"
		buttontext: "Advanced Options"
		clickHandler.onClicked: onActivate3442()
	}
	function onActivate3443() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14847
		setCurrentScreen("Screen_39.qml")
	}
	StyledText {
		x: 479
		y: 0
		height: 240
		width: 180
		textLabel: "Computing"
	}
	ButtonSq {
		id: button3443_48
		x: 479
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Other/squareb.png"
		buttontext: "Computing"
		clickHandler.onClicked: onActivate3443()
	}
	function onActivate3444() 
	{
	 // Calling Command 260 (Set Main Menu) with CommandGroup_Parameters from 14848
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 260 9 \"S\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14849
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 719
		y: 0
		height: 240
		width: 180
		textLabel: "Sleeping menu"
	}
	ButtonSq {
		id: button3444_49
		x: 719
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Other/squareb.png"
		buttontext: "Sleeping menu"
		clickHandler.onClicked: onActivate3444()
	}
	function onActivate3445() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14850
		setCurrentScreen("Screen_76.qml")
	}
	StyledText {
		x: 959
		y: 0
		height: 240
		width: 180
		textLabel: "Power"
	}
	ButtonSq {
		id: button3445_50
		x: 959
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Other/squareb.png"
		buttontext: "Power"
		clickHandler.onClicked: onActivate3445()
	}
}