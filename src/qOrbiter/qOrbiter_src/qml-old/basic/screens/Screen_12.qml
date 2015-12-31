// UI1 Screen MakeCallPlutoUser (12)
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
		x: 1678
		y: 0
		height: 1
		width: 1
		textLabel: "Main menu"
	}
	ButtonSq {
		id: button1785_151
		x: 1678
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
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
		id: button1319_152
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Phone
Book"
		clickHandler.onClicked: onActivate1319()
	}
	function onActivate1316() 
	{
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
		id: button1316_153
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard_on.png"
		buttontext: "Dial
Direct"
		clickHandler.onClicked: onActivate1316()
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
		id: button1321_154
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
		id: button1682_155
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Call
Intercom"
		clickHandler.onClicked: onActivate1682()
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
		id: button5534_156
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Active Calls"
		clickHandler.onClicked: onActivate5534()
	}
	function onActivate1287() 
	{
	}
	ButtonSq {
		id: button1287_157
		x: 120
		y: 90
		width: 1440
		height: 900
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
		x: 120
		y: 90
		height: 240
		width: 180
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button1286_158
		x: 120
		y: 90
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate1286()
	}
	ButtonSq {
		id: button2030_159
		x: 272
		y: 194
		width: 144
		height: 108
		buttonbackground: "skins/Basic/../../users/<%=5:0%>.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2030()
	}
}