// UI1 Screen Game Atari 5200 (153)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/PVR/PVRremote.png"
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
		id: button1785_1603
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate1569() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13739
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18035
		setCurrentScreen("Screen_1.qml")
	}
	ButtonSq {
		id: button1569_1604
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
	ButtonSq {
		id: button5911_1605
		x: 0
		y: 360
		width: 720
		height: 720
		buttonbackground: "skins/Basic/798"
		buttontext: ""
		clickHandler.onClicked: onActivate5911()
	}
	function onActivate3235() 
	{
	 // Calling Command 205 (1) with CommandGroup_Parameters from 13959
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 205 ")
	}
	ButtonSq {
		id: button3235_1606
		x: 0
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/1.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3235()
	}
	function onActivate3236() 
	{
	 // Calling Command 206 (2) with CommandGroup_Parameters from 13960
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 206 ")
	}
	ButtonSq {
		id: button3236_1607
		x: 240
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/2.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3236()
	}
	function onActivate3237() 
	{
	 // Calling Command 207 (3) with CommandGroup_Parameters from 13961
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 207 ")
	}
	ButtonSq {
		id: button3237_1608
		x: 480
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/3.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3237()
	}
	function onActivate3238() 
	{
	 // Calling Command 208 (4) with CommandGroup_Parameters from 13962
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 208 ")
	}
	ButtonSq {
		id: button3238_1609
		x: 0
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/4.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3238()
	}
	function onActivate3239() 
	{
	 // Calling Command 209 (5) with CommandGroup_Parameters from 13964
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 209 ")
	}
	ButtonSq {
		id: button3239_1610
		x: 240
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/5.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3239()
	}
	function onActivate3240() 
	{
	 // Calling Command 210 (6) with CommandGroup_Parameters from 13965
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 210 ")
	}
	ButtonSq {
		id: button3240_1611
		x: 480
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/6.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3240()
	}
	function onActivate3241() 
	{
	 // Calling Command 211 (7) with CommandGroup_Parameters from 13966
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 211 ")
	}
	ButtonSq {
		id: button3241_1612
		x: 0
		y: 1080
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/7.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3241()
	}
	function onActivate3242() 
	{
	 // Calling Command 212 (8) with CommandGroup_Parameters from 13967
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 212 ")
	}
	ButtonSq {
		id: button3242_1613
		x: 240
		y: 1080
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/8.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3242()
	}
	function onActivate3243() 
	{
	 // Calling Command 213 (9) with CommandGroup_Parameters from 13968
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 213 ")
	}
	ButtonSq {
		id: button3243_1614
		x: 480
		y: 1080
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/9.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3243()
	}
	function onActivate3234() 
	{
	 // Calling Command 204 (0) with CommandGroup_Parameters from 13958
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 204 ")
	}
	ButtonSq {
		id: button3234_1615
		x: 240
		y: 1260
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/0.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3234()
	}
	function onActivate5913() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18041
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 26 \"32\" ")
	}
	StyledText {
		x: 0
		y: 900
		height: 240
		width: 180
		textLabel: "*"
	}
	ButtonSq {
		id: button5913_1616
		x: 0
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "*"
		clickHandler.onClicked: onActivate5913()
	}
	function onActivate5912() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18039
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 26 \"33\" ")
	}
	StyledText {
		x: 480
		y: 900
		height: 240
		width: 180
		textLabel: "#"
	}
	ButtonSq {
		id: button5912_1617
		x: 480
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "#"
		clickHandler.onClicked: onActivate5912()
	}
	function onActivate2289() 
	{
	 // Calling Command 97 (Mute) with CommandGroup_Parameters from 13657
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 97 ")
	}
	StyledText {
		x: 960
		y: 540
		height: 240
		width: 180
		textLabel: "Mute"
	}
	ButtonSq {
		id: button2289_1618
		x: 960
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_mute.png"
		buttontext: "Mute"
		clickHandler.onClicked: onActivate2289()
	}
	function onActivate1850() 
	{
	 // Calling Command 89 (Vol Up) with CommandGroup_Parameters from 13671
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 89 ")
	}
	ButtonSq {
		id: button1850_1619
		x: 960
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1850()
	}
	function onActivate1851() 
	{
	 // Calling Command 90 (Vol Down) with CommandGroup_Parameters from 13672
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 90 ")
	}
	ButtonSq {
		id: button1851_1620
		x: 960
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1851()
	}
	function onActivate2028() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 9488
		setCurrentScreen("Screen_52.qml")
	}
	StyledText {
		x: 1200
		y: 900
		height: 240
		width: 180
		textLabel: "Resend
A/V
codes"
	}
	ButtonSq {
		id: button2028_1621
		x: 1200
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Resend
A/V
codes"
		clickHandler.onClicked: onActivate2028()
	}
	function onActivate2458() 
	{
	 // Calling Command 401 (Show File List) with CommandGroup_Parameters from 15962
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 401 29 \"<%=MT%>\" ")
	}
	StyledText {
		x: 1680
		y: 180
		height: 240
		width: 180
		textLabel: "file
list"
	}
	ButtonSq {
		id: button2458_1622
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "file
list"
		clickHandler.onClicked: onActivate2458()
	}
	function onActivate5623() 
	{
	 // Calling Command 87 (Goto Media Menu) with CommandGroup_Parameters from 17666
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 87 64 \"1\" ")
	}
	StyledText {
		x: 720
		y: 900
		height: 240
		width: 180
		textLabel: "Monitor Mode"
	}
	ButtonSq {
		id: button5623_1623
		x: 720
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Monitor Mode"
		clickHandler.onClicked: onActivate5623()
	}
	function onActivate3547() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 15050
		setCurrentScreen("Screen_87.qml")
	}
	StyledText {
		x: 960
		y: 900
		height: 240
		width: 180
		textLabel: "Book
marks"
	}
	ButtonSq {
		id: button3547_1624
		x: 960
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Book
marks"
		clickHandler.onClicked: onActivate3547()
	}
	ButtonSq {
		id: button3368_1625
		x: 1200
		y: 0
		width: 480
		height: 360
		buttonbackground: "skins/Basic/Backgrounds/logo.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3368()
	}
	function onActivate5914() 
	{
	 // Calling Command 943 (Game 1P Start) with CommandGroup_Parameters from 18043
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 943 ")
	}
	StyledText {
		x: 1200
		y: 540
		height: 240
		width: 180
		textLabel: "Start"
	}
	ButtonSq {
		id: button5914_1626
		x: 1200
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/roundblack.png"
		buttontext: "Start"
		clickHandler.onClicked: onActivate5914()
	}
	function onActivate5915() 
	{
	 // Calling Command 39 (Pause Media) with CommandGroup_Parameters from 18045
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 39 ")
	}
	StyledText {
		x: 1440
		y: 540
		height: 240
		width: 180
		textLabel: "Pause"
	}
	ButtonSq {
		id: button5915_1627
		x: 1440
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/roundblack.png"
		buttontext: "Pause"
		clickHandler.onClicked: onActivate5915()
	}
	function onActivate5916() 
	{
	 // Calling Command 952 (Game Reset) with CommandGroup_Parameters from 18047
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 952 ")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "Reset"
	}
	ButtonSq {
		id: button5916_1628
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/roundblack.png"
		buttontext: "Reset"
		clickHandler.onClicked: onActivate5916()
	}
	StyledText {
		x: 729
		y: 7
		height: 208
		width: 346
		textLabel: "<%=NP%>"
	}
	StyledText {
		x: 720
		y: 169
		height: 164
		width: 360
		textLabel: "<%=NP_SEC%>"
	}
	StyledText {
		x: 720
		y: 292
		height: 90
		width: 271
		textLabel: "<%=NP_TIME_SHORT%>"
	}
	StyledText {
		x: 1081
		y: 292
		height: 90
		width: 89
		textLabel: "<%=NP_SPEED%>"
	}
	ButtonSq {
		id: button2455_1629
		x: 720
		y: 0
		width: 480
		height: 360
		buttonbackground: "skins/Basic/532"
		buttontext: "<%=NP_SPEED%>"
		clickHandler.onClicked: onActivate2455()
	}
	function onActivate2441() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14732
		manager.sendDceMessage(srouterip + " "+deviceid + " -104 1 184 76 \"+10\" ")
	}
	ButtonSq {
		id: button2441_1630
		x: 720
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2441()
	}
	function onActivate2443() 
	{
	 // Calling Command 408 (Show Floorplan) with CommandGroup_Parameters from 15042
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 408 11 \"100\" 12 \"100\" 14 \"light\" ")
	}
	StyledText {
		x: 720
		y: 540
		height: 5
		width: 3
		textLabel: "Lights"
	}
	ButtonSq {
		id: button2443_1631
		x: 720
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_map.png"
		buttontext: "Lights"
		clickHandler.onClicked: onActivate2443()
	}
	function onActivate2442() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14733
		manager.sendDceMessage(srouterip + " "+deviceid + " -104 1 184 76 \"-10\" ")
	}
	ButtonSq {
		id: button2442_1632
		x: 720
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2442()
	}
	StyledText {
		x: 0
		y: 270
		height: 120
		width: 270
		textLabel: "P1"
	}
	ButtonSq {
		id: button5925_1633
		x: 0
		y: 270
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbuttons.png"
		buttontext: "P1"
		clickHandler.onClicked: onActivate5925()
	}
	StyledText {
		x: 360
		y: 270
		height: 120
		width: 270
		textLabel: "P2"
	}
	ButtonSq {
		id: button5926_1634
		x: 360
		y: 270
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "P2"
		clickHandler.onClicked: onActivate5926()
	}
	function onActivate5928() 
	{
	 // Calling Command 5 (Goto DesignObj) with CommandGroup_Parameters from 18092
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 5 3 \"5964\" ")
	}
	StyledText {
		x: 0
		y: 0
		height: 240
		width: 180
		textLabel: "See playlist"
	}
	ButtonSq {
		id: button5928_1635
		x: 0
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "See playlist"
		clickHandler.onClicked: onActivate5928()
	}
	function onActivate5876() 
	{
	 // Calling Command 87 (Goto Media Menu) with CommandGroup_Parameters from 18228
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 87 64 \"2\" ")
	}
	StyledText {
		x: 240
		y: 0
		height: 240
		width: 180
		textLabel: "Options"
	}
	ButtonSq {
		id: button5876_1636
		x: 240
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Options"
		clickHandler.onClicked: onActivate5876()
	}
	function onActivate1566() 
	{
	 // Calling Command 102 (Record) with CommandGroup_Parameters from 13636
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 102 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18381
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	ButtonSq {
		id: button1566_1637
		x: 1440
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/record.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1566()
	}
}