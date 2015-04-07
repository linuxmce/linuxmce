// UI1 Screen Media Player Setup Popup Message (238)
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
		id: button3419_2510
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
		id: button3420_2511
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
		id: button3421_2512
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
		id: button3422_2513
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
		textLabel: "Media Player"
	}
	function onActivate4681() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16439
		setCurrentScreen("Screen_239.qml")
	}
	StyledText {
		x: 0
		y: 585
		height: 120
		width: 270
		textLabel: "Add Software"
	}
	ButtonSq {
		id: button4681_2514
		x: 0
		y: 585
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
		x: 0
		y: 135
		height: 120
		width: 270
		textLabel: "TV"
	}
	ButtonSq {
		id: button4622_2515
		x: 0
		y: 135
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
		x: 0
		y: 225
		height: 120
		width: 270
		textLabel: "Receiver / Amp"
	}
	ButtonSq {
		id: button4642_2516
		x: 0
		y: 225
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
		x: 0
		y: 315
		height: 120
		width: 270
		textLabel: "A/V devices"
	}
	ButtonSq {
		id: button4646_2517
		x: 0
		y: 315
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
		x: 0
		y: 45
		height: 120
		width: 270
		textLabel: "Current Room"
	}
	ButtonSq {
		id: button5084_2518
		x: 0
		y: 45
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
		x: 0
		y: 405
		height: 120
		width: 270
		textLabel: "Inputs"
	}
	ButtonSq {
		id: button5320_2519
		x: 0
		y: 405
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
		x: 0
		y: 495
		height: 120
		width: 270
		textLabel: "Providers"
	}
	ButtonSq {
		id: button5321_2520
		x: 0
		y: 495
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
		x: 0
		y: 675
		height: 120
		width: 270
		textLabel: "Done"
	}
	ButtonSq {
		id: button5322_2521
		x: 0
		y: 675
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Done"
		clickHandler.onClicked: onActivate5322()
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
		id: button5067_2522
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