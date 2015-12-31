// UI1 Screen MakeCallFavorites (10)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/favoritesB.png"
		width: 1920
		height: 1080
	}
	function onActivate1384() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2834
		setCurrentScreen("Screen_21.qml")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 16909
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"<%=46%>\" ")
	}
	function onActivate1383() 
	{
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
		id: button1785_121
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate1962() 
	{
	}
	function onActivate1963() 
	{
	}
	function onActivate1964() 
	{
	}
	function onActivate1965() 
	{
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
		id: button1682_122
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
		id: button1320_123
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Dial
Direct"
		clickHandler.onClicked: onActivate1320()
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
		id: button1317_124
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard_on.png"
		buttontext: "Speed
Dial"
		clickHandler.onClicked: onActivate1317()
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
		id: button1319_125
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Phone
Book"
		clickHandler.onClicked: onActivate1319()
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
		id: button5534_126
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Active Calls"
		clickHandler.onClicked: onActivate5534()
	}
}