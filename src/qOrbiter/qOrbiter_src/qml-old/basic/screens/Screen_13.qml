// UI1 Screen SecurityPanel (13)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Security/panelC.png"
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
		id: button1785_160
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
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
		id: button1326_161
		x: 180
		y: 291
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
		id: button1327_162
		x: 420
		y: 291
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
		id: button1328_163
		x: 660
		y: 291
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
		id: button1329_164
		x: 180
		y: 471
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
		id: button1330_165
		x: 420
		y: 471
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
		id: button1331_166
		x: 660
		y: 471
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
		id: button1332_167
		x: 180
		y: 651
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
		id: button1333_168
		x: 420
		y: 651
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
		id: button1334_169
		x: 660
		y: 651
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
		id: button1335_170
		x: 420
		y: 831
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/0.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1335()
	}
	function onActivate1929() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13938
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"112\" ")
	}
	ButtonSq {
		id: button1929_171
		x: 180
		y: 831
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/clear.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1929()
	}
	function onActivate1928() 
	{
	 // Calling Command 408 (Show Floorplan) with CommandGroup_Parameters from 15043
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 408 11 \"100\" 12 \"100\" 14 \"security\" ")
	}
	StyledText {
		x: 660
		y: 831
		height: 240
		width: 180
		textLabel: "Map"
	}
	ButtonSq {
		id: button1928_172
		x: 660
		y: 831
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Map"
		clickHandler.onClicked: onActivate1928()
	}
	function onActivate3271() 
	{
	 // Calling Command 19 (Set House Mode) with CommandGroup_Parameters from 14071
		manager.sendDceMessage(srouterip + " "+deviceid + " -108 1 19 5 \"<%=25%>\" 17 \"<%=U%>\" 99 \"<%=24%>\" 100 \"<%=32%>\" 101 \"R\" ")
	}
	function onActivate1396() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14065
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"25\" 5 \"1\" ")
	}
	StyledText {
		x: 1272
		y: 0
		height: 207
		width: 447
		textLabel: "Disarmed"
	}
	ButtonSq {
		id: button1396_173
		x: 1083
		y: 0
		width: 595
		height: 155
		buttonbackground: "skins/Basic/Buttons/Security/mode.png"
		buttontext: "Disarmed"
		clickHandler.onClicked: onActivate1396()
	}
	ButtonSq {
		id: button1390_174
		x: 2174
		y: 17
		width: 0
		height: 0
		buttonbackground: "skins/Basic/Icons/modes/housedisarmed.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1390()
	}
	function onActivate1398() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14066
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"25\" 5 \"2\" ")
	}
	StyledText {
		x: 1272
		y: 155
		height: 207
		width: 447
		textLabel: "Armed"
	}
	ButtonSq {
		id: button1398_175
		x: 1083
		y: 155
		width: 595
		height: 155
		buttonbackground: "skins/Basic/Buttons/Security/mode.png"
		buttontext: "Armed"
		clickHandler.onClicked: onActivate1398()
	}
	ButtonSq {
		id: button1392_176
		x: 2174
		y: 172
		width: 0
		height: 0
		buttonbackground: "skins/Basic/Icons/modes/housesecure.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1392()
	}
	function onActivate1400() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14067
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"25\" 5 \"3\" ")
	}
	StyledText {
		x: 1272
		y: 310
		height: 207
		width: 447
		textLabel: "At Home Secure"
	}
	ButtonSq {
		id: button1400_177
		x: 1083
		y: 310
		width: 595
		height: 155
		buttonbackground: "skins/Basic/Buttons/Security/mode.png"
		buttontext: "At Home Secure"
		clickHandler.onClicked: onActivate1400()
	}
	ButtonSq {
		id: button1393_178
		x: 2174
		y: 327
		width: 0
		height: 0
		buttonbackground: "skins/Basic/Icons/modes/athomesecure.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1393()
	}
	function onActivate1397() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14068
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"25\" 5 \"4\" ")
	}
	StyledText {
		x: 1272
		y: 465
		height: 207
		width: 447
		textLabel: "Sleeping"
	}
	ButtonSq {
		id: button1397_179
		x: 1083
		y: 465
		width: 595
		height: 155
		buttonbackground: "skins/Basic/Buttons/Security/mode.png"
		buttontext: "Sleeping"
		clickHandler.onClicked: onActivate1397()
	}
	ButtonSq {
		id: button1391_180
		x: 2174
		y: 482
		width: 0
		height: 0
		buttonbackground: "skins/Basic/Icons/modes/housesleeping.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1391()
	}
	function onActivate1401() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14069
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"25\" 5 \"6\" ")
	}
	StyledText {
		x: 1272
		y: 620
		height: 207
		width: 447
		textLabel: "Extended Away"
	}
	ButtonSq {
		id: button1401_181
		x: 1083
		y: 620
		width: 595
		height: 155
		buttonbackground: "skins/Basic/Buttons/Security/mode.png"
		buttontext: "Extended Away"
		clickHandler.onClicked: onActivate1401()
	}
	function onActivate1395() 
	{
	}
	ButtonSq {
		id: button1395_182
		x: 2174
		y: 637
		width: 0
		height: 0
		buttonbackground: "skins/Basic/Icons/modes/extendedaway.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1395()
	}
	function onActivate1399() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14070
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"25\" 5 \"5\" ")
	}
	StyledText {
		x: 1272
		y: 775
		height: 207
		width: 447
		textLabel: "Entertaining"
	}
	ButtonSq {
		id: button1399_183
		x: 1083
		y: 775
		width: 595
		height: 155
		buttonbackground: "skins/Basic/Buttons/Security/mode.png"
		buttontext: "Entertaining"
		clickHandler.onClicked: onActivate1399()
	}
	ButtonSq {
		id: button1394_184
		x: 2174
		y: 792
		width: 0
		height: 0
		buttonbackground: "skins/Basic/Icons/modes/entertain.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1394()
	}
	function onActivate3401() 
	{
	 // Calling Command 19 (Set House Mode) with CommandGroup_Parameters from 14798
		manager.sendDceMessage(srouterip + " "+deviceid + " -108 1 19 5 \"-1\" 17 \"<%=U%>\" 99 \"<%=24%>\" ")
	}
	ButtonSq {
		id: button3401_185
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Security/monitoroff.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3401()
	}
}