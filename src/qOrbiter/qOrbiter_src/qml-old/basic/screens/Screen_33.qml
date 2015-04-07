// UI1 Screen WebURL (33)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
	}
	function onActivate1915() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2918
		setCurrentScreen("Screen_34.qml")
	}
	StyledText {
		x: 1038
		y: 460
		height: 240
		width: 162
		textLabel: "Go"
	}
	ButtonSq {
		id: button1915_353
		x: 1038
		y: 460
		width: 217
		height: 180
		buttonbackground: "skins/Basic/Buttons/roundblack.png"
		buttontext: "Go"
		clickHandler.onClicked: onActivate1915()
	}
	function onActivate1914() 
	{
	}
	ButtonSq {
		id: button1914_354
		x: 361
		y: 68
		width: 632
		height: 541
		buttonbackground: "skins/Basic/220"
		buttontext: ""
		clickHandler.onClicked: onActivate1914()
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
		id: button1785_355
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_356
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate2491() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13813
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"25\" ")
	}
	StyledText {
		x: 1107
		y: 704
		height: 120
		width: 90
		textLabel: ")
0"
	}
	ButtonSq {
		id: button2491_357
		x: 1107
		y: 704
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
		x: 27
		y: 704
		height: 120
		width: 90
		textLabel: "!
1"
	}
	ButtonSq {
		id: button2482_358
		x: 27
		y: 704
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
		x: 147
		y: 704
		height: 120
		width: 90
		textLabel: "@
2"
	}
	ButtonSq {
		id: button2483_359
		x: 147
		y: 704
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
		x: 267
		y: 704
		height: 120
		width: 90
		textLabel: "#
3"
	}
	ButtonSq {
		id: button2484_360
		x: 267
		y: 704
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
		x: 387
		y: 704
		height: 120
		width: 90
		textLabel: "$
4"
	}
	ButtonSq {
		id: button2485_361
		x: 387
		y: 704
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
		x: 507
		y: 704
		height: 120
		width: 90
		textLabel: "%
5"
	}
	ButtonSq {
		id: button2486_362
		x: 507
		y: 704
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
		x: 627
		y: 704
		height: 120
		width: 90
		textLabel: "^
6"
	}
	ButtonSq {
		id: button2487_363
		x: 627
		y: 704
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
		x: 747
		y: 704
		height: 120
		width: 90
		textLabel: "&
7"
	}
	ButtonSq {
		id: button2488_364
		x: 747
		y: 704
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
		x: 867
		y: 704
		height: 120
		width: 90
		textLabel: "*
8"
	}
	ButtonSq {
		id: button2489_365
		x: 867
		y: 704
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
		x: 987
		y: 704
		height: 120
		width: 90
		textLabel: "(
9"
	}
	ButtonSq {
		id: button2490_366
		x: 987
		y: 704
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
		x: 1227
		y: 704
		height: 120
		width: 90
		textLabel: "_
-"
	}
	ButtonSq {
		id: button2492_367
		x: 1227
		y: 704
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
		x: 1347
		y: 704
		height: 120
		width: 90
		textLabel: "+
="
	}
	ButtonSq {
		id: button2493_368
		x: 1347
		y: 704
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
		x: 88
		y: 794
		height: 120
		width: 90
		textLabel: "Q"
	}
	ButtonSq {
		id: button2510_369
		x: 88
		y: 794
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
		x: 208
		y: 794
		height: 120
		width: 90
		textLabel: "W"
	}
	ButtonSq {
		id: button2516_370
		x: 208
		y: 794
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
		x: 328
		y: 794
		height: 120
		width: 90
		textLabel: "E"
	}
	ButtonSq {
		id: button2498_371
		x: 328
		y: 794
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
		x: 448
		y: 794
		height: 120
		width: 90
		textLabel: "R"
	}
	ButtonSq {
		id: button2511_372
		x: 448
		y: 794
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
		x: 568
		y: 794
		height: 120
		width: 90
		textLabel: "T"
	}
	ButtonSq {
		id: button2513_373
		x: 568
		y: 794
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
		x: 688
		y: 794
		height: 120
		width: 90
		textLabel: "Y"
	}
	ButtonSq {
		id: button2518_374
		x: 688
		y: 794
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
		x: 808
		y: 794
		height: 120
		width: 90
		textLabel: "U"
	}
	ButtonSq {
		id: button2514_375
		x: 808
		y: 794
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
		x: 928
		y: 794
		height: 120
		width: 90
		textLabel: "I"
	}
	ButtonSq {
		id: button2502_376
		x: 928
		y: 794
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
		x: 1048
		y: 794
		height: 120
		width: 90
		textLabel: "O"
	}
	ButtonSq {
		id: button2508_377
		x: 1048
		y: 794
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
		x: 1168
		y: 794
		height: 120
		width: 90
		textLabel: "P"
	}
	ButtonSq {
		id: button2509_378
		x: 1168
		y: 794
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
		x: 147
		y: 884
		height: 120
		width: 90
		textLabel: "A"
	}
	ButtonSq {
		id: button2494_379
		x: 147
		y: 884
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
		x: 267
		y: 884
		height: 120
		width: 90
		textLabel: "S"
	}
	ButtonSq {
		id: button2512_380
		x: 267
		y: 884
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
		x: 387
		y: 884
		height: 120
		width: 90
		textLabel: "D"
	}
	ButtonSq {
		id: button2497_381
		x: 387
		y: 884
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
		x: 507
		y: 884
		height: 120
		width: 90
		textLabel: "F"
	}
	ButtonSq {
		id: button2499_382
		x: 507
		y: 884
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
		x: 627
		y: 884
		height: 120
		width: 90
		textLabel: "G"
	}
	ButtonSq {
		id: button2500_383
		x: 627
		y: 884
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
		x: 747
		y: 884
		height: 120
		width: 90
		textLabel: "H"
	}
	ButtonSq {
		id: button2501_384
		x: 747
		y: 884
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
		x: 867
		y: 884
		height: 120
		width: 90
		textLabel: "J"
	}
	ButtonSq {
		id: button2503_385
		x: 867
		y: 884
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
		x: 987
		y: 884
		height: 120
		width: 90
		textLabel: "K"
	}
	ButtonSq {
		id: button2504_386
		x: 987
		y: 884
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
		x: 1107
		y: 884
		height: 120
		width: 90
		textLabel: "L"
	}
	ButtonSq {
		id: button2505_387
		x: 1107
		y: 884
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
		x: 208
		y: 974
		height: 120
		width: 90
		textLabel: "Z"
	}
	ButtonSq {
		id: button2519_388
		x: 208
		y: 974
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
		x: 328
		y: 974
		height: 120
		width: 90
		textLabel: "X"
	}
	ButtonSq {
		id: button2517_389
		x: 328
		y: 974
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
		x: 448
		y: 974
		height: 120
		width: 90
		textLabel: "C"
	}
	ButtonSq {
		id: button2496_390
		x: 448
		y: 974
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
		x: 568
		y: 974
		height: 120
		width: 90
		textLabel: "V"
	}
	ButtonSq {
		id: button2515_391
		x: 568
		y: 974
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
		x: 688
		y: 974
		height: 120
		width: 90
		textLabel: "B"
	}
	ButtonSq {
		id: button2495_392
		x: 688
		y: 974
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
		x: 808
		y: 974
		height: 120
		width: 90
		textLabel: "N"
	}
	ButtonSq {
		id: button2507_393
		x: 808
		y: 974
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
		x: 928
		y: 974
		height: 120
		width: 90
		textLabel: "M"
	}
	ButtonSq {
		id: button2506_394
		x: 928
		y: 974
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
		x: 1227
		y: 884
		height: 120
		width: 90
		textLabel: ":
;"
	}
	ButtonSq {
		id: button2522_395
		x: 1227
		y: 884
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
		x: 1347
		y: 884
		height: 120
		width: 90
		textLabel: ""

	}
	ButtonSq {
		id: button2523_396
		x: 1347
		y: 884
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
		x: 1048
		y: 974
		height: 120
		width: 90
		textLabel: ","
	}
	ButtonSq {
		id: button2524_397
		x: 1048
		y: 974
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
		x: 1168
		y: 974
		height: 120
		width: 90
		textLabel: "."
	}
	ButtonSq {
		id: button2525_398
		x: 1168
		y: 974
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
		x: 1288
		y: 794
		height: 120
		width: 135
		textLabel: "<- Back"
	}
	ButtonSq {
		id: button2528_399
		x: 1288
		y: 794
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
		x: 1288
		y: 974
		height: 120
		width: 135
		textLabel: "Space"
	}
	ButtonSq {
		id: button2520_400
		x: 1288
		y: 974
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
		x: 27
		y: 974
		height: 120
		width: 135
		textLabel: "Shift"
	}
	ButtonSq {
		id: button2527_401
		x: 27
		y: 974
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
		x: 27
		y: 884
		height: 120
		width: 90
		textLabel: "cap
lock"
	}
	ButtonSq {
		id: button2529_402
		x: 27
		y: 884
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "cap
lock"
		clickHandler.onClicked: onActivate2529()
	}
}
