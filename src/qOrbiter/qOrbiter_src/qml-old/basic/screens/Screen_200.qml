// UI1 Screen VOIP Provider (200)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/wizard_smallXine.png"
		width: 1920
		height: 1080
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
		id: button4569_2306
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Please repeat"
		clickHandler.onClicked: onActivate4569()
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
		id: button5067_2307
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareOn.png"
		buttontext: "Cancel Wizard"
		clickHandler.onClicked: onActivate5067()
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
		id: button4619_2308
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
		id: button4656_2309
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
		id: button4647_2310
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
		id: button4654_2311
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
		id: button4679_2312
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
		id: button4682_2313
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
		id: button5318_2314
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
		id: button5319_2315
		x: 0
		y: 675
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Done"
		clickHandler.onClicked: onActivate5319()
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
		id: button4658_2316
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Stop video"
		clickHandler.onClicked: onActivate4658()
	}
	function onActivate4706() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 16550
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"33\" 5 \"broadvoice\" ")
	 // Calling Command 5 (Goto DesignObj) with CommandGroup_Parameters from 16578
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 5 3 \"4627\" ")
	}
	StyledText {
		x: 360
		y: 180
		height: 211
		width: 346
		textLabel: "Broadvoice"
	}
	ButtonSq {
		id: button4706_2317
		x: 360
		y: 180
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Broadvoice"
		clickHandler.onClicked: onActivate4706()
	}
	function onActivate4707() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 16551
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"33\" 5 \"freeworlddialup\" ")
	 // Calling Command 5 (Goto DesignObj) with CommandGroup_Parameters from 16579
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 5 3 \"4627\" ")
	}
	StyledText {
		x: 360
		y: 338
		height: 211
		width: 346
		textLabel: "Free World Dialup"
	}
	ButtonSq {
		id: button4707_2318
		x: 360
		y: 338
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Free World Dialup"
		clickHandler.onClicked: onActivate4707()
	}
	function onActivate4708() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 16552
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"33\" 5 \"inphonex\" ")
	 // Calling Command 5 (Goto DesignObj) with CommandGroup_Parameters from 16580
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 5 3 \"4627\" ")
	}
	StyledText {
		x: 360
		y: 497
		height: 211
		width: 346
		textLabel: "InPhonex"
	}
	ButtonSq {
		id: button4708_2319
		x: 360
		y: 497
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "InPhonex"
		clickHandler.onClicked: onActivate4708()
	}
	function onActivate4709() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 16553
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"33\" 5 \"efon\" ")
	 // Calling Command 5 (Goto DesignObj) with CommandGroup_Parameters from 16581
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 5 3 \"4627\" ")
	}
	StyledText {
		x: 360
		y: 655
		height: 211
		width: 346
		textLabel: "E-fon"
	}
	ButtonSq {
		id: button4709_2320
		x: 360
		y: 655
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "E-fon"
		clickHandler.onClicked: onActivate4709()
	}
	function onActivate4710() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 16554
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"33\" 5 \"teliax-out\" ")
	 // Calling Command 5 (Goto DesignObj) with CommandGroup_Parameters from 16582
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 5 3 \"4627\" ")
	}
	StyledText {
		x: 360
		y: 813
		height: 211
		width: 346
		textLabel: "Teliax"
	}
	ButtonSq {
		id: button4710_2321
		x: 360
		y: 813
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Teliax"
		clickHandler.onClicked: onActivate4710()
	}
	function onActivate5326() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17301
		setCurrentScreen("Screen_250.qml")
	}
	StyledText {
		x: 1444
		y: 914
		height: 211
		width: 237
		textLabel: "Next ->"
	}
	ButtonSq {
		id: button5326_2322
		x: 1444
		y: 914
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/next.png"
		buttontext: "Next ->"
		clickHandler.onClicked: onActivate5326()
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
		id: button5437_2323
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "KDE desktop"
		clickHandler.onClicked: onActivate5437()
	}
}