// UI1 Screen DevIncomingCall (121)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/newphone.png"
		width: 1920
		height: 1080
	}
	StyledText {
		x: 45
		y: 426
		height: 120
		width: 566
		textLabel: "Move This Call"
	}
	ButtonSq {
		id: button2177_1423
		x: 45
		y: 426
		width: 755
		height: 639
		buttonbackground: "skins/Basic/Phone/incomingareas.png"
		buttontext: "Move This Call"
		clickHandler.onClicked: onActivate2177()
	}
	function onActivate2411() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 11916
		setCurrentScreen("Screen_11.qml")
	}
	StyledText {
		x: 285
		y: 516
		height: 240
		width: 180
		textLabel: "Outside Number"
	}
	ButtonSq {
		id: button2411_1424
		x: 285
		y: 516
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Outside Number"
		clickHandler.onClicked: onActivate2411()
	}
	ButtonSq {
		id: button4821_1425
		x: 63
		y: 697
		width: 720
		height: 360
		buttonbackground: "skins/Basic/11"
		buttontext: ""
		clickHandler.onClicked: onActivate4821()
	}
	function onActivate4822() 
	{
	 // Calling Command 923 (Assisted Make Call) with CommandGroup_Parameters from 17527
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 923 263 \"<%=5%>\" ")
	}
	StyledText {
		x: 108
		y: 1123
		height: 240
		width: 180
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button4822_1426
		x: 108
		y: 1123
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate4822()
	}
	function onActivate1285() 
	{
	 // Calling Command 234 (PL_Transfer) with CommandGroup_Parameters from 16547
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 234 83 \"100\" 265 \"<%=64%>\" ")
	}
	StyledText {
		x: 857
		y: 426
		height: 240
		width: 180
		textLabel: "Send to Voice Mail"
	}
	ButtonSq {
		id: button1285_1427
		x: 857
		y: 426
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Send to Voice Mail"
		clickHandler.onClicked: onActivate1285()
	}
	function onActivate1942() 
	{
	 // Calling Command 236 (PL_Cancel) with CommandGroup_Parameters from 16546
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 236 264 \"<%=64%>\" ")
	}
	StyledText {
		x: 857
		y: 629
		height: 240
		width: 180
		textLabel: "Reject Call"
	}
	ButtonSq {
		id: button1942_1428
		x: 857
		y: 629
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Reject Call"
		clickHandler.onClicked: onActivate1942()
	}
	ButtonSq {
		id: button2183_1429
		x: 857
		y: 629
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Icons/comm/reject.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2183()
	}
	StyledText {
		x: 1119
		y: 426
		height: 90
		width: 566
		textLabel: "Transfer Call"
	}
	ButtonSq {
		id: button2178_1430
		x: 1119
		y: 426
		width: 755
		height: 639
		buttonbackground: "skins/Basic/Phone/incomingareas.png"
		buttontext: "Transfer Call"
		clickHandler.onClicked: onActivate2178()
	}
	function onActivate1287() 
	{
	}
	ButtonSq {
		id: button1287_1431
		x: 1137
		y: 494
		width: 720
		height: 540
		buttonbackground: "skins/Basic/13"
		buttontext: ""
		clickHandler.onClicked: onActivate1287()
	}
	function onActivate1286() 
	{
	 // Calling Command 923 (Assisted Make Call) with CommandGroup_Parameters from 17526
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 923 17 \"<%=U%>\" ")
	}
	StyledText {
		x: 2256
		y: 920
		height: 240
		width: 180
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button1286_1432
		x: 2256
		y: 920
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate1286()
	}
	ButtonSq {
		id: button2030_1433
		x: 4543
		y: 1854
		width: 144
		height: 108
		buttonbackground: "skins/Basic/../../users/<%=5:0%>.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2030()
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
		id: button1785_1434
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
		id: button1784_1435
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate1282() 
	{
	 // Calling Command 335 (Phone_Answer) with CommandGroup_Parameters from 16513
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 335 ")
	}
	StyledText {
		x: 135
		y: 257
		height: 211
		width: 346
		textLabel: "Answer on
This Orbiter"
	}
	ButtonSq {
		id: button1282_1436
		x: 135
		y: 257
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Answer on
This Orbiter"
		clickHandler.onClicked: onActivate1282()
	}
}