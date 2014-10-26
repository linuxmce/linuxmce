// UI1 Screen MakeCallIntercom (23)
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
		id: button1679_293
		x: 117
		y: 169
		width: 1386
		height: 633
		buttonbackground: "skins/Basic/11"
		buttontext: ""
		clickHandler.onClicked: onActivate1679()
	}
	function onActivate1678() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 17443
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"19\" 5 \"<%=5%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2720
		setCurrentScreen("Screen_21.qml")
	}
	StyledText {
		x: 117
		y: 169
		height: 265
		width: 346
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button1678_294
		x: 117
		y: 169
		width: 462
		height: 199
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate1678()
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
		id: button1785_295
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate2035() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17553
		setCurrentScreen("Screen_46.qml")
	 // Calling Command 826 (Speak in house) with CommandGroup_Parameters from 17460
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 826 2 \"<%=!%>\" ")
	}
	StyledText {
		x: 361
		y: 890
		height: 213
		width: 698
		textLabel: "Broadcast to Media Directors"
	}
	ButtonSq {
		id: button2035_296
		x: 361
		y: 890
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Broadcast to Media Directors"
		clickHandler.onClicked: onActivate2035()
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
		id: button1319_297
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
		id: button1321_298
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Speed
Dial"
		clickHandler.onClicked: onActivate1321()
	}
	function onActivate1683() 
	{
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
		id: button1683_299
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard_on.png"
		buttontext: "Call
Intercom"
		clickHandler.onClicked: onActivate1683()
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
		id: button1320_300
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
		id: button5534_301
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Active Calls"
		clickHandler.onClicked: onActivate5534()
	}
}