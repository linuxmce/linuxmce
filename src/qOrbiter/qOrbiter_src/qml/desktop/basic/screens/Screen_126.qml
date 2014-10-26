// UI1 Screen MapTelecom (126)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/map.png"
		width: 1920
		height: 1080
	}
	ButtonSq {
		id: button2216_1505
		x: 27
		y: 20
		width: 1408
		height: 589
		buttonbackground: "skins/Basic/7"
		buttontext: ""
		clickHandler.onClicked: onActivate2216()
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
		id: button1785_1506
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate3276() 
	{
	}
	ButtonSq {
		id: button3276_1507
		x: 1440
		y: 180
		width: 480
		height: 0
		buttonbackground: "skins/Basic/15"
		buttontext: ""
		clickHandler.onClicked: onActivate3276()
	}
	function onActivate3277() 
	{
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 14126
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"2213.0.<%=12:1%>.2216\" 8 \"0\" ")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14127
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"12\" 5 \"<%=5%>\" ")
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 14128
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"2213.0.<%=12%>.2216\" 8 \"1\" ")
	}
	StyledText {
		x: 1440
		y: 180
		height: 236
		width: 160
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button3277_1508
		x: 1440
		y: 180
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Buttons/Comm/squareb.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate3277()
	}
	function onActivate3304() 
	{
	 // Calling Command 261 (Set Follow-Me) with CommandGroup_Parameters from 14363
		manager.sendDceMessage(srouterip + " "+deviceid + " -101 1 261 2 \"<%=!%>\" 9 \"T<%=GD2213.0.0.2607%>\" 17 \"<%=U%>\" ")
	}
	ButtonSq {
		id: button3304_1509
		x: 1442
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Comm/squareb.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3304()
	}
	ButtonSq {
		id: button2607_1510
		x: 1460
		y: 3
		width: 198
		height: 171
		buttonbackground: "skins/Basic/mobile/icons/dontfolllowme.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2607()
	}
	function onActivate4787() 
	{
	 // Calling Command 927 (Add Extensions To Call) with CommandGroup_Parameters from 17528
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 927 87 \"<%=59%>\" 274 \"<%=STE%>\" ")
	}
	function onActivate5531() 
	{
	}
	StyledText {
		x: 25
		y: 920
		height: 211
		width: 346
		textLabel: "Call in Progress"
	}
	ButtonSq {
		id: button5531_1511
		x: 25
		y: 920
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Call in Progress"
		clickHandler.onClicked: onActivate5531()
	}
}