// UI1 Screen Which Wizard (231)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/wizard_smallXine.png"
		width: 1920
		height: 1080
	}
	function onActivate4570() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16437
		setCurrentScreen("Screen_193.qml")
	}
	StyledText {
		x: 45
		y: 135
		height: 211
		width: 346
		textLabel: "House Setup"
	}
	ButtonSq {
		id: button4570_2439
		x: 45
		y: 135
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "House Setup"
		clickHandler.onClicked: onActivate4570()
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
		id: button5067_2440
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareOn.png"
		buttontext: "Cancel Wizard"
		clickHandler.onClicked: onActivate5067()
	}
	function onActivate5082() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17035
		setCurrentScreen("Screen_232.qml")
	}
	StyledText {
		x: 45
		y: 609
		height: 211
		width: 346
		textLabel: "Media Player"
	}
	ButtonSq {
		id: button5082_2441
		x: 45
		y: 609
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Media Player"
		clickHandler.onClicked: onActivate5082()
	}
	StyledText {
		x: 1191
		y: 315
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
		x: 1191
		y: 361
		height: 120
		width: 270
		textLabel: "Names"
	}
	ButtonSq {
		id: button4619_2442
		x: 1191
		y: 361
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
		x: 1191
		y: 541
		height: 120
		width: 270
		textLabel: "Rooms"
	}
	ButtonSq {
		id: button4656_2443
		x: 1191
		y: 541
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
		x: 1191
		y: 631
		height: 120
		width: 270
		textLabel: "Lights"
	}
	ButtonSq {
		id: button4647_2444
		x: 1191
		y: 631
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
		x: 1191
		y: 811
		height: 120
		width: 270
		textLabel: "VOIP Provider"
	}
	ButtonSq {
		id: button4654_2445
		x: 1191
		y: 811
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
		x: 1191
		y: 451
		height: 120
		width: 270
		textLabel: "Location"
	}
	ButtonSq {
		id: button4679_2446
		x: 1191
		y: 451
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
		x: 1191
		y: 721
		height: 120
		width: 270
		textLabel: "Alarm Panel"
	}
	ButtonSq {
		id: button4682_2447
		x: 1191
		y: 721
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
		x: 1191
		y: 901
		height: 120
		width: 270
		textLabel: "PVR Software"
	}
	ButtonSq {
		id: button5318_2448
		x: 1191
		y: 901
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
		x: 1191
		y: 991
		height: 120
		width: 270
		textLabel: "Done"
	}
	ButtonSq {
		id: button5319_2449
		x: 1191
		y: 991
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Done"
		clickHandler.onClicked: onActivate5319()
	}
	StyledText {
		x: 1552
		y: 315
		height: 60
		width: 270
		textLabel: "Media Player"
	}
	function onActivate4681() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16439
		setCurrentScreen("Screen_239.qml")
	}
	StyledText {
		x: 1552
		y: 901
		height: 120
		width: 270
		textLabel: "Add Software"
	}
	ButtonSq {
		id: button4681_2450
		x: 1552
		y: 901
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Add Software"
		clickHandler.onClicked: onActivate4681()
	}
	function onActivate4622() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16443
		setCurrentScreen("Screen_196.qml")
	}
	StyledText {
		x: 1552
		y: 451
		height: 120
		width: 270
		textLabel: "TV"
	}
	ButtonSq {
		id: button4622_2451
		x: 1552
		y: 451
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "TV"
		clickHandler.onClicked: onActivate4622()
	}
	function onActivate4642() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16445
		setCurrentScreen("Screen_197.qml")
	}
	StyledText {
		x: 1552
		y: 541
		height: 120
		width: 270
		textLabel: "Receiver / Amp"
	}
	ButtonSq {
		id: button4642_2452
		x: 1552
		y: 541
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Receiver / Amp"
		clickHandler.onClicked: onActivate4642()
	}
	function onActivate4646() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16444
		setCurrentScreen("Screen_201.qml")
	}
	StyledText {
		x: 1552
		y: 631
		height: 120
		width: 270
		textLabel: "A/V devices"
	}
	ButtonSq {
		id: button4646_2453
		x: 1552
		y: 631
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "A/V devices"
		clickHandler.onClicked: onActivate4646()
	}
	function onActivate5084() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17039
		setCurrentScreen("Screen_232.qml")
	}
	StyledText {
		x: 1552
		y: 361
		height: 120
		width: 270
		textLabel: "Current Room"
	}
	ButtonSq {
		id: button5084_2454
		x: 1552
		y: 361
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Current Room"
		clickHandler.onClicked: onActivate5084()
	}
	function onActivate5320() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17298
		setCurrentScreen("Screen_247.qml")
	}
	StyledText {
		x: 1552
		y: 721
		height: 120
		width: 270
		textLabel: "Inputs"
	}
	ButtonSq {
		id: button5320_2455
		x: 1552
		y: 721
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Inputs"
		clickHandler.onClicked: onActivate5320()
	}
	function onActivate5321() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17297
		setCurrentScreen("Screen_245.qml")
	}
	StyledText {
		x: 1552
		y: 811
		height: 120
		width: 270
		textLabel: "Providers"
	}
	ButtonSq {
		id: button5321_2456
		x: 1552
		y: 811
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Providers"
		clickHandler.onClicked: onActivate5321()
	}
	function onActivate5322() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17299
		setCurrentScreen("Screen_236.qml")
	}
	StyledText {
		x: 1552
		y: 991
		height: 120
		width: 270
		textLabel: "Done"
	}
	ButtonSq {
		id: button5322_2457
		x: 1552
		y: 991
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Done"
		clickHandler.onClicked: onActivate5322()
	}
	function onActivate3558() 
	{
	}
	function onActivate5437() 
	{
	 // Calling Command 912 (Activate PC Desktop) with CommandGroup_Parameters from 17388
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 912 119 \"1\" ")
	}
	StyledText {
		x: 1065
		y: 0
		height: 240
		width: 180
		textLabel: "KDE desktop"
	}
	ButtonSq {
		id: button5437_2458
		x: 1065
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "KDE desktop"
		clickHandler.onClicked: onActivate5437()
	}
}