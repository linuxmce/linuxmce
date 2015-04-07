// UI1 Screen VDR OSD (274)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate2491() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13813
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"25\" ")
	}
	StyledText {
		x: 1200
		y: 1445
		height: 120
		width: 90
		textLabel: ")
0"
	}
	ButtonSq {
		id: button2491_2829
		x: 1200
		y: 1445
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
		x: 120
		y: 1445
		height: 120
		width: 90
		textLabel: "!
1"
	}
	ButtonSq {
		id: button2482_2830
		x: 120
		y: 1445
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
		x: 240
		y: 1445
		height: 120
		width: 90
		textLabel: "@
2"
	}
	ButtonSq {
		id: button2483_2831
		x: 240
		y: 1445
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
		x: 360
		y: 1445
		height: 120
		width: 90
		textLabel: "#
3"
	}
	ButtonSq {
		id: button2484_2832
		x: 360
		y: 1445
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
		x: 480
		y: 1445
		height: 120
		width: 90
		textLabel: "$
4"
	}
	ButtonSq {
		id: button2485_2833
		x: 480
		y: 1445
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
		x: 600
		y: 1445
		height: 120
		width: 90
		textLabel: "%
5"
	}
	ButtonSq {
		id: button2486_2834
		x: 600
		y: 1445
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
		x: 720
		y: 1445
		height: 120
		width: 90
		textLabel: "^
6"
	}
	ButtonSq {
		id: button2487_2835
		x: 720
		y: 1445
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
		x: 840
		y: 1445
		height: 120
		width: 90
		textLabel: "&
7"
	}
	ButtonSq {
		id: button2488_2836
		x: 840
		y: 1445
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
		x: 960
		y: 1445
		height: 120
		width: 90
		textLabel: "*
8"
	}
	ButtonSq {
		id: button2489_2837
		x: 960
		y: 1445
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
		x: 1080
		y: 1445
		height: 120
		width: 90
		textLabel: "(
9"
	}
	ButtonSq {
		id: button2490_2838
		x: 1080
		y: 1445
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
		x: 1320
		y: 1445
		height: 120
		width: 90
		textLabel: "_
-"
	}
	ButtonSq {
		id: button2492_2839
		x: 1320
		y: 1445
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
		x: 1440
		y: 1445
		height: 120
		width: 90
		textLabel: "+
="
	}
	ButtonSq {
		id: button2493_2840
		x: 1440
		y: 1445
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
		x: 180
		y: 1535
		height: 120
		width: 90
		textLabel: "Q"
	}
	ButtonSq {
		id: button2510_2841
		x: 180
		y: 1535
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
		x: 300
		y: 1535
		height: 120
		width: 90
		textLabel: "W"
	}
	ButtonSq {
		id: button2516_2842
		x: 300
		y: 1535
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
		x: 420
		y: 1535
		height: 120
		width: 90
		textLabel: "E"
	}
	ButtonSq {
		id: button2498_2843
		x: 420
		y: 1535
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
		x: 540
		y: 1535
		height: 120
		width: 90
		textLabel: "R"
	}
	ButtonSq {
		id: button2511_2844
		x: 540
		y: 1535
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
		x: 660
		y: 1535
		height: 120
		width: 90
		textLabel: "T"
	}
	ButtonSq {
		id: button2513_2845
		x: 660
		y: 1535
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
		x: 780
		y: 1535
		height: 120
		width: 90
		textLabel: "Y"
	}
	ButtonSq {
		id: button2518_2846
		x: 780
		y: 1535
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
		x: 900
		y: 1535
		height: 120
		width: 90
		textLabel: "U"
	}
	ButtonSq {
		id: button2514_2847
		x: 900
		y: 1535
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
		x: 1020
		y: 1535
		height: 120
		width: 90
		textLabel: "I"
	}
	ButtonSq {
		id: button2502_2848
		x: 1020
		y: 1535
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
		x: 1140
		y: 1535
		height: 120
		width: 90
		textLabel: "O"
	}
	ButtonSq {
		id: button2508_2849
		x: 1140
		y: 1535
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
		x: 1260
		y: 1535
		height: 120
		width: 90
		textLabel: "P"
	}
	ButtonSq {
		id: button2509_2850
		x: 1260
		y: 1535
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
		x: 240
		y: 1625
		height: 120
		width: 90
		textLabel: "A"
	}
	ButtonSq {
		id: button2494_2851
		x: 240
		y: 1625
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
		x: 360
		y: 1625
		height: 120
		width: 90
		textLabel: "S"
	}
	ButtonSq {
		id: button2512_2852
		x: 360
		y: 1625
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
		x: 480
		y: 1625
		height: 120
		width: 90
		textLabel: "D"
	}
	ButtonSq {
		id: button2497_2853
		x: 480
		y: 1625
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
		x: 600
		y: 1625
		height: 120
		width: 90
		textLabel: "F"
	}
	ButtonSq {
		id: button2499_2854
		x: 600
		y: 1625
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
		x: 720
		y: 1625
		height: 120
		width: 90
		textLabel: "G"
	}
	ButtonSq {
		id: button2500_2855
		x: 720
		y: 1625
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
		x: 840
		y: 1625
		height: 120
		width: 90
		textLabel: "H"
	}
	ButtonSq {
		id: button2501_2856
		x: 840
		y: 1625
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
		x: 960
		y: 1625
		height: 120
		width: 90
		textLabel: "J"
	}
	ButtonSq {
		id: button2503_2857
		x: 960
		y: 1625
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
		x: 1080
		y: 1625
		height: 120
		width: 90
		textLabel: "K"
	}
	ButtonSq {
		id: button2504_2858
		x: 1080
		y: 1625
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
		x: 1200
		y: 1625
		height: 120
		width: 90
		textLabel: "L"
	}
	ButtonSq {
		id: button2505_2859
		x: 1200
		y: 1625
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
		x: 300
		y: 1715
		height: 120
		width: 90
		textLabel: "Z"
	}
	ButtonSq {
		id: button2519_2860
		x: 300
		y: 1715
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
		x: 420
		y: 1715
		height: 120
		width: 90
		textLabel: "X"
	}
	ButtonSq {
		id: button2517_2861
		x: 420
		y: 1715
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
		x: 540
		y: 1715
		height: 120
		width: 90
		textLabel: "C"
	}
	ButtonSq {
		id: button2496_2862
		x: 540
		y: 1715
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
		x: 660
		y: 1715
		height: 120
		width: 90
		textLabel: "V"
	}
	ButtonSq {
		id: button2515_2863
		x: 660
		y: 1715
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
		x: 780
		y: 1715
		height: 120
		width: 90
		textLabel: "B"
	}
	ButtonSq {
		id: button2495_2864
		x: 780
		y: 1715
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
		x: 900
		y: 1715
		height: 120
		width: 90
		textLabel: "N"
	}
	ButtonSq {
		id: button2507_2865
		x: 900
		y: 1715
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
		x: 1020
		y: 1715
		height: 120
		width: 90
		textLabel: "M"
	}
	ButtonSq {
		id: button2506_2866
		x: 1020
		y: 1715
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
		x: 1320
		y: 1625
		height: 120
		width: 90
		textLabel: ":
;"
	}
	ButtonSq {
		id: button2522_2867
		x: 1320
		y: 1625
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
		x: 1440
		y: 1625
		height: 120
		width: 90
		textLabel: ""

	}
	ButtonSq {
		id: button2523_2868
		x: 1440
		y: 1625
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
		x: 1140
		y: 1715
		height: 120
		width: 90
		textLabel: ","
	}
	ButtonSq {
		id: button2524_2869
		x: 1140
		y: 1715
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
		x: 1260
		y: 1715
		height: 120
		width: 90
		textLabel: "."
	}
	ButtonSq {
		id: button2525_2870
		x: 1260
		y: 1715
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
		x: 1380
		y: 1535
		height: 120
		width: 135
		textLabel: "<- Back"
	}
	ButtonSq {
		id: button2528_2871
		x: 1380
		y: 1535
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
		x: 1380
		y: 1715
		height: 120
		width: 135
		textLabel: "Space"
	}
	ButtonSq {
		id: button2520_2872
		x: 1380
		y: 1715
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
		x: 120
		y: 1715
		height: 120
		width: 135
		textLabel: "Shift"
	}
	ButtonSq {
		id: button2527_2873
		x: 120
		y: 1715
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
		x: 120
		y: 1625
		height: 120
		width: 90
		textLabel: "cap
lock"
	}
	ButtonSq {
		id: button2529_2874
		x: 120
		y: 1625
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "cap
lock"
		clickHandler.onClicked: onActivate2529()
	}
	function onActivate4305() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16023
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"187\" ")
	}
	StyledText {
		x: 0
		y: 723
		height: 120
		width: 90
		textLabel: "Esc"
	}
	ButtonSq {
		id: button4305_2875
		x: 0
		y: 723
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Esc"
		clickHandler.onClicked: onActivate4305()
	}
	function onActivate4306() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16024
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"188\" ")
	}
	StyledText {
		x: 0
		y: 813
		height: 120
		width: 135
		textLabel: "Tab"
	}
	ButtonSq {
		id: button4306_2876
		x: 0
		y: 813
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Tab"
		clickHandler.onClicked: onActivate4306()
	}
	function onActivate4312() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16025
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"5\" ")
	}
	StyledText {
		x: 1652
		y: 813
		height: 120
		width: 135
		textLabel: "Enter"
	}
	ButtonSq {
		id: button4312_2877
		x: 1652
		y: 813
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Enter"
		clickHandler.onClicked: onActivate4312()
	}
	function onActivate4310() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16028
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 28 26 \"3\" ")
	}
	ButtonSq {
		id: button4310_2878
		x: 1560
		y: 993
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4310()
	}
	ButtonSq {
		id: button4315_2879
		x: 1560
		y: 1715
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/left.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4315()
	}
	function onActivate4308() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16026
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 28 26 \"1\" ")
	}
	ButtonSq {
		id: button4308_2880
		x: 1680
		y: 903
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4308()
	}
	ButtonSq {
		id: button4313_2881
		x: 1680
		y: 1625
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4313()
	}
	function onActivate4309() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16027
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 28 26 \"2\" ")
	}
	ButtonSq {
		id: button4309_2882
		x: 1680
		y: 993
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4309()
	}
	ButtonSq {
		id: button4314_2883
		x: 1680
		y: 1715
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4314()
	}
	function onActivate4311() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16029
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 28 26 \"4\" ")
	}
	ButtonSq {
		id: button4311_2884
		x: 1800
		y: 993
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4311()
	}
	ButtonSq {
		id: button4316_2885
		x: 1800
		y: 1715
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/right.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4316()
	}
	StyledText {
		x: 1209
		y: 187
		height: 208
		width: 346
		textLabel: "<%=NP%>"
	}
	StyledText {
		x: 1200
		y: 349
		height: 164
		width: 360
		textLabel: "<%=NP_SEC%>"
	}
	StyledText {
		x: 1200
		y: 472
		height: 90
		width: 271
		textLabel: "<%=NP_TIME_SHORT%>"
	}
	StyledText {
		x: 1561
		y: 472
		height: 90
		width: 89
		textLabel: "<%=NP_SPEED%>"
	}
	ButtonSq {
		id: button2455_2886
		x: 1200
		y: 180
		width: 480
		height: 360
		buttonbackground: "skins/Basic/532"
		buttontext: "<%=NP_SPEED%>"
		clickHandler.onClicked: onActivate2455()
	}
	function onActivate1569() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13739
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18035
		setCurrentScreen("Screen_1.qml")
	}
	ButtonSq {
		id: button1569_2887
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
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
		id: button1785_2888
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate4771() 
	{
	 // Calling Command 548 (Menu) with CommandGroup_Parameters from 16672
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 548 9 \"old_recordings\" ")
	}
	StyledText {
		x: 240
		y: 180
		height: 240
		width: 180
		textLabel: "Recordings"
	}
	ButtonSq {
		id: button4771_2889
		x: 240
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Recordings
"
		clickHandler.onClicked: onActivate4771()
	}
	function onActivate4772() 
	{
	 // Calling Command 548 (Menu) with CommandGroup_Parameters from 16673
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 548 9 \"future_recordings\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17309
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 480
		y: 180
		height: 240
		width: 180
		textLabel: "Schedule"
	}
	ButtonSq {
		id: button4772_2890
		x: 480
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Schedule
"
		clickHandler.onClicked: onActivate4772()
	}
	function onActivate4775() 
	{
	 // Calling Command 762 (Live TV) with CommandGroup_Parameters from 16683
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 762 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17310
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 240
		y: 360
		height: 240
		width: 180
		textLabel: "live"
	}
	ButtonSq {
		id: button4775_2891
		x: 240
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "live"
		clickHandler.onClicked: onActivate4775()
	}
	function onActivate2445() 
	{
	 // Calling Command 126 (Guide) with CommandGroup_Parameters from 16669
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 126 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17308
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 480
		y: 360
		height: 240
		width: 180
		textLabel: "guide"
	}
	ButtonSq {
		id: button2445_2892
		x: 480
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "guide"
		clickHandler.onClicked: onActivate2445()
	}
}
