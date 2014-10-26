// UI1 Screen AudiClimate (169)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankB.png"
		width: 1920
		height: 1080
	}
	ButtonSq {
		id: button6083_1948
		x: 0
		y: 0
		width: 1680
		height: 630
		buttonbackground: "skins/Basic/Media/blankC.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6083()
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
		id: button1785_1949
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
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
		id: button2458_1950
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "file
list"
		clickHandler.onClicked: onActivate2458()
	}
	function onActivate1569() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13739
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18035
		setCurrentScreen("Screen_1.qml")
	}
	ButtonSq {
		id: button1569_1951
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
	ButtonSq {
		id: button6192_1952
		x: 0
		y: 630
		width: 1920
		height: 450
		buttonbackground: "skins/Basic/2128"
		buttontext: ""
		clickHandler.onClicked: onActivate6192()
	}
	function onActivate6087() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18246
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"187\" ")
	}
	StyledText {
		x: 0
		y: 630
		height: 120
		width: 90
		textLabel: "Esc"
	}
	ButtonSq {
		id: button6087_1953
		x: 0
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Esc"
		clickHandler.onClicked: onActivate6087()
	}
	function onActivate6085() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18245
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"15\" ")
	}
	StyledText {
		x: 120
		y: 630
		height: 120
		width: 90
		textLabel: "!
1"
	}
	ButtonSq {
		id: button6085_1954
		x: 120
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "!
1"
		clickHandler.onClicked: onActivate6085()
	}
	function onActivate6181() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18326
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"17\" ")
	}
	StyledText {
		x: 240
		y: 630
		height: 120
		width: 90
		textLabel: ""

	}
	ButtonSq {
		id: button6181_1955
		x: 240
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""

		clickHandler.onClicked: onActivate6181()
	}
	function onActivate6089() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18248
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"17\" ")
	}
	StyledText {
		x: 360
		y: 630
		height: 120
		width: 90
		textLabel: "#
3"
	}
	ButtonSq {
		id: button6089_1956
		x: 360
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "#
3"
		clickHandler.onClicked: onActivate6089()
	}
	function onActivate6090() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18249
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"18\" ")
	}
	StyledText {
		x: 480
		y: 630
		height: 120
		width: 90
		textLabel: "$
4"
	}
	ButtonSq {
		id: button6090_1957
		x: 480
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "$
4"
		clickHandler.onClicked: onActivate6090()
	}
	function onActivate6091() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18250
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"20\" ")
	}
	StyledText {
		x: 599
		y: 630
		height: 120
		width: 90
		textLabel: "%
5"
	}
	ButtonSq {
		id: button6091_1958
		x: 599
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "%
5"
		clickHandler.onClicked: onActivate6091()
	}
	function onActivate6182() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18327
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"21\" ")
	}
	StyledText {
		x: 720
		y: 630
		height: 120
		width: 90
		textLabel: "&
6"
	}
	ButtonSq {
		id: button6182_1959
		x: 720
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "&
6"
		clickHandler.onClicked: onActivate6182()
	}
	function onActivate6183() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18328
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"22\" ")
	}
	StyledText {
		x: 840
		y: 630
		height: 120
		width: 90
		textLabel: "'
7"
	}
	ButtonSq {
		id: button6183_1960
		x: 840
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "'
7"
		clickHandler.onClicked: onActivate6183()
	}
	function onActivate6193() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18349
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"23\" ")
	}
	StyledText {
		x: 960
		y: 630
		height: 120
		width: 90
		textLabel: "@
8"
	}
	ButtonSq {
		id: button6193_1961
		x: 960
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "@
8"
		clickHandler.onClicked: onActivate6193()
	}
	function onActivate6095() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18254
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"24\" ")
	}
	StyledText {
		x: 1080
		y: 630
		height: 120
		width: 90
		textLabel: "(
9"
	}
	ButtonSq {
		id: button6095_1962
		x: 1080
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "(
9"
		clickHandler.onClicked: onActivate6095()
	}
	function onActivate6096() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18255
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"25\" ")
	}
	StyledText {
		x: 1200
		y: 630
		height: 120
		width: 90
		textLabel: ")
0"
	}
	ButtonSq {
		id: button6096_1963
		x: 1200
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ")
0"
		clickHandler.onClicked: onActivate6096()
	}
	function onActivate6194() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18350
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"244\" ")
	}
	StyledText {
		x: 1320
		y: 630
		height: 120
		width: 90
		textLabel: "CLR
<"
	}
	ButtonSq {
		id: button6194_1964
		x: 1320
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "CLR
<"
		clickHandler.onClicked: onActivate6194()
	}
	function onActivate6195() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18351
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"245\" ")
	}
	StyledText {
		x: 1440
		y: 630
		height: 120
		width: 90
		textLabel: "INS
>"
	}
	ButtonSq {
		id: button6195_1965
		x: 1440
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "INS
>"
		clickHandler.onClicked: onActivate6195()
	}
	function onActivate6196() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18352
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"112\" ")
	}
	StyledText {
		x: 1559
		y: 630
		height: 120
		width: 90
		textLabel: "Del"
	}
	ButtonSq {
		id: button6196_1966
		x: 1559
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Del"
		clickHandler.onClicked: onActivate6196()
	}
	function onActivate6197() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18353
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"246\" ")
	}
	StyledText {
		x: 1680
		y: 630
		height: 120
		width: 90
		textLabel: "Brk"
	}
	ButtonSq {
		id: button6197_1967
		x: 1680
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Brk"
		clickHandler.onClicked: onActivate6197()
	}
	function onActivate6099() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18259
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"188\" ")
	}
	StyledText {
		x: 0
		y: 720
		height: 120
		width: 135
		textLabel: "Tab"
	}
	ButtonSq {
		id: button6099_1968
		x: 0
		y: 720
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Tab"
		clickHandler.onClicked: onActivate6099()
	}
	function onActivate6100() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18260
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"129\" ")
	}
	StyledText {
		x: 180
		y: 720
		height: 120
		width: 90
		textLabel: "Q"
	}
	ButtonSq {
		id: button6100_1969
		x: 180
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Q"
		clickHandler.onClicked: onActivate6100()
	}
	function onActivate6101() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18261
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"135\" ")
	}
	StyledText {
		x: 300
		y: 720
		height: 120
		width: 90
		textLabel: "W"
	}
	ButtonSq {
		id: button6101_1970
		x: 300
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "W"
		clickHandler.onClicked: onActivate6101()
	}
	function onActivate6102() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18262
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"117\" ")
	}
	StyledText {
		x: 420
		y: 720
		height: 120
		width: 90
		textLabel: "E"
	}
	ButtonSq {
		id: button6102_1971
		x: 420
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "E"
		clickHandler.onClicked: onActivate6102()
	}
	function onActivate6103() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18263
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"130\" ")
	}
	StyledText {
		x: 540
		y: 720
		height: 120
		width: 90
		textLabel: "R"
	}
	ButtonSq {
		id: button6103_1972
		x: 540
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "R"
		clickHandler.onClicked: onActivate6103()
	}
	function onActivate6104() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18264
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"132\" ")
	}
	StyledText {
		x: 660
		y: 720
		height: 120
		width: 90
		textLabel: "T"
	}
	ButtonSq {
		id: button6104_1973
		x: 660
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "T"
		clickHandler.onClicked: onActivate6104()
	}
	function onActivate6105() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18265
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"137\" ")
	}
	StyledText {
		x: 780
		y: 720
		height: 120
		width: 90
		textLabel: "Y"
	}
	ButtonSq {
		id: button6105_1974
		x: 780
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Y"
		clickHandler.onClicked: onActivate6105()
	}
	function onActivate6106() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18266
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"133\" ")
	}
	StyledText {
		x: 900
		y: 720
		height: 120
		width: 90
		textLabel: "U"
	}
	ButtonSq {
		id: button6106_1975
		x: 900
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "U"
		clickHandler.onClicked: onActivate6106()
	}
	function onActivate6107() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18267
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"121\" ")
	}
	StyledText {
		x: 1020
		y: 720
		height: 120
		width: 90
		textLabel: "I"
	}
	ButtonSq {
		id: button6107_1976
		x: 1020
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "I"
		clickHandler.onClicked: onActivate6107()
	}
	function onActivate6108() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18268
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"127\" ")
	}
	StyledText {
		x: 1140
		y: 720
		height: 120
		width: 90
		textLabel: "O"
	}
	ButtonSq {
		id: button6108_1977
		x: 1140
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "O"
		clickHandler.onClicked: onActivate6108()
	}
	function onActivate6109() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18269
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"128\" ")
	}
	StyledText {
		x: 1260
		y: 720
		height: 120
		width: 90
		textLabel: "P"
	}
	ButtonSq {
		id: button6109_1978
		x: 1260
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "P"
		clickHandler.onClicked: onActivate6109()
	}
	function onActivate6198() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18354
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 ")
	}
	StyledText {
		x: 1380
		y: 720
		height: 120
		width: 90
		textLabel: "U_
-"
	}
	ButtonSq {
		id: button6198_1979
		x: 1380
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "U_
-"
		clickHandler.onClicked: onActivate6198()
	}
	function onActivate6199() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18355
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"248\" ")
	}
	StyledText {
		x: 1500
		y: 720
		height: 120
		width: 90
		textLabel: "D|
="
	}
	ButtonSq {
		id: button6199_1980
		x: 1500
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "D|
="
		clickHandler.onClicked: onActivate6199()
	}
	function onActivate6121() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18285
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"5\" ")
	}
	StyledText {
		x: 1620
		y: 720
		height: 120
		width: 135
		textLabel: "Enter"
	}
	ButtonSq {
		id: button6121_1981
		x: 1620
		y: 720
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Enter"
		clickHandler.onClicked: onActivate6121()
	}
	function onActivate6072() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18273
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"225\" ")
	}
	StyledText {
		x: 0
		y: 810
		height: 120
		width: 180
		textLabel: "Ctrl"
	}
	ButtonSq {
		id: button6072_1982
		x: 0
		y: 810
		width: 240
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Ctrl"
		clickHandler.onClicked: onActivate6072()
	}
	function onActivate6110() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18274
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"113\" ")
	}
	StyledText {
		x: 240
		y: 810
		height: 120
		width: 90
		textLabel: "A"
	}
	ButtonSq {
		id: button6110_1983
		x: 240
		y: 810
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "A"
		clickHandler.onClicked: onActivate6110()
	}
	function onActivate6111() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18275
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"131\" ")
	}
	StyledText {
		x: 360
		y: 810
		height: 120
		width: 90
		textLabel: "S"
	}
	ButtonSq {
		id: button6111_1984
		x: 360
		y: 810
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "S"
		clickHandler.onClicked: onActivate6111()
	}
	function onActivate6112() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18276
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"116\" ")
	}
	StyledText {
		x: 480
		y: 810
		height: 120
		width: 90
		textLabel: "D"
	}
	ButtonSq {
		id: button6112_1985
		x: 480
		y: 810
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "D"
		clickHandler.onClicked: onActivate6112()
	}
	function onActivate6113() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18277
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"118\" ")
	}
	StyledText {
		x: 599
		y: 810
		height: 120
		width: 90
		textLabel: "F"
	}
	ButtonSq {
		id: button6113_1986
		x: 599
		y: 810
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "F"
		clickHandler.onClicked: onActivate6113()
	}
	function onActivate6114() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18278
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"119\" ")
	}
	StyledText {
		x: 720
		y: 810
		height: 120
		width: 90
		textLabel: "G"
	}
	ButtonSq {
		id: button6114_1987
		x: 720
		y: 810
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "G"
		clickHandler.onClicked: onActivate6114()
	}
	function onActivate6115() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18279
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"120\" ")
	}
	StyledText {
		x: 840
		y: 810
		height: 120
		width: 90
		textLabel: "H"
	}
	ButtonSq {
		id: button6115_1988
		x: 840
		y: 810
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "H"
		clickHandler.onClicked: onActivate6115()
	}
	function onActivate6116() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18280
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"122\" ")
	}
	StyledText {
		x: 960
		y: 810
		height: 120
		width: 90
		textLabel: "J"
	}
	ButtonSq {
		id: button6116_1989
		x: 960
		y: 810
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "J"
		clickHandler.onClicked: onActivate6116()
	}
	function onActivate6117() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18281
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"123\" ")
	}
	StyledText {
		x: 1080
		y: 810
		height: 120
		width: 90
		textLabel: "K"
	}
	ButtonSq {
		id: button6117_1990
		x: 1080
		y: 810
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "K"
		clickHandler.onClicked: onActivate6117()
	}
	function onActivate6118() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18282
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"124\" ")
	}
	StyledText {
		x: 1200
		y: 810
		height: 120
		width: 90
		textLabel: "L"
	}
	ButtonSq {
		id: button6118_1991
		x: 1200
		y: 810
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "L"
		clickHandler.onClicked: onActivate6118()
	}
	function onActivate6119() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18283
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"184\" ")
	}
	StyledText {
		x: 1320
		y: 810
		height: 120
		width: 90
		textLabel: ":
;"
	}
	ButtonSq {
		id: button6119_1992
		x: 1320
		y: 810
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ":
;"
		clickHandler.onClicked: onActivate6119()
	}
	function onActivate6200() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18356
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"249\" ")
	}
	StyledText {
		x: 1440
		y: 810
		height: 120
		width: 90
		textLabel: "L\
+"
	}
	ButtonSq {
		id: button6200_1993
		x: 1440
		y: 810
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "L\
+"
		clickHandler.onClicked: onActivate6200()
	}
	function onActivate6201() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18357
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"250\" ")
	}
	StyledText {
		x: 1560
		y: 810
		height: 120
		width: 90
		textLabel: "R^
*"
	}
	ButtonSq {
		id: button6201_1994
		x: 1560
		y: 810
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "R^
*"
		clickHandler.onClicked: onActivate6201()
	}
	function onActivate6130() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18296
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"170\" ")
	}
	StyledText {
		x: 1680
		y: 810
		height: 120
		width: 90
		textLabel: "cap
lock"
	}
	ButtonSq {
		id: button6130_1995
		x: 1680
		y: 810
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "cap
lock"
		clickHandler.onClicked: onActivate6130()
	}
	function onActivate6122() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18286
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"226\" ")
	}
	StyledText {
		x: 0
		y: 900
		height: 120
		width: 225
		textLabel: "Shift"
	}
	ButtonSq {
		id: button6122_1996
		x: 0
		y: 900
		width: 300
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Shift"
		clickHandler.onClicked: onActivate6122()
	}
	function onActivate6123() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18287
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"138\" ")
	}
	StyledText {
		x: 300
		y: 900
		height: 120
		width: 90
		textLabel: "Z"
	}
	ButtonSq {
		id: button6123_1997
		x: 300
		y: 900
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Z"
		clickHandler.onClicked: onActivate6123()
	}
	function onActivate6124() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18288
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"136\" ")
	}
	StyledText {
		x: 420
		y: 900
		height: 120
		width: 90
		textLabel: "X"
	}
	ButtonSq {
		id: button6124_1998
		x: 420
		y: 900
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "X"
		clickHandler.onClicked: onActivate6124()
	}
	function onActivate6125() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18289
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"115\" ")
	}
	StyledText {
		x: 540
		y: 900
		height: 120
		width: 90
		textLabel: "C"
	}
	ButtonSq {
		id: button6125_1999
		x: 540
		y: 900
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "C"
		clickHandler.onClicked: onActivate6125()
	}
	function onActivate6126() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18346
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"134\" ")
	}
	StyledText {
		x: 660
		y: 900
		height: 120
		width: 90
		textLabel: "V"
	}
	ButtonSq {
		id: button6126_2000
		x: 660
		y: 900
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "V"
		clickHandler.onClicked: onActivate6126()
	}
	function onActivate6127() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18290
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"114\" ")
	}
	StyledText {
		x: 780
		y: 900
		height: 120
		width: 90
		textLabel: "B"
	}
	ButtonSq {
		id: button6127_2001
		x: 780
		y: 900
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "B"
		clickHandler.onClicked: onActivate6127()
	}
	function onActivate6128() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18291
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"126\" ")
	}
	StyledText {
		x: 900
		y: 900
		height: 120
		width: 90
		textLabel: "N"
	}
	ButtonSq {
		id: button6128_2002
		x: 900
		y: 900
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "N"
		clickHandler.onClicked: onActivate6128()
	}
	function onActivate6129() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18292
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"125\" ")
	}
	StyledText {
		x: 1020
		y: 900
		height: 120
		width: 90
		textLabel: "M"
	}
	ButtonSq {
		id: button6129_2003
		x: 1020
		y: 900
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "M"
		clickHandler.onClicked: onActivate6129()
	}
	function onActivate6202() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18358
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"166\" ")
	}
	StyledText {
		x: 1140
		y: 900
		height: 120
		width: 90
		textLabel: "[
,"
	}
	ButtonSq {
		id: button6202_2004
		x: 1140
		y: 900
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "[
,"
		clickHandler.onClicked: onActivate6202()
	}
	function onActivate6203() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18359
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"167\" ")
	}
	StyledText {
		x: 1260
		y: 900
		height: 120
		width: 90
		textLabel: "]
."
	}
	ButtonSq {
		id: button6203_2005
		x: 1260
		y: 900
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "]
."
		clickHandler.onClicked: onActivate6203()
	}
	function onActivate6075() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18295
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"181\" ")
	}
	StyledText {
		x: 1380
		y: 900
		height: 120
		width: 90
		textLabel: "?
/"
	}
	ButtonSq {
		id: button6075_2006
		x: 1380
		y: 900
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "?
/"
		clickHandler.onClicked: onActivate6075()
	}
	function onActivate6204() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18360
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"251\" ")
	}
	ButtonSq {
		id: button6204_2007
		x: 1500
		y: 900
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6204()
	}
	ButtonSq {
		id: button6205_2008
		x: 1500
		y: 1530
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/atari_logo.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6205()
	}
	function onActivate6122() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18286
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"226\" ")
	}
	StyledText {
		x: 1620
		y: 900
		height: 120
		width: 135
		textLabel: "Shift"
	}
	ButtonSq {
		id: button6122_2009
		x: 1620
		y: 900
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Shift"
		clickHandler.onClicked: onActivate6122()
	}
	function onActivate6133() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18299
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"165\" ")
	}
	StyledText {
		x: 359
		y: 990
		height: 120
		width: 809
		textLabel: "Space"
	}
	ButtonSq {
		id: button6133_2010
		x: 359
		y: 990
		width: 1079
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Space"
		clickHandler.onClicked: onActivate6133()
	}
	StyledText {
		x: 9
		y: 7
		height: 208
		width: 346
		textLabel: "<%=NP%>"
	}
	StyledText {
		x: 0
		y: 169
		height: 164
		width: 360
		textLabel: "<%=NP_SEC%>"
	}
	StyledText {
		x: 0
		y: 292
		height: 90
		width: 271
		textLabel: "<%=NP_TIME_SHORT%>"
	}
	StyledText {
		x: 361
		y: 292
		height: 90
		width: 89
		textLabel: "<%=NP_SPEED%>"
	}
	ButtonSq {
		id: button2455_2011
		x: 0
		y: 0
		width: 480
		height: 360
		buttonbackground: "skins/Basic/532"
		buttontext: "<%=NP_SPEED%>"
		clickHandler.onClicked: onActivate2455()
	}
	function onActivate3285() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14143
		setCurrentScreen("Screen_20.qml")
	}
	StyledText {
		x: 0
		y: 406
		height: 240
		width: 180
		textLabel: "Manage
playlist"
	}
	ButtonSq {
		id: button3285_2012
		x: 0
		y: 406
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Manage
playlist"
		clickHandler.onClicked: onActivate3285()
	}
	function onActivate3547() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 15050
		setCurrentScreen("Screen_87.qml")
	}
	StyledText {
		x: 240
		y: 406
		height: 240
		width: 180
		textLabel: "Book
marks"
	}
	ButtonSq {
		id: button3547_2013
		x: 240
		y: 406
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Book
marks"
		clickHandler.onClicked: onActivate3547()
	}
	function onActivate3351() 
	{
	 // Calling Command 29 (Simulate Mouse Click) with CommandGroup_Parameters from 14282
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 29 11 \"<%=XS%>\" 12 \"<%=YS%>\" ")
	}
	ButtonSq {
		id: button3351_2014
		x: 717
		y: 22
		width: 934
		height: 587
		buttonbackground: "skins/Basic/<%=NPD%>"
		buttontext: ""
		clickHandler.onClicked: onActivate3351()
	}
	function onActivate6208() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18361
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"252\" ")
	}
	StyledText {
		x: 509
		y: 22
		height: 120
		width: 135
		textLabel: "Reset"
	}
	ButtonSq {
		id: button6208_2015
		x: 509
		y: 22
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Reset"
		clickHandler.onClicked: onActivate6208()
	}
	function onActivate6209() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18362
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"253\" ")
	}
	StyledText {
		x: 509
		y: 112
		height: 120
		width: 135
		textLabel: "Option"
	}
	ButtonSq {
		id: button6209_2016
		x: 509
		y: 112
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Option"
		clickHandler.onClicked: onActivate6209()
	}
	function onActivate6210() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18363
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"254\" ")
	}
	StyledText {
		x: 505
		y: 202
		height: 120
		width: 135
		textLabel: "Select"
	}
	ButtonSq {
		id: button6210_2017
		x: 505
		y: 202
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Select"
		clickHandler.onClicked: onActivate6210()
	}
	function onActivate6211() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18364
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"255\" ")
	}
	StyledText {
		x: 509
		y: 292
		height: 120
		width: 135
		textLabel: "Start"
	}
	ButtonSq {
		id: button6211_2018
		x: 509
		y: 292
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Start"
		clickHandler.onClicked: onActivate6211()
	}
}
