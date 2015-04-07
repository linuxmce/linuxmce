// UI1 Screen TVEPG1 (99)
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
		id: button1785_1281
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
		id: button1569_1282
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
	function onActivate6053() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18213
		setCurrentScreen("Screen_100.qml")
	}
	ButtonSq {
		id: button6053_1283
		x: 0
		y: 0
		width: 1680
		height: 1080
		buttonbackground: "skins/Basic/28"
		buttontext: ""
		clickHandler.onClicked: onActivate6053()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_1284
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate6049() 
	{
	 // Calling Command 9 (Scroll Grid) with CommandGroup_Parameters from 18209
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 9 30 \"3\" ")
	}
	ButtonSq {
		id: button6049_1285
		x: 1680
		y: 540
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/TabletControls/left.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6049()
	}
	function onActivate6050() 
	{
	 // Calling Command 9 (Scroll Grid) with CommandGroup_Parameters from 18210
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 9 30 \"4\" ")
	}
	ButtonSq {
		id: button6050_1286
		x: 1800
		y: 540
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/TabletControls/right.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6050()
	}
	function onActivate6051() 
	{
	 // Calling Command 9 (Scroll Grid) with CommandGroup_Parameters from 18211
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 9 30 \"1\" ")
	}
	ButtonSq {
		id: button6051_1287
		x: 1740
		y: 450
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/TabletControls/up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6051()
	}
	function onActivate6052() 
	{
	 // Calling Command 9 (Scroll Grid) with CommandGroup_Parameters from 18212
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 9 30 \"2\" ")
	}
	ButtonSq {
		id: button6052_1288
		x: 1740
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/TabletControls/down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6052()
	}
}