// UI1 Screen DevMoveCallsInProgress (122)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
	}
	ButtonSq {
		id: button1622_1437
		x: 108
		y: 264
		width: 1678
		height: 180
		buttonbackground: "skins/Basic/11"
		buttontext: ""
		clickHandler.onClicked: onActivate1622()
	}
	function onActivate1623() 
	{
	}
	StyledText {
		x: 117
		y: 264
		height: 236
		width: 149
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button1623_1438
		x: 108
		y: 264
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate1623()
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
		id: button1785_1439
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
		id: button1784_1440
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate2043() 
	{
	}
	StyledText {
		x: 108
		y: 68
		height: 213
		width: 698
		textLabel: "Move Call Here"
	}
	ButtonSq {
		id: button2043_1441
		x: 108
		y: 68
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Move Call Here"
		clickHandler.onClicked: onActivate2043()
	}
	ButtonSq {
		id: button2359_1442
		x: 108
		y: 460
		width: 1516
		height: 180
		buttonbackground: "skins/Basic/10"
		buttontext: ""
		clickHandler.onClicked: onActivate2359()
	}
	function onActivate2360() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 12094
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 21 \"1\" ")
	}
	StyledText {
		x: 108
		y: 460
		height: 236
		width: 160
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button2360_1443
		x: 108
		y: 460
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate2360()
	}
	ButtonSq {
		id: button2357_1444
		x: 108
		y: 656
		width: 1516
		height: 180
		buttonbackground: "skins/Basic/19"
		buttontext: ""
		clickHandler.onClicked: onActivate2357()
	}
	function onActivate2358() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 12092
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 21 \"1\" ")
	}
	StyledText {
		x: 108
		y: 656
		height: 236
		width: 160
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button2358_1445
		x: 108
		y: 656
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate2358()
	}
	ButtonSq {
		id: button2362_1446
		x: 108
		y: 853
		width: 1516
		height: 180
		buttonbackground: "skins/Basic/11"
		buttontext: ""
		clickHandler.onClicked: onActivate2362()
	}
	function onActivate2364() 
	{
	 // Calling Command 234 (PL_Transfer) with CommandGroup_Parameters from 16548
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 234 2 \"<%=5%>\" 196 \"<%=12%>\" ")
	}
	StyledText {
		x: 108
		y: 853
		height: 236
		width: 160
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button2364_1447
		x: 108
		y: 853
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate2364()
	}
}