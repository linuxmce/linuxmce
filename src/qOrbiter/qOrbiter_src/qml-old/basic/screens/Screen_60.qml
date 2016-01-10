// UI1 Screen UserPinCode (60)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_698
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
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
		id: button1785_699
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate2395() 
	{
	 // Calling Command 16 (Requires Special Handling) with CommandGroup_Parameters from 14823
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 16 ")
	}
	StyledText {
		x: 758
		y: 866
		height: 240
		width: 180
		textLabel: "Go"
	}
	ButtonSq {
		id: button2395_700
		x: 758
		y: 866
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Go"
		clickHandler.onClicked: onActivate2395()
	}
	function onActivate1929() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13938
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"112\" ")
	}
	ButtonSq {
		id: button1929_701
		x: 271
		y: 866
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/clear.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1929()
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
		id: button1326_702
		x: 271
		y: 338
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
		id: button1327_703
		x: 511
		y: 338
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
		id: button1328_704
		x: 751
		y: 338
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
		id: button1329_705
		x: 271
		y: 518
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
		id: button1330_706
		x: 511
		y: 518
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
		id: button1331_707
		x: 751
		y: 518
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
		id: button1332_708
		x: 271
		y: 698
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
		id: button1333_709
		x: 511
		y: 698
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
		id: button1334_710
		x: 751
		y: 698
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
		id: button1335_711
		x: 511
		y: 878
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/0.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1335()
	}
}