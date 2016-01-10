// UI1 Screen CurrentUser (24)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/pluto_users.png"
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
		id: button1785_302
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate1695() 
	{
	}
	ButtonSq {
		id: button1695_303
		x: 217
		y: 180
		width: 0
		height: 0
		buttonbackground: "skins/Basic/13"
		buttontext: ""
		clickHandler.onClicked: onActivate1695()
	}
	function onActivate1694() 
	{
	 // Calling Command 58 (Set Current User) with CommandGroup_Parameters from 13582
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 58 17 \"<%=5:0%>\" ")
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 14251
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	StyledText {
		x: 217
		y: 180
		height: 240
		width: 270
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button1694_304
		x: 217
		y: 180
		width: 360
		height: 180
		buttonbackground: "skins/Basic/Buttons/user_room.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate1694()
	}
	ButtonSq {
		id: button2030_305
		x: 550
		y: 374
		width: 144
		height: 108
		buttonbackground: "skins/Basic/../../users/<%=5:0%>.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2030()
	}
}