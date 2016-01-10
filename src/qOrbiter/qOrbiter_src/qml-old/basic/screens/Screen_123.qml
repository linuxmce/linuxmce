// UI1 Screen MapSecurity (123)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/map.png"
		width: 1920
		height: 1080
	}
	function onActivate1892() 
	{
	}
	ButtonSq {
		id: button1892_1448
		x: 1440
		y: 180
		width: 480
		height: 0
		buttonbackground: "skins/Basic/15"
		buttontext: ""
		clickHandler.onClicked: onActivate1892()
	}
	function onActivate1891() 
	{
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 14123
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"1881.0.<%=12:1%>.2197\" 8 \"0\" ")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14124
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"12\" 5 \"<%=5%>\" ")
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 14125
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"1881.0.<%=12%>.2197\" 8 \"1\" ")
	}
	StyledText {
		x: 1440
		y: 180
		height: 240
		width: 162
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button1891_1449
		x: 1440
		y: 180
		width: 217
		height: 180
		buttonbackground: "skins/Basic/Buttons/Security/squareb.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate1891()
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
		id: button1785_1450
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate2197() 
	{
	}
	function onActivate3305() 
	{
	 // Calling Command 261 (Set Follow-Me) with CommandGroup_Parameters from 14361
		manager.sendDceMessage(srouterip + " "+deviceid + " -101 1 261 2 \"<%=!%>\" 9 \"S<%=GD1881.0.0.2607%>\" 17 \"<%=U%>\" ")
	}
	ButtonSq {
		id: button3305_1451
		x: 1440
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Security/squareb.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3305()
	}
	ButtonSq {
		id: button2607_1452
		x: 1458
		y: 3
		width: 198
		height: 171
		buttonbackground: "skins/Basic/mobile/icons/dontfolllowme.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2607()
	}
	function onActivate3433() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16057
		setCurrentScreen("Screen_15.qml")
	}
	StyledText {
		x: 1308
		y: 20
		height: 136
		width: 93
		textLabel: "view"
	}
	ButtonSq {
		id: button3433_1453
		x: 1308
		y: 20
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "view"
		clickHandler.onClicked: onActivate3433()
	}
	StyledText {
		x: 1308
		y: 20
		height: 136
		width: 93
		textLabel: "speak"
	}
	ButtonSq {
		id: button3434_1454
		x: 1308
		y: 20
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "speak"
		clickHandler.onClicked: onActivate3434()
	}
	StyledText {
		x: 1308
		y: 122
		height: 136
		width: 93
		textLabel: "open"
	}
	ButtonSq {
		id: button3435_1455
		x: 1308
		y: 122
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "open"
		clickHandler.onClicked: onActivate3435()
	}
	StyledText {
		x: 1308
		y: 20
		height: 136
		width: 93
		textLabel: "sound for"
	}
	ButtonSq {
		id: button3436_1456
		x: 1308
		y: 20
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "sound for"
		clickHandler.onClicked: onActivate3436()
	}
	StyledText {
		x: 1308
		y: 122
		height: 136
		width: 93
		textLabel: "1 sec"
	}
	ButtonSq {
		id: button3437_1457
		x: 1308
		y: 122
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "1 sec"
		clickHandler.onClicked: onActivate3437()
	}
	StyledText {
		x: 1308
		y: 225
		height: 136
		width: 93
		textLabel: "5 sec"
	}
	ButtonSq {
		id: button3438_1458
		x: 1308
		y: 225
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "5 sec"
		clickHandler.onClicked: onActivate3438()
	}
	StyledText {
		x: 1308
		y: 327
		height: 136
		width: 93
		textLabel: "15 sec"
	}
	ButtonSq {
		id: button3439_1459
		x: 1308
		y: 327
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "15 sec"
		clickHandler.onClicked: onActivate3439()
	}
	StyledText {
		x: 1308
		y: 429
		height: 136
		width: 93
		textLabel: "30 sec"
	}
	ButtonSq {
		id: button3440_1460
		x: 1308
		y: 429
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "30 sec"
		clickHandler.onClicked: onActivate3440()
	}
	StyledText {
		x: 1308
		y: 531
		height: 136
		width: 93
		textLabel: "60 sec"
	}
	ButtonSq {
		id: button3441_1461
		x: 1308
		y: 531
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "60 sec"
		clickHandler.onClicked: onActivate3441()
	}
}