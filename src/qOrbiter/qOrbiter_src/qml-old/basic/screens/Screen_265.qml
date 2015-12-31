// UI1 Screen Zoom & Aspect (265)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
	}
	function onActivate5472() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17399
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"-5\" ")
	}
	StyledText {
		x: 361
		y: 359
		height: 72
		width: 275
		textLabel: "-5"
	}
	ButtonSq {
		id: button5472_2711
		x: 361
		y: 359
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "-5"
		clickHandler.onClicked: onActivate5472()
	}
	function onActivate5471() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17398
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"+5\" ")
	}
	StyledText {
		x: 361
		y: 738
		height: 72
		width: 275
		textLabel: "+5"
	}
	ButtonSq {
		id: button5471_2712
		x: 361
		y: 738
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "+5"
		clickHandler.onClicked: onActivate5471()
	}
	function onActivate5478() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17401
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"-7\" ")
	}
	StyledText {
		x: 361
		y: 305
		height: 72
		width: 275
		textLabel: "-7"
	}
	ButtonSq {
		id: button5478_2713
		x: 361
		y: 305
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "-7"
		clickHandler.onClicked: onActivate5478()
	}
	function onActivate5479() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17402
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"-4\" ")
	}
	StyledText {
		x: 361
		y: 413
		height: 72
		width: 275
		textLabel: "-4"
	}
	ButtonSq {
		id: button5479_2714
		x: 361
		y: 413
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "-4"
		clickHandler.onClicked: onActivate5479()
	}
	function onActivate5480() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17403
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"-2\" ")
	}
	StyledText {
		x: 361
		y: 467
		height: 72
		width: 275
		textLabel: "-2"
	}
	ButtonSq {
		id: button5480_2715
		x: 361
		y: 467
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "-2"
		clickHandler.onClicked: onActivate5480()
	}
	function onActivate5481() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17404
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"-1\" ")
	}
	StyledText {
		x: 361
		y: 521
		height: 72
		width: 275
		textLabel: "-1"
	}
	ButtonSq {
		id: button5481_2716
		x: 361
		y: 521
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "-1"
		clickHandler.onClicked: onActivate5481()
	}
	function onActivate5477() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17405
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"+1\" ")
	}
	StyledText {
		x: 361
		y: 575
		height: 72
		width: 275
		textLabel: "+1"
	}
	ButtonSq {
		id: button5477_2717
		x: 361
		y: 575
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "+1"
		clickHandler.onClicked: onActivate5477()
	}
	function onActivate5476() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17406
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"+2\" ")
	}
	StyledText {
		x: 361
		y: 629
		height: 72
		width: 275
		textLabel: "+2"
	}
	ButtonSq {
		id: button5476_2718
		x: 361
		y: 629
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "+2"
		clickHandler.onClicked: onActivate5476()
	}
	function onActivate5475() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17407
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"+4\" ")
	}
	StyledText {
		x: 361
		y: 683
		height: 72
		width: 275
		textLabel: "+4"
	}
	ButtonSq {
		id: button5475_2719
		x: 361
		y: 683
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "+4"
		clickHandler.onClicked: onActivate5475()
	}
	function onActivate5474() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17408
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"+7\" ")
	}
	StyledText {
		x: 361
		y: 792
		height: 72
		width: 275
		textLabel: "+7"
	}
	ButtonSq {
		id: button5474_2720
		x: 361
		y: 792
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "+7"
		clickHandler.onClicked: onActivate5474()
	}
	function onActivate5484() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17410
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"90\" ")
	}
	StyledText {
		x: 767
		y: 305
		height: 72
		width: 275
		textLabel: "90%"
	}
	ButtonSq {
		id: button5484_2721
		x: 767
		y: 305
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "90%"
		clickHandler.onClicked: onActivate5484()
	}
	function onActivate5483() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17409
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"95\" ")
	}
	StyledText {
		x: 767
		y: 359
		height: 72
		width: 275
		textLabel: "95%"
	}
	ButtonSq {
		id: button5483_2722
		x: 767
		y: 359
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "95%"
		clickHandler.onClicked: onActivate5483()
	}
	function onActivate5485() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17411
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"100\" ")
	}
	StyledText {
		x: 767
		y: 413
		height: 72
		width: 275
		textLabel: "100%"
	}
	ButtonSq {
		id: button5485_2723
		x: 767
		y: 413
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "100%"
		clickHandler.onClicked: onActivate5485()
	}
	function onActivate5486() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17412
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"105\" ")
	}
	StyledText {
		x: 767
		y: 467
		height: 72
		width: 275
		textLabel: "105%"
	}
	ButtonSq {
		id: button5486_2724
		x: 767
		y: 467
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "105%"
		clickHandler.onClicked: onActivate5486()
	}
	function onActivate5487() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17413
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"110\" ")
	}
	StyledText {
		x: 767
		y: 521
		height: 72
		width: 275
		textLabel: "110%"
	}
	ButtonSq {
		id: button5487_2725
		x: 767
		y: 521
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "110%"
		clickHandler.onClicked: onActivate5487()
	}
	function onActivate5488() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17414
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"115\" ")
	}
	StyledText {
		x: 767
		y: 575
		height: 72
		width: 275
		textLabel: "115%"
	}
	ButtonSq {
		id: button5488_2726
		x: 767
		y: 575
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "115%"
		clickHandler.onClicked: onActivate5488()
	}
	function onActivate5489() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17415
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"120\" ")
	}
	StyledText {
		x: 767
		y: 629
		height: 72
		width: 275
		textLabel: "120%"
	}
	ButtonSq {
		id: button5489_2727
		x: 767
		y: 629
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "120%"
		clickHandler.onClicked: onActivate5489()
	}
	function onActivate5490() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17416
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"125\" ")
	}
	StyledText {
		x: 767
		y: 683
		height: 72
		width: 275
		textLabel: "125%"
	}
	ButtonSq {
		id: button5490_2728
		x: 767
		y: 683
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "125%"
		clickHandler.onClicked: onActivate5490()
	}
	function onActivate5491() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17417
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"130\" ")
	}
	StyledText {
		x: 767
		y: 738
		height: 72
		width: 275
		textLabel: "130%"
	}
	ButtonSq {
		id: button5491_2729
		x: 767
		y: 738
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "130%"
		clickHandler.onClicked: onActivate5491()
	}
	function onActivate5492() 
	{
	 // Calling Command 917 (Set Zoom) with CommandGroup_Parameters from 17418
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 917 261 \"135\" ")
	}
	StyledText {
		x: 767
		y: 792
		height: 72
		width: 275
		textLabel: "135%"
	}
	ButtonSq {
		id: button5492_2730
		x: 767
		y: 792
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "135%"
		clickHandler.onClicked: onActivate5492()
	}
	function onActivate5494() 
	{
	 // Calling Command 916 (Set Aspect Ratio) with CommandGroup_Parameters from 17419
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 916 260 \"auto\" ")
	}
	StyledText {
		x: 1213
		y: 575
		height: 72
		width: 275
		textLabel: "Auto"
	}
	ButtonSq {
		id: button5494_2731
		x: 1213
		y: 575
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "Auto"
		clickHandler.onClicked: onActivate5494()
	}
	function onActivate5495() 
	{
	 // Calling Command 916 (Set Aspect Ratio) with CommandGroup_Parameters from 17420
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 916 260 \"1:1\" ")
	}
	StyledText {
		x: 1213
		y: 629
		height: 72
		width: 275
		textLabel: "1:1"
	}
	ButtonSq {
		id: button5495_2732
		x: 1213
		y: 629
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "1:1"
		clickHandler.onClicked: onActivate5495()
	}
	function onActivate5496() 
	{
	 // Calling Command 916 (Set Aspect Ratio) with CommandGroup_Parameters from 17421
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 916 260 \"4:3\" ")
	}
	StyledText {
		x: 1213
		y: 683
		height: 72
		width: 275
		textLabel: "4:3"
	}
	ButtonSq {
		id: button5496_2733
		x: 1213
		y: 683
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "4:3"
		clickHandler.onClicked: onActivate5496()
	}
	function onActivate5497() 
	{
	 // Calling Command 916 (Set Aspect Ratio) with CommandGroup_Parameters from 17422
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 916 260 \"16:9\" ")
	}
	StyledText {
		x: 1213
		y: 738
		height: 72
		width: 275
		textLabel: "16:9"
	}
	ButtonSq {
		id: button5497_2734
		x: 1213
		y: 738
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "16:9"
		clickHandler.onClicked: onActivate5497()
	}
	function onActivate5498() 
	{
	 // Calling Command 916 (Set Aspect Ratio) with CommandGroup_Parameters from 17423
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 916 260 \"2.11:1\" ")
	}
	StyledText {
		x: 1213
		y: 792
		height: 72
		width: 275
		textLabel: "2.11:1"
	}
	ButtonSq {
		id: button5498_2735
		x: 1213
		y: 792
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/Buttons/main%20menu%20act.png"
		buttontext: "2.11:1"
		clickHandler.onClicked: onActivate5498()
	}
	function onActivate1785() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17174
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 1678
		y: 0
		height: 1
		width: 1
		textLabel: "Main menu"
	}
	ButtonSq {
		id: button1785_2736
		x: 1678
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
		id: button1784_2737
		x: 1678
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
}