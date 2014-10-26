// UI1 Screen MapClimate (125)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/map.png"
		width: 1920
		height: 1080
	}
	function onActivate2215() 
	{
	}
	ButtonSq {
		id: button2215_1478
		x: 27
		y: 20
		width: 1263
		height: 947
		buttonbackground: "skins/Basic/3"
		buttontext: ""
		clickHandler.onClicked: onActivate2215()
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
		id: button1785_1479
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate2308() 
	{
	}
	ButtonSq {
		id: button2308_1480
		x: 1440
		y: 180
		width: 480
		height: 0
		buttonbackground: "skins/Basic/15"
		buttontext: ""
		clickHandler.onClicked: onActivate2308()
	}
	function onActivate2307() 
	{
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 14120
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"2212.0.<%=12:1%>.2215\" 8 \"0\" ")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14121
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"12\" 5 \"<%=5%>\" ")
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 14122
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"2212.0.<%=12%>.2215\" 8 \"1\" ")
	}
	StyledText {
		x: 1440
		y: 180
		height: 236
		width: 160
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button2307_1481
		x: 1440
		y: 180
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Buttons/Climate/squareb.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate2307()
	}
	function onActivate2276() 
	{
	 // Calling Command 280 (Set Heat/Cool) with CommandGroup_Parameters from 14879
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 280 8 \"A\" ")
	}
	StyledText {
		x: 1308
		y: 20
		height: 136
		width: 93
		textLabel: "Auto"
	}
	ButtonSq {
		id: button2276_1482
		x: 1308
		y: 20
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "Auto"
		clickHandler.onClicked: onActivate2276()
	}
	function onActivate2278() 
	{
	 // Calling Command 280 (Set Heat/Cool) with CommandGroup_Parameters from 14880
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 280 8 \"H\" ")
	}
	StyledText {
		x: 1308
		y: 129
		height: 136
		width: 93
		textLabel: "Heat"
	}
	ButtonSq {
		id: button2278_1483
		x: 1308
		y: 129
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "Heat"
		clickHandler.onClicked: onActivate2278()
	}
	function onActivate2279() 
	{
	 // Calling Command 280 (Set Heat/Cool) with CommandGroup_Parameters from 14881
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 280 8 \"C\" ")
	}
	StyledText {
		x: 1308
		y: 237
		height: 136
		width: 93
		textLabel: "Cool"
	}
	ButtonSq {
		id: button2279_1484
		x: 1308
		y: 237
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "Cool"
		clickHandler.onClicked: onActivate2279()
	}
	function onActivate2277() 
	{
	 // Calling Command 279 (Set Fan) with CommandGroup_Parameters from 14882
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 279 8 \"1\" ")
	}
	StyledText {
		x: 1308
		y: 345
		height: 136
		width: 93
		textLabel: "Fan On"
	}
	ButtonSq {
		id: button2277_1485
		x: 1308
		y: 345
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "Fan On"
		clickHandler.onClicked: onActivate2277()
	}
	function onActivate2282() 
	{
	 // Calling Command 193 (Off) with CommandGroup_Parameters from 14884
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 193 ")
	}
	StyledText {
		x: 1308
		y: 562
		height: 136
		width: 93
		textLabel: "Off"
	}
	ButtonSq {
		id: button2282_1486
		x: 1308
		y: 562
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "Off"
		clickHandler.onClicked: onActivate2282()
	}
	function onActivate2280() 
	{
	 // Calling Command 278 (Set Temperature) with CommandGroup_Parameters from 14885
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 278 5 \"+1\" ")
	}
	StyledText {
		x: 1308
		y: 670
		height: 136
		width: 93
		textLabel: "+"
	}
	ButtonSq {
		id: button2280_1487
		x: 1308
		y: 670
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "+"
		clickHandler.onClicked: onActivate2280()
	}
	function onActivate2281() 
	{
	 // Calling Command 278 (Set Temperature) with CommandGroup_Parameters from 14886
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 278 5 \"-1\" ")
	}
	StyledText {
		x: 1308
		y: 778
		height: 136
		width: 93
		textLabel: "-"
	}
	ButtonSq {
		id: button2281_1488
		x: 1308
		y: 778
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "-"
		clickHandler.onClicked: onActivate2281()
	}
	function onActivate2345() 
	{
	 // Calling Command 279 (Set Fan) with CommandGroup_Parameters from 14883
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 279 8 \"0\" ")
	}
	StyledText {
		x: 1308
		y: 453
		height: 136
		width: 93
		textLabel: "Fan Auto"
	}
	ButtonSq {
		id: button2345_1489
		x: 1308
		y: 453
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "Fan Auto"
		clickHandler.onClicked: onActivate2345()
	}
	function onActivate2276() 
	{
	 // Calling Command 280 (Set Heat/Cool) with CommandGroup_Parameters from 14879
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 280 8 \"A\" ")
	}
	StyledText {
		x: 1308
		y: 20
		height: 136
		width: 93
		textLabel: "Auto"
	}
	ButtonSq {
		id: button2276_1490
		x: 1308
		y: 20
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "Auto"
		clickHandler.onClicked: onActivate2276()
	}
	function onActivate2588() 
	{
	}
	StyledText {
		x: 1308
		y: 129
		height: 136
		width: 93
		textLabel: "On"
	}
	ButtonSq {
		id: button2588_1491
		x: 1308
		y: 129
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "On"
		clickHandler.onClicked: onActivate2588()
	}
	function onActivate2282() 
	{
	 // Calling Command 193 (Off) with CommandGroup_Parameters from 14884
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 193 ")
	}
	StyledText {
		x: 1308
		y: 237
		height: 136
		width: 93
		textLabel: "Off"
	}
	ButtonSq {
		id: button2282_1492
		x: 1308
		y: 237
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "Off"
		clickHandler.onClicked: onActivate2282()
	}
	function onActivate2281() 
	{
	 // Calling Command 278 (Set Temperature) with CommandGroup_Parameters from 14886
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 278 5 \"-1\" ")
	}
	StyledText {
		x: 1308
		y: 345
		height: 136
		width: 93
		textLabel: "-"
	}
	ButtonSq {
		id: button2281_1493
		x: 1308
		y: 345
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "-"
		clickHandler.onClicked: onActivate2281()
	}
	function onActivate2280() 
	{
	 // Calling Command 278 (Set Temperature) with CommandGroup_Parameters from 14885
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 278 5 \"+1\" ")
	}
	StyledText {
		x: 1308
		y: 453
		height: 136
		width: 93
		textLabel: "+"
	}
	ButtonSq {
		id: button2280_1494
		x: 1308
		y: 453
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "+"
		clickHandler.onClicked: onActivate2280()
	}
	function onActivate2588() 
	{
	}
	StyledText {
		x: 1308
		y: 20
		height: 136
		width: 93
		textLabel: "On"
	}
	ButtonSq {
		id: button2588_1495
		x: 1308
		y: 20
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "On"
		clickHandler.onClicked: onActivate2588()
	}
	function onActivate2282() 
	{
	 // Calling Command 193 (Off) with CommandGroup_Parameters from 14884
		manager.sendDceMessage(srouterip + " "+deviceid + "  1 193 ")
	}
	StyledText {
		x: 1308
		y: 129
		height: 136
		width: 93
		textLabel: "Off"
	}
	ButtonSq {
		id: button2282_1496
		x: 1308
		y: 129
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "Off"
		clickHandler.onClicked: onActivate2282()
	}
	StyledText {
		x: 1308
		y: 237
		height: 136
		width: 93
		textLabel: "5 min"
	}
	ButtonSq {
		id: button2591_1497
		x: 1308
		y: 237
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "5 min"
		clickHandler.onClicked: onActivate2591()
	}
	StyledText {
		x: 1308
		y: 345
		height: 136
		width: 93
		textLabel: "10 min"
	}
	ButtonSq {
		id: button2592_1498
		x: 1308
		y: 345
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "10 min"
		clickHandler.onClicked: onActivate2592()
	}
	StyledText {
		x: 1308
		y: 453
		height: 136
		width: 93
		textLabel: "20 min"
	}
	ButtonSq {
		id: button2593_1499
		x: 1308
		y: 453
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "20 min"
		clickHandler.onClicked: onActivate2593()
	}
	StyledText {
		x: 1308
		y: 562
		height: 136
		width: 93
		textLabel: "30 min"
	}
	ButtonSq {
		id: button2594_1500
		x: 1308
		y: 562
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "30 min"
		clickHandler.onClicked: onActivate2594()
	}
	StyledText {
		x: 1308
		y: 670
		height: 136
		width: 93
		textLabel: "45 min"
	}
	ButtonSq {
		id: button2595_1501
		x: 1308
		y: 670
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "45 min"
		clickHandler.onClicked: onActivate2595()
	}
	StyledText {
		x: 1308
		y: 778
		height: 136
		width: 93
		textLabel: "1 hour"
	}
	ButtonSq {
		id: button2596_1502
		x: 1308
		y: 778
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "1 hour"
		clickHandler.onClicked: onActivate2596()
	}
	function onActivate3303() 
	{
	 // Calling Command 261 (Set Follow-Me) with CommandGroup_Parameters from 14359
		manager.sendDceMessage(srouterip + " "+deviceid + " -101 1 261 2 \"<%=!%>\" 9 \"C<%=GD2212.0.0.2607%>\" 17 \"<%=U%>\" ")
	}
	ButtonSq {
		id: button3303_1503
		x: 1440
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Climate/squareb.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3303()
	}
	ButtonSq {
		id: button2607_1504
		x: 1458
		y: 3
		width: 198
		height: 171
		buttonbackground: "skins/Basic/mobile/icons/dontfolllowme.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2607()
	}
}