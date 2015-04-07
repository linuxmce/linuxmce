// UI1 Screen CreateViewBookmarksTV (103)
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
		id: button1785_1312
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate4397() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17015
		setCurrentScreen("Screen_220.qml")
	}
	ButtonSq {
		id: button4397_1313
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4397()
	}
	function onActivate4398() 
	{
	 // Calling Command 409 (Save Bookmark) with CommandGroup_Parameters from 16123
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 409 163 \"<%=12%>\" 188 \"<%=31%>\" ")
	}
	StyledText {
		x: 0
		y: 68
		height: 240
		width: 180
		textLabel: "Public"
	}
	ButtonSq {
		id: button4398_1314
		x: 0
		y: 68
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Public"
		clickHandler.onClicked: onActivate4398()
	}
	function onActivate4399() 
	{
	 // Calling Command 409 (Save Bookmark) with CommandGroup_Parameters from 16124
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 409 163 \"<%=13%>\" 188 \"<%=33%>\" ")
	}
	StyledText {
		x: 720
		y: 68
		height: 240
		width: 180
		textLabel: "Public"
	}
	ButtonSq {
		id: button4399_1315
		x: 720
		y: 68
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Public"
		clickHandler.onClicked: onActivate4399()
	}
	function onActivate4365() 
	{
	 // Calling Command 412 (Set Media Position) with CommandGroup_Parameters from 16118
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 412 42 \"<%=2%>\" ")
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 16119
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"4400\" ")
	}
	function onActivate4364() 
	{
	 // Calling Command 412 (Set Media Position) with CommandGroup_Parameters from 16120
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 412 42 \"<%=2%>\" ")
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 16121
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"4400\" ")
	}
	function onActivate4400() 
	{
	 // Calling Command 410 (Delete Bookmark) with CommandGroup_Parameters from 16117
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 410 129 \"<%=2%>\" ")
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
		id: button4400_1316
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Delete
book
mark"
		clickHandler.onClicked: onActivate4400()
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "Thumbnail Channel"
	}
	ButtonSq {
		id: button5392_1317
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Thumbnail Channel"
		clickHandler.onClicked: onActivate5392()
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "Thumbnail"
	}
	ButtonSq {
		id: button5393_1318
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Thumbnail"
		clickHandler.onClicked: onActivate5393()
	}
	function onActivate5395() 
	{
	 // Calling Command 409 (Save Bookmark) with CommandGroup_Parameters from 17377
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 409 17 \"<%=U%>\" 163 \"<%=13%>\" 188 \"<%=33%>\" ")
	}
	StyledText {
		x: 960
		y: 68
		height: 211
		width: 346
		textLabel: "<%=UN%>"
	}
	ButtonSq {
		id: button5395_1319
		x: 960
		y: 68
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "<%=UN%>"
		clickHandler.onClicked: onActivate5395()
	}
	function onActivate5394() 
	{
	 // Calling Command 409 (Save Bookmark) with CommandGroup_Parameters from 17376
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 409 17 \"<%=U%>\" 163 \"<%=12%>\" 188 \"<%=31%>\" ")
	}
	StyledText {
		x: 240
		y: 68
		height: 211
		width: 346
		textLabel: "<%=UN%>"
	}
	ButtonSq {
		id: button5394_1320
		x: 240
		y: 68
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "<%=UN%>"
		clickHandler.onClicked: onActivate5394()
	}
}