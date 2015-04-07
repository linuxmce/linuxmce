// UI1 Screen TransferOutside (62)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/enternumberB.png"
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
		id: button1785_719
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate1337() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13926
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"112\" ")
	}
	ButtonSq {
		id: button1337_720
		x: 240
		y: 812
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/clear.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1337()
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
		id: button1319_721
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
		id: button1316_722
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
		id: button1321_723
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
		id: button1682_724
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Call
Intercom"
		clickHandler.onClicked: onActivate1682()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_725
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate4262() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 15893
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"32\" ")
	}
	ButtonSq {
		id: button4262_726
		x: 480
		y: 812
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4262()
	}
	ButtonSq {
		id: button4263_727
		x: 480
		y: 812
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/astrisk.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4263()
	}
	function onActivate4261() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 15892
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"33\" ")
	}
	ButtonSq {
		id: button4261_728
		x: 960
		y: 812
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4261()
	}
	ButtonSq {
		id: button4264_729
		x: 960
		y: 812
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/pound.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4264()
	}
	function onActivate1336() 
	{
	}
	function onActivate1326() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14055
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"16\" ")
	}
	ButtonSq {
		id: button1326_730
		x: 483
		y: 271
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/1.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1326()
	}
	function onActivate1327() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13893
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"17\" ")
	}
	ButtonSq {
		id: button1327_731
		x: 723
		y: 271
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/2.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1327()
	}
	function onActivate1328() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13894
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"18\" ")
	}
	ButtonSq {
		id: button1328_732
		x: 963
		y: 271
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/3.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1328()
	}
	function onActivate1329() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13895
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"19\" ")
	}
	ButtonSq {
		id: button1329_733
		x: 483
		y: 451
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/4.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1329()
	}
	function onActivate1330() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13896
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"20\" ")
	}
	ButtonSq {
		id: button1330_734
		x: 723
		y: 451
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/5.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1330()
	}
	function onActivate1331() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13897
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"21\" ")
	}
	ButtonSq {
		id: button1331_735
		x: 963
		y: 451
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/6.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1331()
	}
	function onActivate1332() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13898
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"22\" ")
	}
	ButtonSq {
		id: button1332_736
		x: 483
		y: 631
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/7.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1332()
	}
	function onActivate1333() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13899
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"23\" ")
	}
	ButtonSq {
		id: button1333_737
		x: 723
		y: 631
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/8.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1333()
	}
	function onActivate1334() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13900
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"24\" ")
	}
	ButtonSq {
		id: button1334_738
		x: 963
		y: 631
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/9.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1334()
	}
	function onActivate1335() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13901
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"25\" ")
	}
	ButtonSq {
		id: button1335_739
		x: 723
		y: 811
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/0.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1335()
	}
	function onActivate4820() 
	{
	 // Calling Command 234 (PL_Transfer) with CommandGroup_Parameters from 16775
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 234 83 \"<%=17%>\" 196 \"<%=12%>\" ")
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 17466
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	StyledText {
		x: 1200
		y: 812
		height: 240
		width: 180
		textLabel: "Call"
	}
	ButtonSq {
		id: button4820_740
		x: 1200
		y: 812
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Call"
		clickHandler.onClicked: onActivate4820()
	}
}