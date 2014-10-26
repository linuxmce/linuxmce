// UI1 Screen MakeCallDialNumber (11)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/enternumberB.png"
		width: 1920
		height: 1080
	}
	function onActivate1337() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13926
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"112\" ")
	}
	ButtonSq {
		id: button1337_127
		x: 240
		y: 811
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/clear.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1337()
	}
	function onActivate1338() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2392
		setCurrentScreen("Screen_21.qml")
	}
	StyledText {
		x: 1200
		y: 811
		height: 240
		width: 180
		textLabel: "Call"
	}
	ButtonSq {
		id: button1338_128
		x: 1200
		y: 811
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Call"
		clickHandler.onClicked: onActivate1338()
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
		id: button1785_129
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate2229() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 9390
		setCurrentScreen("Screen_12.qml")
	}
	StyledText {
		x: 1440
		y: 811
		height: 240
		width: 180
		textLabel: "Call LinuxMCE User"
	}
	ButtonSq {
		id: button2229_130
		x: 1440
		y: 811
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Call LinuxMCE User"
		clickHandler.onClicked: onActivate2229()
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
		id: button1319_131
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
		id: button1316_132
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
		id: button1321_133
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
		id: button1682_134
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Call
Intercom"
		clickHandler.onClicked: onActivate1682()
	}
	function onActivate3563() 
	{
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 15123
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"1338\" 8 \"1\" ")
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
		id: button1326_135
		x: 965
		y: 541
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
		id: button1327_136
		x: 1205
		y: 541
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
		id: button1328_137
		x: 1445
		y: 541
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
		id: button1329_138
		x: 965
		y: 721
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
		id: button1330_139
		x: 1205
		y: 721
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
		id: button1331_140
		x: 1445
		y: 721
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
		id: button1332_141
		x: 965
		y: 901
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
		id: button1333_142
		x: 1205
		y: 901
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
		id: button1334_143
		x: 1445
		y: 901
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
		id: button1335_144
		x: 1205
		y: 1081
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/0.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1335()
	}
	function onActivate4261() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 15892
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"33\" ")
	}
	ButtonSq {
		id: button4261_145
		x: 963
		y: 811
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4261()
	}
	ButtonSq {
		id: button4264_146
		x: 1445
		y: 1081
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/pound.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4264()
	}
	function onActivate4262() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 15893
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"32\" ")
	}
	ButtonSq {
		id: button4262_147
		x: 483
		y: 811
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4262()
	}
	ButtonSq {
		id: button4263_148
		x: 965
		y: 1081
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/astrisk.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4263()
	}
	function onActivate5135() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 17110
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" ")
	 // Calling Command 25 (Set Text) with CommandGroup_Parameters from 17111
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 25 25 \"347\" ")
	}
	StyledText {
		x: 1200
		y: 271
		height: 240
		width: 180
		textLabel: "Clear"
	}
	ButtonSq {
		id: button5135_149
		x: 1200
		y: 271
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Clear"
		clickHandler.onClicked: onActivate5135()
	}
	function onActivate5534() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17518
		setCurrentScreen("Screen_267.qml")
	}
	StyledText {
		x: 1678
		y: 180
		height: 240
		width: 180
		textLabel: "Active Calls"
	}
	ButtonSq {
		id: button5534_150
		x: 1678
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Active Calls"
		clickHandler.onClicked: onActivate5534()
	}
}