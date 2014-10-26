// UI1 Screen MakeCallDevice (21)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
	}
	function onActivate1619() 
	{
	}
	ButtonSq {
		id: button1619_277
		x: 90
		y: 338
		width: 1462
		height: 643
		buttonbackground: "skins/Basic/11"
		buttontext: ""
		clickHandler.onClicked: onActivate1619()
	}
	function onActivate1620() 
	{
	 // Calling Command 923 (Assisted Make Call) with CommandGroup_Parameters from 17520
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 923 17 \"<%=39%>\" 83 \"<%=17%>\" 184 \"<%=5%>\" 263 \"<%=19%>\" ")
	}
	StyledText {
		x: 90
		y: 338
		height: 240
		width: 180
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button1620_278
		x: 90
		y: 338
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate1620()
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
		id: button1785_279
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate2044() 
	{
	 // Calling Command 923 (Assisted Make Call) with CommandGroup_Parameters from 17519
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 923 17 \"<%=39%>\" 83 \"<%=17%>\" 184 \"<%=!%>\" 263 \"<%=19%>\" ")
	}
	StyledText {
		x: 379
		y: 169
		height: 213
		width: 698
		textLabel: "Place From Here"
	}
	ButtonSq {
		id: button2044_280
		x: 379
		y: 169
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Place From Here"
		clickHandler.onClicked: onActivate2044()
	}
	function onActivate1319() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2500
		setCurrentScreen("Screen_9.qml")
	}
	StyledText {
		x: 1680
		y: 360
		height: 240
		width: 180
		textLabel: "Phone
Book"
	}
	ButtonSq {
		id: button1319_281
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Phone
Book"
		clickHandler.onClicked: onActivate1319()
	}
	function onActivate1321() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2207
		setCurrentScreen("Screen_10.qml")
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "Speed
Dial"
	}
	ButtonSq {
		id: button1321_282
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Speed
Dial"
		clickHandler.onClicked: onActivate1321()
	}
	function onActivate1682() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2416
		setCurrentScreen("Screen_23.qml")
	}
	StyledText {
		x: 1680
		y: 900
		height: 240
		width: 180
		textLabel: "Call
Intercom"
	}
	ButtonSq {
		id: button1682_283
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Call
Intercom"
		clickHandler.onClicked: onActivate1682()
	}
	function onActivate1320() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2210
		setCurrentScreen("Screen_11.qml")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "Dial
Direct"
	}
	ButtonSq {
		id: button1320_284
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Dial
Direct"
		clickHandler.onClicked: onActivate1320()
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
		id: button5534_285
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Active Calls"
		clickHandler.onClicked: onActivate5534()
	}
}