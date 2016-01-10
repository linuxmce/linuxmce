// UI1 Screen MapLighting (124)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/map.png"
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
		id: button1785_1462
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate2217() 
	{
	}
	ButtonSq {
		id: button2217_1463
		x: 27
		y: 20
		width: 1173
		height: 947
		buttonbackground: "skins/Basic/2"
		buttontext: ""
		clickHandler.onClicked: onActivate2217()
	}
	function onActivate2209() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 18237
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 184 76 \"100\" ")
	}
	StyledText {
		x: 1263
		y: 200
		height: 136
		width: 93
		textLabel: "Full"
	}
	ButtonSq {
		id: button2209_1464
		x: 1263
		y: 200
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "Full"
		clickHandler.onClicked: onActivate2209()
	}
	function onActivate2270() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14378
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 184 76 \"90\" ")
	}
	StyledText {
		x: 1263
		y: 309
		height: 136
		width: 93
		textLabel: "90%"
	}
	ButtonSq {
		id: button2270_1465
		x: 1263
		y: 309
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "90%"
		clickHandler.onClicked: onActivate2270()
	}
	function onActivate2271() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14379
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 184 76 \"70\" ")
	}
	StyledText {
		x: 1263
		y: 417
		height: 136
		width: 93
		textLabel: "70%"
	}
	ButtonSq {
		id: button2271_1466
		x: 1263
		y: 417
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "70%"
		clickHandler.onClicked: onActivate2271()
	}
	function onActivate2272() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14380
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 184 76 \"50\" ")
	}
	StyledText {
		x: 1263
		y: 525
		height: 136
		width: 93
		textLabel: "50%"
	}
	ButtonSq {
		id: button2272_1467
		x: 1263
		y: 525
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "50%"
		clickHandler.onClicked: onActivate2272()
	}
	function onActivate2273() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14381
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 184 76 \"30\" ")
	}
	StyledText {
		x: 1263
		y: 633
		height: 136
		width: 93
		textLabel: "30%"
	}
	ButtonSq {
		id: button2273_1468
		x: 1263
		y: 633
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "30%"
		clickHandler.onClicked: onActivate2273()
	}
	function onActivate2274() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14382
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 184 76 \"10\" ")
	}
	StyledText {
		x: 1263
		y: 742
		height: 136
		width: 93
		textLabel: "10%"
	}
	ButtonSq {
		id: button2274_1469
		x: 1263
		y: 742
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "10%"
		clickHandler.onClicked: onActivate2274()
	}
	function onActivate2210() 
	{
	 // Calling Command 193 (Off) with CommandGroup_Parameters from 13970
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 193 ")
	}
	StyledText {
		x: 1263
		y: 850
		height: 136
		width: 93
		textLabel: "Off"
	}
	ButtonSq {
		id: button2210_1470
		x: 1263
		y: 850
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "Off"
		clickHandler.onClicked: onActivate2210()
	}
	ButtonSq {
		id: button2306_1471
		x: 1440
		y: 180
		width: 480
		height: 0
		buttonbackground: "skins/Basic/15"
		buttontext: ""
		clickHandler.onClicked: onActivate2306()
	}
	function onActivate2305() 
	{
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 13690
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"2211.0.<%=12:1%>.2217\" 8 \"0\" ")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13691
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"12\" 5 \"<%=5%>\" ")
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 13692
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"2211.0.<%=12%>.2217\" 8 \"1\" ")
	}
	StyledText {
		x: 1440
		y: 180
		height: 236
		width: 160
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button2305_1472
		x: 1440
		y: 180
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Buttons/Lights/squareb.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate2305()
	}
	function onActivate3302() 
	{
	 // Calling Command 261 (Set Follow-Me) with CommandGroup_Parameters from 14352
		manager.sendDceMessage(srouterip + " "+deviceid + " -101 1 261 2 \"<%=!%>\" 9 \"L<%=GD2211.0.0.2607%>\" 17 \"<%=U%>\" ")
	}
	ButtonSq {
		id: button3302_1473
		x: 1200
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Lights/squareb.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3302()
	}
	ButtonSq {
		id: button2607_1474
		x: 1218
		y: 3
		width: 198
		height: 171
		buttonbackground: "skins/Basic/mobile/icons/dontfolllowme.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2607()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_1475
		x: 1440
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate2210() 
	{
	 // Calling Command 193 (Off) with CommandGroup_Parameters from 13970
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 193 ")
	}
	StyledText {
		x: 1263
		y: 200
		height: 136
		width: 93
		textLabel: "Off"
	}
	ButtonSq {
		id: button2210_1476
		x: 1263
		y: 200
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "Off"
		clickHandler.onClicked: onActivate2210()
	}
	function onActivate5587() 
	{
	 // Calling Command 192 (On) with CommandGroup_Parameters from 17639
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 192 ")
	}
	StyledText {
		x: 1263
		y: 309
		height: 136
		width: 93
		textLabel: "On"
	}
	ButtonSq {
		id: button5587_1477
		x: 1263
		y: 309
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "On"
		clickHandler.onClicked: onActivate5587()
	}
}