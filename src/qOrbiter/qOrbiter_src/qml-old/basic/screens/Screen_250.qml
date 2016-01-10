// UI1 Screen PVR Software (250)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/wizard_bigXine.png"
		width: 1920
		height: 1080
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
		id: button4619_2636
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
		id: button4656_2637
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
		id: button4647_2638
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
		id: button4654_2639
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
		id: button4679_2640
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
		id: button4682_2641
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
		id: button5318_2642
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
		id: button5319_2643
		x: 0
		y: 675
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Done"
		clickHandler.onClicked: onActivate5319()
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
		id: button5067_2644
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareOn.png"
		buttontext: "Cancel Wizard"
		clickHandler.onClicked: onActivate5067()
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
		id: button4658_2645
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Stop video"
		clickHandler.onClicked: onActivate4658()
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
		id: button4569_2646
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
	function onActivate5523() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17511
		setCurrentScreen("Screen_251.qml")
	}
	StyledText {
		x: 1444
		y: 755
		height: 211
		width: 346
		textLabel: "MythTV"
	}
	ButtonSq {
		id: button5523_2647
		x: 1444
		y: 755
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/next.png"
		buttontext: "MythTV"
		clickHandler.onClicked: onActivate5523()
	}
	function onActivate5524() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17512
		setCurrentScreen("Screen_251.qml")
	}
	StyledText {
		x: 1444
		y: 914
		height: 211
		width: 346
		textLabel: "VDR"
	}
	ButtonSq {
		id: button5524_2648
		x: 1444
		y: 914
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/next.png"
		buttontext: "VDR"
		clickHandler.onClicked: onActivate5524()
	}
}