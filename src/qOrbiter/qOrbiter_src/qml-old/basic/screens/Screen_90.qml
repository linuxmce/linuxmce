// UI1 Screen BookmarksByMediaType (90)
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
		id: button1785_1109
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
		id: button1784_1110
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate3550() 
	{
	 // Calling Command 410 (Delete Bookmark) with CommandGroup_Parameters from 15052
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 410 129 \"<%=2%>\" ")
	 // Calling Command 14 (Refresh) with CommandGroup_Parameters from 16727
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 14 15 \"*\" ")
	}
	StyledText {
		x: 1680
		y: 900
		height: 240
		width: 180
		textLabel: "Delete
book
mark"
	}
	ButtonSq {
		id: button3550_1111
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Delete
book
mark"
		clickHandler.onClicked: onActivate3550()
	}
	function onActivate3561() 
	{
	 // Calling Command 9 (Scroll Grid) with CommandGroup_Parameters from 15119
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 9 30 \"1\" ")
	}
	ButtonSq {
		id: button3561_1112
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3561()
	}
	function onActivate3562() 
	{
	 // Calling Command 9 (Scroll Grid) with CommandGroup_Parameters from 15120
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 9 30 \"2\" ")
	}
	ButtonSq {
		id: button3562_1113
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3562()
	}
	function onActivate3571() 
	{
	 // Calling Command 43 (MH Play Media) with CommandGroup_Parameters from 15171
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 43 13 \"!B<%=2%>\" ")
	}
	ButtonSq {
		id: button3571_1114
		x: 45
		y: 34
		width: 1579
		height: 1015
		buttonbackground: "skins/Basic/8"
		buttontext: ""
		clickHandler.onClicked: onActivate3571()
	}
}