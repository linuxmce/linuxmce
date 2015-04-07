// UI1 Screen CreateViewBookmarks (87)
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
		id: button1785_1034
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
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
		id: button3550_1035
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
	function onActivate3551() 
	{
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 15124
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"3550\" ")
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 17557
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"5543\" ")
	}
	ButtonSq {
		id: button3551_1036
		x: 45
		y: 68
		width: 1588
		height: 785
		buttonbackground: "skins/Basic/3"
		buttontext: ""
		clickHandler.onClicked: onActivate3551()
	}
	function onActivate3549() 
	{
	 // Calling Command 409 (Save Bookmark) with CommandGroup_Parameters from 15051
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 409 45 \"<%=E%>\" ")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 15121
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" ")
	 // Calling Command 14 (Refresh) with CommandGroup_Parameters from 17379
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 14 15 \"*\" ")
	}
	StyledText {
		x: 45
		y: 880
		height: 211
		width: 346
		textLabel: "Bookmark this position"
	}
	ButtonSq {
		id: button3549_1037
		x: 45
		y: 880
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Bookmark this position"
		clickHandler.onClicked: onActivate3549()
	}
	function onActivate3561() 
	{
	 // Calling Command 9 (Scroll Grid) with CommandGroup_Parameters from 15119
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 9 30 \"1\" ")
	}
	ButtonSq {
		id: button3561_1038
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
		id: button3562_1039
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3562()
	}
	function onActivate4397() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17015
		setCurrentScreen("Screen_220.qml")
	}
	ButtonSq {
		id: button4397_1040
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4397()
	}
	function onActivate5167() 
	{
	 // Calling Command 409 (Save Bookmark) with CommandGroup_Parameters from 17144
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 409 45 \"<%=E%>\" 225 \"1\" ")
	 // Calling Command 14 (Refresh) with CommandGroup_Parameters from 17147
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 14 15 \"*\" ")
	}
	StyledText {
		x: 600
		y: 880
		height: 211
		width: 346
		textLabel: "Set as Start Point"
	}
	ButtonSq {
		id: button5167_1041
		x: 600
		y: 880
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Set as Start Point"
		clickHandler.onClicked: onActivate5167()
	}
	function onActivate5543() 
	{
	 // Calling Command 412 (Set Media Position) with CommandGroup_Parameters from 17556
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 412 42 \"<%=2%>\" ")
	}
	StyledText {
		x: 1173
		y: 880
		height: 211
		width: 346
		textLabel: "Play from this position"
	}
	ButtonSq {
		id: button5543_1042
		x: 1173
		y: 880
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Play from this position"
		clickHandler.onClicked: onActivate5543()
	}
}