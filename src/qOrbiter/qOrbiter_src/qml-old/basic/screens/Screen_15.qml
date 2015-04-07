// UI1 Screen SingleCameraViewOnly (15)
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
		id: button1785_188
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
		id: button1784_189
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate1854() 
	{
	}
	ButtonSq {
		id: button1854_190
		x: 90
		y: 135
		width: 1507
		height: 683
		buttonbackground: "skins/Basic/<%=1%>"
		buttontext: ""
		clickHandler.onClicked: onActivate1854()
	}
	function onActivate2382() 
	{
	}
	ButtonSq {
		id: button2382_191
		x: 1680
		y: 360
		width: 240
		height: 720
		buttonbackground: "skins/Basic/7"
		buttontext: ""
		clickHandler.onClicked: onActivate2382()
	}
	function onActivate2383() 
	{
	}
	StyledText {
		x: 1680
		y: 360
		height: 236
		width: 160
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button2383_192
		x: 1680
		y: 360
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate2383()
	}
	function onActivate4297() 
	{
	 // Calling Command 200 (Move Up) with CommandGroup_Parameters from 16017
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 200 ")
	}
	ButtonSq {
		id: button4297_193
		x: 240
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_move_up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4297()
	}
	function onActivate4298() 
	{
	 // Calling Command 201 (Move Down) with CommandGroup_Parameters from 16016
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 201 ")
	}
	ButtonSq {
		id: button4298_194
		x: 960
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_move_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4298()
	}
	function onActivate4299() 
	{
	 // Calling Command 202 (Move Left) with CommandGroup_Parameters from 16018
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 202 ")
	}
	ButtonSq {
		id: button4299_195
		x: 0
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_move_left.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4299()
	}
	function onActivate4300() 
	{
	 // Calling Command 203 (Move Right) with CommandGroup_Parameters from 16019
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 203 ")
	}
	ButtonSq {
		id: button4300_196
		x: 1200
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_move_right.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4300()
	}
	function onActivate4301() 
	{
	 // Calling Command 684 (Zoom In) with CommandGroup_Parameters from 16020
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 684 ")
	}
	StyledText {
		x: 480
		y: 900
		height: 240
		width: 180
		textLabel: "Zoom
in"
	}
	ButtonSq {
		id: button4301_197
		x: 480
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/roundblack.png"
		buttontext: "Zoom
in"
		clickHandler.onClicked: onActivate4301()
	}
	function onActivate4302() 
	{
	 // Calling Command 685 (Zoom Out) with CommandGroup_Parameters from 16021
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 685 ")
	}
	StyledText {
		x: 720
		y: 900
		height: 240
		width: 180
		textLabel: "Zoom
out"
	}
	ButtonSq {
		id: button4302_198
		x: 720
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/roundblack.png"
		buttontext: "Zoom
out"
		clickHandler.onClicked: onActivate4302()
	}
	function onActivate5188() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17554
		setCurrentScreen("Screen_46.qml")
	 // Calling Command 826 (Speak in house) with CommandGroup_Parameters from 17207
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 826 2 \"<%=!%>\" 201 \"<%=1%>\" ")
	}
	StyledText {
		x: 1440
		y: 900
		height: 240
		width: 180
		textLabel: "speak"
	}
	ButtonSq {
		id: button5188_199
		x: 1440
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/roundblack.png"
		buttontext: "speak"
		clickHandler.onClicked: onActivate5188()
	}
	StyledText {
		x: 240
		y: 14
		height: 90
		width: 900
		textLabel: "<%=LD%>"
	}
}