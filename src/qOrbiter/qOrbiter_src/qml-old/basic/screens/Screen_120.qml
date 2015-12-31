// UI1 Screen DevCallInProgress (120)
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
		id: button1785_1400
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	ButtonSq {
		id: button5891_1401
		x: 0
		y: 360
		width: 720
		height: 720
		buttonbackground: "skins/Basic/798"
		buttontext: ""
		clickHandler.onClicked: onActivate5891()
	}
	function onActivate5878() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 17999
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 28 26 \"17\" ")
	}
	ButtonSq {
		id: button5878_1402
		x: 240
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/2.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5878()
	}
	function onActivate5879() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18000
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 28 26 \"18\" ")
	}
	ButtonSq {
		id: button5879_1403
		x: 480
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/3.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5879()
	}
	function onActivate5880() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18001
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 28 26 \"19\" ")
	}
	ButtonSq {
		id: button5880_1404
		x: 0
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/4.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5880()
	}
	function onActivate5881() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18002
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 28 26 \"20\" ")
	}
	ButtonSq {
		id: button5881_1405
		x: 240
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/5.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5881()
	}
	function onActivate5882() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18003
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 28 26 \"21\" ")
	}
	ButtonSq {
		id: button5882_1406
		x: 480
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/6.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5882()
	}
	function onActivate5883() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18004
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 28 26 \"22\" ")
	}
	ButtonSq {
		id: button5883_1407
		x: 0
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/7.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5883()
	}
	function onActivate5884() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18005
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 28 26 \"23\" ")
	}
	ButtonSq {
		id: button5884_1408
		x: 240
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/8.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5884()
	}
	function onActivate5885() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18006
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 28 26 \"24\" ")
	}
	ButtonSq {
		id: button5885_1409
		x: 480
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/9.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5885()
	}
	function onActivate5886() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18007
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 28 26 \"32\" ")
	}
	ButtonSq {
		id: button5887_1410
		x: 0
		y: 1260
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/roundblack.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5887()
	}
	ButtonSq {
		id: button5888_1411
		x: 0
		y: 1260
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/astrisk.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5888()
	}
	function onActivate5890() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18008
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 28 26 \"33\" ")
	}
	ButtonSq {
		id: button5890_1412
		x: 480
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/266"
		buttontext: ""
		clickHandler.onClicked: onActivate5890()
	}
	ButtonSq {
		id: button5887_1413
		x: 480
		y: 1260
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/roundblack.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5887()
	}
	ButtonSq {
		id: button5889_1414
		x: 480
		y: 1260
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/pound.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5889()
	}
	function onActivate5896() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18014
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 28 26 \"25\" ")
	}
	ButtonSq {
		id: button5896_1415
		x: 240
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/0.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5896()
	}
	function onActivate5897() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18015
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 28 26 \"16\" ")
	}
	ButtonSq {
		id: button5897_1416
		x: 0
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/1.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5897()
	}
	ButtonSq {
		id: button5527_1417
		x: 783
		y: 102
		width: 823
		height: 927
		buttonbackground: "skins/Basic/6"
		buttontext: ""
		clickHandler.onClicked: onActivate5527()
	}
	function onActivate1284() 
	{
	 // Calling Command 336 (Phone_Drop) with CommandGroup_Parameters from 16514
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 336 ")
	}
	StyledText {
		x: 1680
		y: 900
		height: 240
		width: 180
		textLabel: "Drop Call"
	}
	ButtonSq {
		id: button1284_1418
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Drop Call"
		clickHandler.onClicked: onActivate1284()
	}
	function onActivate1285() 
	{
	 // Calling Command 234 (PL_Transfer) with CommandGroup_Parameters from 16547
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 234 83 \"100\" 265 \"<%=64%>\" ")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "Send to Voice Mail"
	}
	ButtonSq {
		id: button1285_1419
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Send to Voice Mail"
		clickHandler.onClicked: onActivate1285()
	}
	function onActivate1835() 
	{
	 // Calling Command 5 (Goto DesignObj) with CommandGroup_Parameters from 2828
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 5 ")
	}
	StyledText {
		x: 1680
		y: 360
		height: 240
		width: 180
		textLabel: "Add to Speed Dial"
	}
	ButtonSq {
		id: button1835_1420
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Add to Speed Dial"
		clickHandler.onClicked: onActivate1835()
	}
	function onActivate5534() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17518
		setCurrentScreen("Screen_267.qml")
	}
	StyledText {
		x: 1680
		y: 180
		height: 240
		width: 180
		textLabel: "Active Calls"
	}
	ButtonSq {
		id: button5534_1421
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Active Calls"
		clickHandler.onClicked: onActivate5534()
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "Transfer or Conference"
	}
	ButtonSq {
		id: button5532_1422
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Transfer or Conference"
		clickHandler.onClicked: onActivate5532()
	}
}