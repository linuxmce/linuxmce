// UI1 Screen Game Atari 5200 FS Options (156)
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
		id: button1785_1652
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
		id: button1569_1653
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
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
		id: button2458_1654
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "file
list"
		clickHandler.onClicked: onActivate2458()
	}
	function onActivate3558() 
	{
	}
	ButtonSq {
		id: button5911_1655
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
		id: button3235_1656
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
		id: button3236_1657
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
		id: button3237_1658
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
		id: button3238_1659
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
		id: button3239_1660
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
		id: button3240_1661
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
		id: button3241_1662
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
		id: button3242_1663
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
		id: button3243_1664
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
		id: button3234_1665
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
		id: button5913_1666
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
		id: button5912_1667
		x: 480
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "#"
		clickHandler.onClicked: onActivate5912()
	}
	function onActivate5914() 
	{
	 // Calling Command 943 (Game 1P Start) with CommandGroup_Parameters from 18043
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 943 ")
	}
	StyledText {
		x: 0
		y: 180
		height: 240
		width: 180
		textLabel: "Start"
	}
	ButtonSq {
		id: button5914_1668
		x: 0
		y: 180
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
		x: 240
		y: 180
		height: 240
		width: 180
		textLabel: "Pause"
	}
	ButtonSq {
		id: button5915_1669
		x: 240
		y: 180
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
		x: 480
		y: 180
		height: 240
		width: 180
		textLabel: "Reset"
	}
	ButtonSq {
		id: button5916_1670
		x: 480
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/roundblack.png"
		buttontext: "Reset"
		clickHandler.onClicked: onActivate5916()
	}
	function onActivate2289() 
	{
	 // Calling Command 97 (Mute) with CommandGroup_Parameters from 13657
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 97 ")
	}
	StyledText {
		x: 1200
		y: 720
		height: 240
		width: 180
		textLabel: "Mute"
	}
	ButtonSq {
		id: button2289_1671
		x: 1200
		y: 720
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
		id: button1850_1672
		x: 1200
		y: 540
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
		id: button1851_1673
		x: 1200
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1851()
	}
	function onActivate2441() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14732
		manager.sendDceMessage(srouterip + " "+deviceid + " -104 1 184 76 \"+10\" ")
	}
	ButtonSq {
		id: button2441_1674
		x: 960
		y: 540
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
		x: 960
		y: 720
		height: 5
		width: 3
		textLabel: "Lights"
	}
	ButtonSq {
		id: button2443_1675
		x: 960
		y: 720
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
		id: button2442_1676
		x: 960
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2442()
	}
	function onActivate5623() 
	{
	 // Calling Command 87 (Goto Media Menu) with CommandGroup_Parameters from 17666
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 87 64 \"1\" ")
	}
	StyledText {
		x: 1680
		y: 360
		height: 240
		width: 180
		textLabel: "Monitor Mode"
	}
	ButtonSq {
		id: button5623_1677
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Monitor Mode"
		clickHandler.onClicked: onActivate5623()
	}
	function onActivate2028() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 9488
		setCurrentScreen("Screen_52.qml")
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "Resend
A/V
codes"
	}
	ButtonSq {
		id: button2028_1678
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Resend
A/V
codes"
		clickHandler.onClicked: onActivate2028()
	}
	function onActivate3547() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 15050
		setCurrentScreen("Screen_87.qml")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "Book
marks"
	}
	ButtonSq {
		id: button3547_1679
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Book
marks"
		clickHandler.onClicked: onActivate3547()
	}
	function onActivate5876() 
	{
	 // Calling Command 87 (Goto Media Menu) with CommandGroup_Parameters from 18228
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 87 64 \"2\" ")
	}
	StyledText {
		x: 0
		y: 0
		height: 240
		width: 270
		textLabel: "Options"
	}
	ButtonSq {
		id: button5876_1680
		x: 0
		y: 0
		width: 360
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Options"
		clickHandler.onClicked: onActivate5876()
	}
	function onActivate3285() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14143
		setCurrentScreen("Screen_20.qml")
	}
	StyledText {
		x: 360
		y: 0
		height: 240
		width: 270
		textLabel: "Manage
playlist"
	}
	ButtonSq {
		id: button3285_1681
		x: 360
		y: 0
		width: 360
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Manage
playlist"
		clickHandler.onClicked: onActivate3285()
	}
}