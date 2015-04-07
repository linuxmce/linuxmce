// UI1 Screen UsersWizard (193)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/wizard_bigXine.png"
		width: 1920
		height: 1080
	}
	function onActivate3558() 
	{
	}
	function onActivate5067() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17032
		setCurrentScreen("Screen_53.qml")
	}
	StyledText {
		x: 1680
		y: 0
		height: 240
		width: 180
		textLabel: "Cancel Wizard"
	}
	ButtonSq {
		id: button5067_2138
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareOn.png"
		buttontext: "Cancel Wizard"
		clickHandler.onClicked: onActivate5067()
	}
	function onActivate4569() 
	{
	 // Calling Command 15 (Regen Screen) with CommandGroup_Parameters from 17507
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 15 ")
	}
	StyledText {
		x: 1680
		y: 360
		height: 240
		width: 180
		textLabel: "Please repeat"
	}
	ButtonSq {
		id: button4569_2139
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Please repeat"
		clickHandler.onClicked: onActivate4569()
	}
	function onActivate4571() 
	{
	 // Calling Command 5 (Goto DesignObj) with CommandGroup_Parameters from 16326
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 5 3 \"4551\" ")
	}
	StyledText {
		x: 1444
		y: 914
		height: 211
		width: 237
		textLabel: "Next ->"
	}
	ButtonSq {
		id: button4571_2140
		x: 1444
		y: 914
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/next.png"
		buttontext: "Next ->"
		clickHandler.onClicked: onActivate4571()
	}
	function onActivate2491() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13813
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"25\" ")
	}
	StyledText {
		x: 1080
		y: 1563
		height: 120
		width: 90
		textLabel: ")
0"
	}
	ButtonSq {
		id: button2491_2141
		x: 1080
		y: 1563
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ")
0"
		clickHandler.onClicked: onActivate2491()
	}
	function onActivate2482() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13814
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"16\" ")
	}
	StyledText {
		x: 0
		y: 1563
		height: 120
		width: 90
		textLabel: "!
1"
	}
	ButtonSq {
		id: button2482_2142
		x: 0
		y: 1563
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "!
1"
		clickHandler.onClicked: onActivate2482()
	}
	function onActivate2483() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13815
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"17\" ")
	}
	StyledText {
		x: 120
		y: 1563
		height: 120
		width: 90
		textLabel: "@
2"
	}
	ButtonSq {
		id: button2483_2143
		x: 120
		y: 1563
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "@
2"
		clickHandler.onClicked: onActivate2483()
	}
	function onActivate2484() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13816
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"18\" ")
	}
	StyledText {
		x: 240
		y: 1563
		height: 120
		width: 90
		textLabel: "#
3"
	}
	ButtonSq {
		id: button2484_2144
		x: 240
		y: 1563
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "#
3"
		clickHandler.onClicked: onActivate2484()
	}
	function onActivate2485() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13817
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"19\" ")
	}
	StyledText {
		x: 360
		y: 1563
		height: 120
		width: 90
		textLabel: "$
4"
	}
	ButtonSq {
		id: button2485_2145
		x: 360
		y: 1563
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "$
4"
		clickHandler.onClicked: onActivate2485()
	}
	function onActivate2486() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13818
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"20\" ")
	}
	StyledText {
		x: 480
		y: 1563
		height: 120
		width: 90
		textLabel: "%
5"
	}
	ButtonSq {
		id: button2486_2146
		x: 480
		y: 1563
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "%
5"
		clickHandler.onClicked: onActivate2486()
	}
	function onActivate2487() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13819
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"21\" ")
	}
	StyledText {
		x: 600
		y: 1563
		height: 120
		width: 90
		textLabel: "^
6"
	}
	ButtonSq {
		id: button2487_2147
		x: 600
		y: 1563
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "^
6"
		clickHandler.onClicked: onActivate2487()
	}
	function onActivate2488() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13820
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"22\" ")
	}
	StyledText {
		x: 720
		y: 1563
		height: 120
		width: 90
		textLabel: "&
7"
	}
	ButtonSq {
		id: button2488_2148
		x: 720
		y: 1563
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "&
7"
		clickHandler.onClicked: onActivate2488()
	}
	function onActivate2489() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13821
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"23\" ")
	}
	StyledText {
		x: 840
		y: 1563
		height: 120
		width: 90
		textLabel: "*
8"
	}
	ButtonSq {
		id: button2489_2149
		x: 840
		y: 1563
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "*
8"
		clickHandler.onClicked: onActivate2489()
	}
	function onActivate2490() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13822
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"24\" ")
	}
	StyledText {
		x: 960
		y: 1563
		height: 120
		width: 90
		textLabel: "(
9"
	}
	ButtonSq {
		id: button2490_2150
		x: 960
		y: 1563
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "(
9"
		clickHandler.onClicked: onActivate2490()
	}
	function onActivate2492() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13823
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"182\" 50 \"dash\" ")
	}
	StyledText {
		x: 1200
		y: 1563
		height: 120
		width: 90
		textLabel: "_
-"
	}
	ButtonSq {
		id: button2492_2151
		x: 1200
		y: 1563
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "_
-"
		clickHandler.onClicked: onActivate2492()
	}
	function onActivate2493() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13824
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"197\" 50 \"equals sign\" ")
	}
	StyledText {
		x: 1320
		y: 1563
		height: 120
		width: 90
		textLabel: "+
="
	}
	ButtonSq {
		id: button2493_2152
		x: 1320
		y: 1563
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "+
="
		clickHandler.onClicked: onActivate2493()
	}
	function onActivate2510() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13825
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"129\" ")
	}
	StyledText {
		x: 60
		y: 1653
		height: 120
		width: 90
		textLabel: "Q"
	}
	ButtonSq {
		id: button2510_2153
		x: 60
		y: 1653
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Q"
		clickHandler.onClicked: onActivate2510()
	}
	function onActivate2516() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13826
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"135\" ")
	}
	StyledText {
		x: 180
		y: 1653
		height: 120
		width: 90
		textLabel: "W"
	}
	ButtonSq {
		id: button2516_2154
		x: 180
		y: 1653
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "W"
		clickHandler.onClicked: onActivate2516()
	}
	function onActivate2498() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13827
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"117\" ")
	}
	StyledText {
		x: 300
		y: 1653
		height: 120
		width: 90
		textLabel: "E"
	}
	ButtonSq {
		id: button2498_2155
		x: 300
		y: 1653
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "E"
		clickHandler.onClicked: onActivate2498()
	}
	function onActivate2511() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13828
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"130\" ")
	}
	StyledText {
		x: 420
		y: 1653
		height: 120
		width: 90
		textLabel: "R"
	}
	ButtonSq {
		id: button2511_2156
		x: 420
		y: 1653
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "R"
		clickHandler.onClicked: onActivate2511()
	}
	function onActivate2513() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13829
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"132\" ")
	}
	StyledText {
		x: 540
		y: 1653
		height: 120
		width: 90
		textLabel: "T"
	}
	ButtonSq {
		id: button2513_2157
		x: 540
		y: 1653
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "T"
		clickHandler.onClicked: onActivate2513()
	}
	function onActivate2518() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13830
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"137\" ")
	}
	StyledText {
		x: 660
		y: 1653
		height: 120
		width: 90
		textLabel: "Y"
	}
	ButtonSq {
		id: button2518_2158
		x: 660
		y: 1653
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Y"
		clickHandler.onClicked: onActivate2518()
	}
	function onActivate2514() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13831
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"133\" ")
	}
	StyledText {
		x: 780
		y: 1653
		height: 120
		width: 90
		textLabel: "U"
	}
	ButtonSq {
		id: button2514_2159
		x: 780
		y: 1653
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "U"
		clickHandler.onClicked: onActivate2514()
	}
	function onActivate2502() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13832
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"121\" ")
	}
	StyledText {
		x: 900
		y: 1653
		height: 120
		width: 90
		textLabel: "I"
	}
	ButtonSq {
		id: button2502_2160
		x: 900
		y: 1653
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "I"
		clickHandler.onClicked: onActivate2502()
	}
	function onActivate2508() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13833
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"127\" ")
	}
	StyledText {
		x: 1020
		y: 1653
		height: 120
		width: 90
		textLabel: "O"
	}
	ButtonSq {
		id: button2508_2161
		x: 1020
		y: 1653
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "O"
		clickHandler.onClicked: onActivate2508()
	}
	function onActivate2509() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13834
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"128\" ")
	}
	StyledText {
		x: 1140
		y: 1653
		height: 120
		width: 90
		textLabel: "P"
	}
	ButtonSq {
		id: button2509_2162
		x: 1140
		y: 1653
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "P"
		clickHandler.onClicked: onActivate2509()
	}
	function onActivate2494() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13835
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"113\" ")
	}
	StyledText {
		x: 120
		y: 1743
		height: 120
		width: 90
		textLabel: "A"
	}
	ButtonSq {
		id: button2494_2163
		x: 120
		y: 1743
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "A"
		clickHandler.onClicked: onActivate2494()
	}
	function onActivate2512() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13836
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"131\" ")
	}
	StyledText {
		x: 240
		y: 1743
		height: 120
		width: 90
		textLabel: "S"
	}
	ButtonSq {
		id: button2512_2164
		x: 240
		y: 1743
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "S"
		clickHandler.onClicked: onActivate2512()
	}
	function onActivate2497() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13837
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"116\" ")
	}
	StyledText {
		x: 360
		y: 1743
		height: 120
		width: 90
		textLabel: "D"
	}
	ButtonSq {
		id: button2497_2165
		x: 360
		y: 1743
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "D"
		clickHandler.onClicked: onActivate2497()
	}
	function onActivate2499() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13838
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"118\" ")
	}
	StyledText {
		x: 480
		y: 1743
		height: 120
		width: 90
		textLabel: "F"
	}
	ButtonSq {
		id: button2499_2166
		x: 480
		y: 1743
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "F"
		clickHandler.onClicked: onActivate2499()
	}
	function onActivate2500() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13839
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"119\" ")
	}
	StyledText {
		x: 600
		y: 1743
		height: 120
		width: 90
		textLabel: "G"
	}
	ButtonSq {
		id: button2500_2167
		x: 600
		y: 1743
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "G"
		clickHandler.onClicked: onActivate2500()
	}
	function onActivate2501() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13840
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"120\" ")
	}
	StyledText {
		x: 720
		y: 1743
		height: 120
		width: 90
		textLabel: "H"
	}
	ButtonSq {
		id: button2501_2168
		x: 720
		y: 1743
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "H"
		clickHandler.onClicked: onActivate2501()
	}
	function onActivate2503() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13841
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"122\" ")
	}
	StyledText {
		x: 840
		y: 1743
		height: 120
		width: 90
		textLabel: "J"
	}
	ButtonSq {
		id: button2503_2169
		x: 840
		y: 1743
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "J"
		clickHandler.onClicked: onActivate2503()
	}
	function onActivate2504() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13842
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"123\" ")
	}
	StyledText {
		x: 960
		y: 1743
		height: 120
		width: 90
		textLabel: "K"
	}
	ButtonSq {
		id: button2504_2170
		x: 960
		y: 1743
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "K"
		clickHandler.onClicked: onActivate2504()
	}
	function onActivate2505() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13843
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"124\" ")
	}
	StyledText {
		x: 1080
		y: 1743
		height: 120
		width: 90
		textLabel: "L"
	}
	ButtonSq {
		id: button2505_2171
		x: 1080
		y: 1743
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "L"
		clickHandler.onClicked: onActivate2505()
	}
	function onActivate2519() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13844
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"138\" ")
	}
	StyledText {
		x: 180
		y: 1833
		height: 120
		width: 90
		textLabel: "Z"
	}
	ButtonSq {
		id: button2519_2172
		x: 180
		y: 1833
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Z"
		clickHandler.onClicked: onActivate2519()
	}
	function onActivate2517() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13845
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"136\" ")
	}
	StyledText {
		x: 300
		y: 1833
		height: 120
		width: 90
		textLabel: "X"
	}
	ButtonSq {
		id: button2517_2173
		x: 300
		y: 1833
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "X"
		clickHandler.onClicked: onActivate2517()
	}
	function onActivate2496() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13846
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"115\" ")
	}
	StyledText {
		x: 420
		y: 1833
		height: 120
		width: 90
		textLabel: "C"
	}
	ButtonSq {
		id: button2496_2174
		x: 420
		y: 1833
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "C"
		clickHandler.onClicked: onActivate2496()
	}
	function onActivate2515() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13847
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"134\" ")
	}
	StyledText {
		x: 540
		y: 1833
		height: 120
		width: 90
		textLabel: "V"
	}
	ButtonSq {
		id: button2515_2175
		x: 540
		y: 1833
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "V"
		clickHandler.onClicked: onActivate2515()
	}
	function onActivate2495() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13848
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"114\" ")
	}
	StyledText {
		x: 660
		y: 1833
		height: 120
		width: 90
		textLabel: "B"
	}
	ButtonSq {
		id: button2495_2176
		x: 660
		y: 1833
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "B"
		clickHandler.onClicked: onActivate2495()
	}
	function onActivate2507() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13849
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"126\" ")
	}
	StyledText {
		x: 780
		y: 1833
		height: 120
		width: 90
		textLabel: "N"
	}
	ButtonSq {
		id: button2507_2177
		x: 780
		y: 1833
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "N"
		clickHandler.onClicked: onActivate2507()
	}
	function onActivate2506() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13850
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"125\" ")
	}
	StyledText {
		x: 900
		y: 1833
		height: 120
		width: 90
		textLabel: "M"
	}
	ButtonSq {
		id: button2506_2178
		x: 900
		y: 1833
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "M"
		clickHandler.onClicked: onActivate2506()
	}
	function onActivate2522() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13851
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"184\" 50 \";\" ")
	}
	StyledText {
		x: 1200
		y: 1743
		height: 120
		width: 90
		textLabel: ":
;"
	}
	ButtonSq {
		id: button2522_2179
		x: 1200
		y: 1743
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ":
;"
		clickHandler.onClicked: onActivate2522()
	}
	function onActivate2523() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13852
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"185\" 50 \"\'\" ")
	}
	StyledText {
		x: 1320
		y: 1743
		height: 120
		width: 90
		textLabel: ""

	}
	ButtonSq {
		id: button2523_2180
		x: 1320
		y: 1743
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""

		clickHandler.onClicked: onActivate2523()
	}
	function onActivate2524() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13853
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"166\" ")
	}
	StyledText {
		x: 1020
		y: 1833
		height: 120
		width: 90
		textLabel: ","
	}
	ButtonSq {
		id: button2524_2181
		x: 1020
		y: 1833
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ","
		clickHandler.onClicked: onActivate2524()
	}
	function onActivate2525() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13854
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"186\" 50 \".\" ")
	}
	StyledText {
		x: 1140
		y: 1833
		height: 120
		width: 90
		textLabel: "."
	}
	ButtonSq {
		id: button2525_2182
		x: 1140
		y: 1833
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "."
		clickHandler.onClicked: onActivate2525()
	}
	function onActivate2528() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13855
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"112\" ")
	}
	StyledText {
		x: 1260
		y: 1653
		height: 120
		width: 135
		textLabel: "<- Back"
	}
	ButtonSq {
		id: button2528_2183
		x: 1260
		y: 1653
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "<- Back"
		clickHandler.onClicked: onActivate2528()
	}
	function onActivate2520() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13856
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"165\" ")
	}
	StyledText {
		x: 1260
		y: 1833
		height: 120
		width: 135
		textLabel: "Space"
	}
	ButtonSq {
		id: button2520_2184
		x: 1260
		y: 1833
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Space"
		clickHandler.onClicked: onActivate2520()
	}
	function onActivate2527() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13857
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"168\" ")
	}
	StyledText {
		x: 0
		y: 1833
		height: 120
		width: 135
		textLabel: "Shift"
	}
	ButtonSq {
		id: button2527_2185
		x: 0
		y: 1833
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Shift"
		clickHandler.onClicked: onActivate2527()
	}
	function onActivate2529() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13858
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"170\" ")
	}
	StyledText {
		x: 0
		y: 1743
		height: 120
		width: 90
		textLabel: "cap
lock"
	}
	ButtonSq {
		id: button2529_2186
		x: 0
		y: 1743
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "cap
lock"
		clickHandler.onClicked: onActivate2529()
	}
	function onActivate4658() 
	{
	 // Calling Command 38 (Stop Media) with CommandGroup_Parameters from 16418
		manager.sendDceMessage(srouterip + " "+deviceid + " -152 1 38 ")
	}
	StyledText {
		x: 1680
		y: 180
		height: 240
		width: 180
		textLabel: "Stop video"
	}
	ButtonSq {
		id: button4658_2187
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Stop video"
		clickHandler.onClicked: onActivate4658()
	}
	StyledText {
		x: 0
		y: 0
		height: 60
		width: 270
		textLabel: "House Setup"
	}
	function onActivate4619() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16441
		setCurrentScreen("Screen_193.qml")
	}
	StyledText {
		x: 0
		y: 45
		height: 120
		width: 270
		textLabel: "Names"
	}
	ButtonSq {
		id: button4619_2188
		x: 0
		y: 45
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Names"
		clickHandler.onClicked: onActivate4619()
	}
	function onActivate4656() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16442
		setCurrentScreen("Screen_192.qml")
	}
	StyledText {
		x: 0
		y: 225
		height: 120
		width: 270
		textLabel: "Rooms"
	}
	ButtonSq {
		id: button4656_2189
		x: 0
		y: 225
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Rooms"
		clickHandler.onClicked: onActivate4656()
	}
	function onActivate4647() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16536
		setCurrentScreen("Screen_198.qml")
	}
	StyledText {
		x: 0
		y: 315
		height: 120
		width: 270
		textLabel: "Lights"
	}
	ButtonSq {
		id: button4647_2190
		x: 0
		y: 315
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Lights"
		clickHandler.onClicked: onActivate4647()
	}
	function onActivate4654() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16537
		setCurrentScreen("Screen_200.qml")
	}
	StyledText {
		x: 0
		y: 495
		height: 120
		width: 270
		textLabel: "VOIP Provider"
	}
	ButtonSq {
		id: button4654_2191
		x: 0
		y: 495
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "VOIP Provider"
		clickHandler.onClicked: onActivate4654()
	}
	function onActivate4679() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16438
		setCurrentScreen("Screen_194.qml")
	}
	StyledText {
		x: 0
		y: 135
		height: 120
		width: 270
		textLabel: "Location"
	}
	ButtonSq {
		id: button4679_2192
		x: 0
		y: 135
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Location"
		clickHandler.onClicked: onActivate4679()
	}
	function onActivate4682() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16440
		setCurrentScreen("Screen_199.qml")
	}
	StyledText {
		x: 0
		y: 405
		height: 120
		width: 270
		textLabel: "Alarm Panel"
	}
	ButtonSq {
		id: button4682_2193
		x: 0
		y: 405
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Alarm Panel"
		clickHandler.onClicked: onActivate4682()
	}
	function onActivate5318() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17300
		setCurrentScreen("Screen_250.qml")
	}
	StyledText {
		x: 0
		y: 585
		height: 120
		width: 270
		textLabel: "PVR Software"
	}
	ButtonSq {
		id: button5318_2194
		x: 0
		y: 585
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "PVR Software"
		clickHandler.onClicked: onActivate5318()
	}
	function onActivate5319() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17305
		setCurrentScreen("Screen_251.qml")
	}
	StyledText {
		x: 0
		y: 675
		height: 120
		width: 270
		textLabel: "Done"
	}
	ButtonSq {
		id: button5319_2195
		x: 0
		y: 675
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Done"
		clickHandler.onClicked: onActivate5319()
	}
	function onActivate5437() 
	{
	 // Calling Command 912 (Activate PC Desktop) with CommandGroup_Parameters from 17388
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 912 119 \"1\" ")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "KDE desktop"
	}
	ButtonSq {
		id: button5437_2196
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "KDE desktop"
		clickHandler.onClicked: onActivate5437()
	}
}
