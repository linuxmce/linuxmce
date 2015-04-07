// UI1 Screen TVEPG3 (101)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
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
		id: button1785_1294
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate1569() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13739
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18035
		setCurrentScreen("Screen_1.qml")
	}
	ButtonSq {
		id: button1569_1295
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
	function onActivate4389() 
	{
	 // Calling Command 9 (Scroll Grid) with CommandGroup_Parameters from 16109
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 9 30 \"1\" ")
	}
	ButtonSq {
		id: button4389_1296
		x: 1800
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4389()
	}
	function onActivate4391() 
	{
	 // Calling Command 9 (Scroll Grid) with CommandGroup_Parameters from 16110
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 9 30 \"3\" ")
	}
	ButtonSq {
		id: button4391_1297
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/left.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4391()
	}
	function onActivate4392() 
	{
	 // Calling Command 9 (Scroll Grid) with CommandGroup_Parameters from 16108
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 9 30 \"4\" ")
	}
	ButtonSq {
		id: button4392_1298
		x: 1920
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/right.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4392()
	}
	function onActivate4390() 
	{
	 // Calling Command 9 (Scroll Grid) with CommandGroup_Parameters from 16107
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 9 30 \"2\" ")
	}
	ButtonSq {
		id: button4390_1299
		x: 1800
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4390()
	}
	function onActivate4370() 
	{
	 // Calling Command 698 (Get Extended Media Data) with CommandGroup_Parameters from 16104
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 698 3 \"4383\" 68 \"<%=2%>\" ")
	}
	StyledText {
		x: 1680
		y: 360
		height: 120
		width: 90
		textLabel: "Info"
	}
	ButtonSq {
		id: button4370_1300
		x: 1680
		y: 360
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/roundblack.png"
		buttontext: "Info"
		clickHandler.onClicked: onActivate4370()
	}
	function onActivate4369() 
	{
	}
	StyledText {
		x: 1680
		y: 443
		height: 120
		width: 90
		textLabel: "Search"
	}
	StyledText {
		x: 1680
		y: 443
		height: 120
		width: 90
		textLabel: "Search"
	}
	ButtonSq {
		id: button4369_1301
		x: 1680
		y: 443
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/roundblack.png"
		buttontext: "Search"
		clickHandler.onClicked: onActivate4369()
	}
	function onActivate4378() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 12569
		setCurrentScreen("Screen_99.qml")
	}
	StyledText {
		x: 1803
		y: 360
		height: 120
		width: 90
		textLabel: "View"
	}
	ButtonSq {
		id: button4378_1302
		x: 1803
		y: 360
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/roundblack.png"
		buttontext: "View"
		clickHandler.onClicked: onActivate4378()
	}
	function onActivate4397() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17015
		setCurrentScreen("Screen_220.qml")
	}
	ButtonSq {
		id: button4397_1303
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4397()
	}
	function onActivate4401() 
	{
	 // Calling Command 698 (Get Extended Media Data) with CommandGroup_Parameters from 16127
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 698 68 \"<%=2%>\" ")
	}
	function onActivate4362() 
	{
	 // Calling Command 65 (Jump Position In Playlist) with CommandGroup_Parameters from 16101
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 65 5 \"<%=2%>\" ")
	}
	StyledText {
		x: 1440
		y: 900
		height: 240
		width: 180
		textLabel: "Go"
	}
	StyledText {
		x: 1440
		y: 900
		height: 240
		width: 180
		textLabel: "Go"
	}
	ButtonSq {
		id: button4362_1304
		x: 1440
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/roundblack.png"
		buttontext: "Go"
		clickHandler.onClicked: onActivate4362()
	}
}