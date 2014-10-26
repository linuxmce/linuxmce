// UI1 Screen VDR Remote (117)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/PVR/VDRremote2.png"
		width: 1920
		height: 1080
	}
	function onActivate4494() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16258
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"Record\" ")
	}
	ButtonSq {
		id: button4494_1340
		x: 960
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/record.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4494()
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
		id: button1785_1341
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate4504() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16267
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"Red\" ")
	}
	StyledText {
		x: 0
		y: 900
		height: 240
		width: 180
		textLabel: "Red"
	}
	ButtonSq {
		id: button4504_1342
		x: 0
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "Red
"
		clickHandler.onClicked: onActivate4504()
	}
	function onActivate4505() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16268
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"Blue\" ")
	}
	StyledText {
		x: 720
		y: 900
		height: 240
		width: 180
		textLabel: "Blue"
	}
	ButtonSq {
		id: button4505_1343
		x: 720
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "Blue
"
		clickHandler.onClicked: onActivate4505()
	}
	function onActivate4506() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16269
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"Green\" ")
	}
	StyledText {
		x: 240
		y: 900
		height: 240
		width: 180
		textLabel: "Green"
	}
	ButtonSq {
		id: button4506_1344
		x: 240
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "Green
"
		clickHandler.onClicked: onActivate4506()
	}
	function onActivate4507() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16270
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"Yellow\" ")
	}
	StyledText {
		x: 480
		y: 900
		height: 240
		width: 180
		textLabel: "Yellow"
	}
	ButtonSq {
		id: button4507_1345
		x: 480
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "Yellow
"
		clickHandler.onClicked: onActivate4507()
	}
	function onActivate4508() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16271
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"Back\" ")
	}
	StyledText {
		x: 1200
		y: 180
		height: 240
		width: 180
		textLabel: "Back"
	}
	ButtonSq {
		id: button4508_1346
		x: 1200
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "Back
"
		clickHandler.onClicked: onActivate4508()
	}
	function onActivate4509() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16272
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"Left\" ")
	}
	ButtonSq {
		id: button4509_1347
		x: 1200
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_move_left.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4509()
	}
	function onActivate4510() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16273
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"Right\" ")
	}
	ButtonSq {
		id: button4510_1348
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_move_right.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4510()
	}
	function onActivate4511() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16274
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"Up\" ")
	}
	ButtonSq {
		id: button4511_1349
		x: 1440
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_move_up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4511()
	}
	function onActivate4512() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16275
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"Down\" ")
	}
	ButtonSq {
		id: button4512_1350
		x: 1440
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_move_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4512()
	}
	function onActivate4513() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16276
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"Ok\" ")
	}
	StyledText {
		x: 1440
		y: 360
		height: 240
		width: 180
		textLabel: "OK"
	}
	ButtonSq {
		id: button4513_1351
		x: 1440
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/roundblack.png"
		buttontext: "OK"
		clickHandler.onClicked: onActivate4513()
	}
	function onActivate4528() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16290
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"Menu\" ")
	}
	StyledText {
		x: 1680
		y: 180
		height: 240
		width: 180
		textLabel: "Menu"
	}
	ButtonSq {
		id: button4528_1352
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tr.png"
		buttontext: "Menu"
		clickHandler.onClicked: onActivate4528()
	}
	function onActivate2567() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14691
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"16\" 50 \"\" ")
	}
	ButtonSq {
		id: button2567_1353
		x: 0
		y: 179
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/1.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2567()
	}
	function onActivate2568() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14692
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"17\" ")
	}
	ButtonSq {
		id: button2568_1354
		x: 240
		y: 179
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/2.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2568()
	}
	function onActivate2569() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14693
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"18\" ")
	}
	ButtonSq {
		id: button2569_1355
		x: 480
		y: 179
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/3.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2569()
	}
	function onActivate2570() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14694
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"19\" ")
	}
	ButtonSq {
		id: button2570_1356
		x: 0
		y: 359
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/4.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2570()
	}
	function onActivate2571() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14695
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"20\" ")
	}
	ButtonSq {
		id: button2571_1357
		x: 240
		y: 359
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/5.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2571()
	}
	function onActivate2572() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14696
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"21\" ")
	}
	ButtonSq {
		id: button2572_1358
		x: 480
		y: 359
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/6.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2572()
	}
	function onActivate2573() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14697
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"22\" ")
	}
	ButtonSq {
		id: button2573_1359
		x: 0
		y: 539
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/7.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2573()
	}
	function onActivate2574() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14698
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"23\" ")
	}
	ButtonSq {
		id: button2574_1360
		x: 240
		y: 539
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/8.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2574()
	}
	function onActivate2575() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14699
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"24\" ")
	}
	ButtonSq {
		id: button2575_1361
		x: 480
		y: 539
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/9.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2575()
	}
	function onActivate2566() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14700
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"25\" ")
	}
	ButtonSq {
		id: button2566_1362
		x: 240
		y: 719
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/0.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2566()
	}
	function onActivate2578() 
	{
	 // Calling Command 187 (Tune to channel) with CommandGroup_Parameters from 14701
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 187 68 \"<%=17%>\" ")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14715
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" ")
	}
	StyledText {
		x: 480
		y: 719
		height: 240
		width: 180
		textLabel: "enter"
	}
	ButtonSq {
		id: button2578_1363
		x: 480
		y: 719
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "enter"
		clickHandler.onClicked: onActivate2578()
	}
	function onActivate2579() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14702
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"112\" ")
	}
	ButtonSq {
		id: button2579_1364
		x: 0
		y: 719
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/clear.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2579()
	}
	function onActivate4499() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16262
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"Recordings\" ")
	}
	StyledText {
		x: 1200
		y: 720
		height: 240
		width: 180
		textLabel: "RECs"
	}
	ButtonSq {
		id: button4499_1365
		x: 1200
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "RECs"
		clickHandler.onClicked: onActivate4499()
	}
	function onActivate4500() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16263
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"Timers\" ")
	}
	StyledText {
		x: 1440
		y: 900
		height: 240
		width: 180
		textLabel: "Timers"
	}
	ButtonSq {
		id: button4500_1366
		x: 1440
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Timers
"
		clickHandler.onClicked: onActivate4500()
	}
	function onActivate4502() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16265
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"Schedule\" ")
	}
	StyledText {
		x: 1200
		y: 900
		height: 240
		width: 180
		textLabel: "Schedule"
	}
	ButtonSq {
		id: button4502_1367
		x: 1200
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Schedule
"
		clickHandler.onClicked: onActivate4502()
	}
	function onActivate4501() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16264
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"Channels\" ")
	}
	StyledText {
		x: 960
		y: 900
		height: 240
		width: 180
		textLabel: "Channels"
	}
	ButtonSq {
		id: button4501_1368
		x: 960
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_1.png"
		buttontext: "Channels
"
		clickHandler.onClicked: onActivate4501()
	}
	function onActivate1569() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13739
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18035
		setCurrentScreen("Screen_1.qml")
	}
	ButtonSq {
		id: button1569_1369
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
	function onActivate2289() 
	{
	 // Calling Command 97 (Mute) with CommandGroup_Parameters from 13657
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 97 ")
	}
	StyledText {
		x: 953
		y: 360
		height: 240
		width: 180
		textLabel: "Mute"
	}
	ButtonSq {
		id: button2289_1370
		x: 953
		y: 360
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
		id: button1850_1371
		x: 953
		y: 180
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
		id: button1851_1372
		x: 953
		y: 540
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
		id: button2441_1373
		x: 720
		y: 180
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
		y: 360
		height: 5
		width: 3
		textLabel: "Lights"
	}
	ButtonSq {
		id: button2443_1374
		x: 720
		y: 360
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
		id: button2442_1375
		x: 720
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2442()
	}
	function onActivate6061() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18226
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"9\" ")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "Next"
	}
	ButtonSq {
		id: button6061_1376
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "Next"
		clickHandler.onClicked: onActivate6061()
	}
	function onActivate6062() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18227
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 28 50 \"7\" ")
	}
	StyledText {
		x: 1200
		y: 540
		height: 240
		width: 180
		textLabel: "Previous"
	}
	ButtonSq {
		id: button6062_1377
		x: 1200
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "Previous"
		clickHandler.onClicked: onActivate6062()
	}
	function onActivate2598() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 12831
		setCurrentScreen("Screen_58.qml")
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "DVD Options"
	}
	ButtonSq {
		id: button2598_1378
		x: 1680
		y: 720
		width: 343
		height: 257
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "DVD Options"
		clickHandler.onClicked: onActivate2598()
	}
}