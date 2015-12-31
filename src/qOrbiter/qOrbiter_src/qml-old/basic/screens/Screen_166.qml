// UI1 Screen Apple 2 Remote (166)
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
		id: button6083_1789
		x: 0
		y: 0
		width: 1680
		height: 630
		buttonbackground: "skins/Basic/Media/blankC.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6083()
	}
	ButtonSq {
		id: button6066_1790
		x: 0
		y: 630
		width: 1920
		height: 450
		buttonbackground: "skins/Basic/2128"
		buttontext: ""
		clickHandler.onClicked: onActivate6066()
	}
	function onActivate6067() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18258
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"112\" ")
	}
	StyledText {
		x: 1560
		y: 630
		height: 120
		width: 135
		textLabel: "Delete"
	}
	ButtonSq {
		id: button6067_1791
		x: 1560
		y: 630
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Delete"
		clickHandler.onClicked: onActivate6067()
	}
	function onActivate6068() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18270
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"222\" ")
	}
	StyledText {
		x: 1380
		y: 720
		height: 120
		width: 90
		textLabel: "{
["
	}
	ButtonSq {
		id: button6068_1792
		x: 1380
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "{
["
		clickHandler.onClicked: onActivate6068()
	}
	function onActivate6070() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18271
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"223\" ")
	}
	StyledText {
		x: 1500
		y: 720
		height: 120
		width: 90
		textLabel: "}
]"
	}
	ButtonSq {
		id: button6070_1793
		x: 1500
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "}
]"
		clickHandler.onClicked: onActivate6070()
	}
	function onActivate6071() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18272
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"224\" ")
	}
	StyledText {
		x: 1620
		y: 720
		height: 120
		width: 90
        textLabel: "|"
	}
	ButtonSq {
		id: button6071_1794
		x: 1620
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
        buttontext: "|"
		clickHandler.onClicked: onActivate6071()
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
		id: button6072_1795
		x: 0
		y: 810
		width: 240
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Ctrl"
		clickHandler.onClicked: onActivate6072()
	}
	function onActivate6073() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18293
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"166\" ")
	}
	StyledText {
		x: 1110
		y: 900
		height: 120
		width: 90
		textLabel: "<
,"
	}
	ButtonSq {
		id: button6073_1796
		x: 1110
		y: 900
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "<
,"
		clickHandler.onClicked: onActivate6073()
	}
	function onActivate6074() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18294
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"167\" ")
	}
	StyledText {
		x: 1230
		y: 900
		height: 120
		width: 90
		textLabel: ">
."
	}
	ButtonSq {
		id: button6074_1797
		x: 1230
		y: 900
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ">
."
		clickHandler.onClicked: onActivate6074()
	}
	function onActivate6075() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18295
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"181\" ")
	}
	StyledText {
		x: 1350
		y: 900
		height: 120
		width: 90
		textLabel: "?
/"
	}
	ButtonSq {
		id: button6075_1798
		x: 1350
		y: 900
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "?
/"
		clickHandler.onClicked: onActivate6075()
	}
	function onActivate6076() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18297
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"228\" ")
	}
	StyledText {
		x: 120
		y: 990
		height: 120
		width: 90
		textLabel: "~
`"
	}
	ButtonSq {
		id: button6076_1799
		x: 120
		y: 990
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "~
`"
		clickHandler.onClicked: onActivate6076()
	}
	ButtonSq {
		id: button6077_1800
		x: 360
		y: 990
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6077()
	}
	ButtonSq {
		id: button6078_1801
		x: 360
		y: 1620
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/OpenApple.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6078()
	}
	function onActivate6080() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18300
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"230\" ")
	}
	ButtonSq {
		id: button6080_1802
		x: 1140
		y: 990
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6080()
	}
	ButtonSq {
		id: button6079_1803
		x: 1140
		y: 1620
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/ClosedApple.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6079()
	}
	function onActivate2520() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13856
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"165\" ")
	}
	StyledText {
		x: 480
		y: 990
		height: 120
		width: 495
		textLabel: "Space"
	}
	ButtonSq {
		id: button2520_1804
		x: 480
		y: 990
		width: 660
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Space"
		clickHandler.onClicked: onActivate2520()
	}
	StyledText {
		x: 1770
		y: 649
		height: 120
		width: 90
		textLabel: "Reset"
	}
	ButtonSq {
		id: button6081_1805
		x: 1770
		y: 649
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Reset"
		clickHandler.onClicked: onActivate6081()
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
		id: button6085_1806
		x: 120
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "!
1"
		clickHandler.onClicked: onActivate6085()
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
		id: button6087_1807
		x: 0
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Esc"
		clickHandler.onClicked: onActivate6087()
	}
	function onActivate6088() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18247
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"17\" ")
	}
	StyledText {
		x: 240
		y: 630
		height: 120
		width: 90
		textLabel: "@
2"
	}
	ButtonSq {
		id: button6088_1808
		x: 240
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "@
2"
		clickHandler.onClicked: onActivate6088()
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
		id: button6089_1809
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
		id: button6090_1810
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
		x: 600
		y: 630
		height: 120
		width: 90
		textLabel: "%
5"
	}
	ButtonSq {
		id: button6091_1811
		x: 600
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "%
5"
		clickHandler.onClicked: onActivate6091()
	}
	function onActivate6092() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18251
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"21\" ")
	}
	StyledText {
		x: 720
		y: 630
		height: 120
		width: 90
		textLabel: "^
6"
	}
	ButtonSq {
		id: button6092_1812
		x: 720
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "^
6"
		clickHandler.onClicked: onActivate6092()
	}
	function onActivate6093() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18252
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"22\" ")
	}
	StyledText {
		x: 840
		y: 630
		height: 120
		width: 90
		textLabel: "&
7"
	}
	ButtonSq {
		id: button6093_1813
		x: 840
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "&
7"
		clickHandler.onClicked: onActivate6093()
	}
	function onActivate6094() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18253
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"23\" ")
	}
	StyledText {
		x: 960
		y: 630
		height: 120
		width: 90
		textLabel: "*
8"
	}
	ButtonSq {
		id: button6094_1814
		x: 960
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "*
8"
		clickHandler.onClicked: onActivate6094()
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
		id: button6095_1815
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
		id: button6096_1816
		x: 1200
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ")
0"
		clickHandler.onClicked: onActivate6096()
	}
	function onActivate6097() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18256
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"182\" ")
	}
	StyledText {
		x: 1320
		y: 630
		height: 120
		width: 90
		textLabel: "_
-"
	}
	ButtonSq {
		id: button6097_1817
		x: 1320
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "_
-"
		clickHandler.onClicked: onActivate6097()
	}
	function onActivate6098() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18257
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"197\" ")
	}
	StyledText {
		x: 1440
		y: 630
		height: 120
		width: 90
		textLabel: "+
="
	}
	ButtonSq {
		id: button6098_1818
		x: 1440
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "+
="
		clickHandler.onClicked: onActivate6098()
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
		id: button6099_1819
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
		id: button6100_1820
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
		id: button6101_1821
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
		id: button6102_1822
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
		id: button6103_1823
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
		id: button6104_1824
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
		id: button6105_1825
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
		id: button6106_1826
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
		id: button6107_1827
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
		id: button6108_1828
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
		id: button6109_1829
		x: 1260
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "P"
		clickHandler.onClicked: onActivate6109()
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
		id: button6110_1830
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
		id: button6111_1831
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
		id: button6112_1832
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
		id: button6113_1833
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
		id: button6114_1834
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
		id: button6115_1835
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
		id: button6116_1836
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
		id: button6117_1837
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
		id: button6118_1838
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
		id: button6119_1839
		x: 1320
		y: 810
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ":
;"
		clickHandler.onClicked: onActivate6119()
	}
	function onActivate6120() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18284
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"185\" ")
	}
	StyledText {
		x: 1440
		y: 810
		height: 120
		width: 90
		textLabel: ""
	}

	ButtonSq {
		id: button6120_1840
		x: 1440
		y: 810
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6120()
	}

	function onActivate6121() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18285
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"5\" ")
	}
	StyledText {
		x: 1560
		y: 810
		height: 120
		width: 135
		textLabel: "Enter"
	}
	ButtonSq {
		id: button6121_1841
		x: 1560
		y: 810
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Enter"
		clickHandler.onClicked: onActivate6121()
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
		width: 202
		textLabel: "Shift"
	}
	ButtonSq {
		id: button6122_1842
		x: 0
		y: 900
		width: 270
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
		x: 270
		y: 900
		height: 120
		width: 90
		textLabel: "Z"
	}
	ButtonSq {
		id: button6123_1843
		x: 270
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
		x: 390
		y: 900
		height: 120
		width: 90
		textLabel: "X"
	}
	ButtonSq {
		id: button6124_1844
		x: 390
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
		x: 510
		y: 900
		height: 120
		width: 90
		textLabel: "C"
	}
	ButtonSq {
		id: button6125_1845
		x: 510
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
		x: 630
		y: 900
		height: 120
		width: 90
		textLabel: "V"
	}
	ButtonSq {
		id: button6126_1846
		x: 630
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
		x: 750
		y: 900
		height: 120
		width: 90
		textLabel: "B"
	}
	ButtonSq {
		id: button6127_1847
		x: 750
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
		x: 870
		y: 900
		height: 120
		width: 90
		textLabel: "N"
	}
	ButtonSq {
		id: button6128_1848
		x: 870
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
		x: 990
		y: 900
		height: 120
		width: 90
		textLabel: "M"
	}
	ButtonSq {
		id: button6129_1849
		x: 990
		y: 900
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "M"
		clickHandler.onClicked: onActivate6129()
	}
	function onActivate6122() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18286
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"226\" ")
	}
	StyledText {
		x: 1470
		y: 900
		height: 120
		width: 202
		textLabel: "Shift"
	}
	ButtonSq {
		id: button6122_1850
		x: 1470
		y: 900
		width: 270
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Shift"
		clickHandler.onClicked: onActivate6122()
	}
	function onActivate6130() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18296
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"170\" ")
	}
	StyledText {
		x: 0
		y: 990
		height: 120
		width: 90
		textLabel: "cap
lock"
	}
	ButtonSq {
		id: button6130_1851
		x: 0
		y: 990
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "cap
lock"
		clickHandler.onClicked: onActivate6130()
	}
	function onActivate6133() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18299
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"165\" ")
	}
	StyledText {
		x: 480
		y: 990
		height: 120
		width: 495
		textLabel: "Space"
	}
	ButtonSq {
		id: button6133_1852
		x: 480
		y: 990
		width: 660
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Space"
		clickHandler.onClicked: onActivate6133()
	}
	function onActivate6134() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18301
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"1\" ")
	}
	ButtonSq {
		id: button6134_1853
		x: 1260
		y: 990
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6134()
	}
	ButtonSq {
		id: button6135_1854
		x: 1260
		y: 1620
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6135()
	}
	function onActivate6143() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18302
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"2\" ")
	}
	ButtonSq {
		id: button6143_1855
		x: 1380
		y: 990
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6143()
	}
	ButtonSq {
		id: button6137_1856
		x: 1380
		y: 1620
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6137()
	}
	function onActivate6144() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18303
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"3\" ")
	}
	ButtonSq {
		id: button6144_1857
		x: 1500
		y: 990
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6144()
	}
	ButtonSq {
		id: button6139_1858
		x: 1500
		y: 1620
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/left.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6139()
	}
	function onActivate6145() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 18304
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"4\" ")
	}
	ButtonSq {
		id: button6145_1859
		x: 1620
		y: 990
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6145()
	}
	ButtonSq {
		id: button6141_1860
		x: 1620
		y: 1620
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/right.png"
		buttontext: ""
		clickHandler.onClicked: onActivate6141()
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
		id: button2455_1861
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
		id: button3285_1862
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
		id: button3547_1863
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
		id: button3351_1864
		x: 509
		y: 22
		width: 1142
		height: 587
		buttonbackground: "skins/Basic/<%=NPD%>"
		buttontext: ""
		clickHandler.onClicked: onActivate3351()
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
		id: button1785_1865
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
		id: button2458_1866
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
		id: button1569_1867
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
}
