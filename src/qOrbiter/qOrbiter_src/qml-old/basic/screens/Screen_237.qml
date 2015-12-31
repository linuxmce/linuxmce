// UI1 Screen House Setup Popup Message (237)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/wizard_smallXine.png"
		width: 1920
		height: 1080
	}
	function onActivate3419() 
	{
	 // Calling Command 389 (Send Message) with CommandGroup_Parameters from 14828
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 389 9 \"<%=46%>\" 144 \"1\" ")
	}
	StyledText {
		x: 442
		y: 555
		height: 211
		width: 346
		textLabel: "<%=0:1%>"
	}
	ButtonSq {
		id: button3419_2497
		x: 442
		y: 555
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "<%=0:1%>"
		clickHandler.onClicked: onActivate3419()
	}
	function onActivate3420() 
	{
	 // Calling Command 389 (Send Message) with CommandGroup_Parameters from 14829
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 389 9 \"<%=47%>\" 144 \"1\" ")
	}
	StyledText {
		x: 965
		y: 555
		height: 211
		width: 346
		textLabel: "<%=0:1%>"
	}
	ButtonSq {
		id: button3420_2498
		x: 965
		y: 555
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "<%=0:1%>"
		clickHandler.onClicked: onActivate3420()
	}
	function onActivate3421() 
	{
	 // Calling Command 389 (Send Message) with CommandGroup_Parameters from 14830
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 389 9 \"<%=48%>\" 144 \"1\" ")
	}
	StyledText {
		x: 442
		y: 812
		height: 211
		width: 346
		textLabel: "<%=0:1%>"
	}
	ButtonSq {
		id: button3421_2499
		x: 442
		y: 812
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "<%=0:1%>"
		clickHandler.onClicked: onActivate3421()
	}
	function onActivate3422() 
	{
	 // Calling Command 389 (Send Message) with CommandGroup_Parameters from 14831
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 389 9 \"<%=49%>\" 144 \"1\" ")
	}
	StyledText {
		x: 965
		y: 812
		height: 211
		width: 346
		textLabel: "<%=0:1%>"
	}
	ButtonSq {
		id: button3422_2500
		x: 965
		y: 812
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "<%=0:1%>"
		clickHandler.onClicked: onActivate3422()
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
		id: button4619_2501
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
		id: button4656_2502
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
		id: button4647_2503
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
		id: button4654_2504
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
		id: button4679_2505
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
		id: button4682_2506
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
		id: button5318_2507
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
		id: button5319_2508
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
		id: button5067_2509
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareOn.png"
		buttontext: "Cancel Wizard"
		clickHandler.onClicked: onActivate5067()
	}
	function onActivate3558() 
	{
	}
}