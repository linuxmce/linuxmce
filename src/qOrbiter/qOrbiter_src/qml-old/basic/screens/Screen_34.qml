// UI1 Screen Browser (34)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	StyledText {
		x: 18
		y: 14
		height: 240
		width: 1260
		textLabel: "LinuxMCE Web Browser"
	}
	function onActivate1920() 
	{
	}
	ButtonSq {
		id: button1920_403
		x: 1270
		y: 78
		width: 108
		height: 102
		buttonbackground: "skins/Basic/Browser/home.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1920()
	}
	function onActivate1923() 
	{
	}
	ButtonSq {
		id: button1923_404
		x: 837
		y: 78
		width: 108
		height: 102
		buttonbackground: "skins/Basic/Browser/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1923()
	}
	function onActivate1922() 
	{
	}
	ButtonSq {
		id: button1922_405
		x: 946
		y: 78
		width: 108
		height: 102
		buttonbackground: "skins/Basic/Browser/forward.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1922()
	}
	function onActivate1921() 
	{
	}
	ButtonSq {
		id: button1921_406
		x: 1054
		y: 78
		width: 108
		height: 102
		buttonbackground: "skins/Basic/Browser/stop.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1921()
	}
	function onActivate1924() 
	{
	}
	ButtonSq {
		id: button1924_407
		x: 1162
		y: 78
		width: 108
		height: 102
		buttonbackground: "skins/Basic/Browser/refresh.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1924()
	}
	function onActivate1925() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2744
		setCurrentScreen("Screen_33.qml")
	}
	StyledText {
		x: 1247
		y: 0
		height: 135
		width: 406
		textLabel: "URL/Options"
	}
	ButtonSq {
		id: button1925_408
		x: 1139
		y: 0
		width: 541
		height: 102
		buttonbackground: "skins/Basic/Browser/keyboard.png"
		buttontext: "URL/Options"
		clickHandler.onClicked: onActivate1925()
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
		id: button1785_409
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
}