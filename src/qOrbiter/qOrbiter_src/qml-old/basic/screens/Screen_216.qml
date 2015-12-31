// UI1 Screen TVConfirmInputsToggle (216)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
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
		id: button4681_2401
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
		id: button4622_2402
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
		id: button4642_2403
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
		id: button4646_2404
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
		id: button5084_2405
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
		id: button5320_2406
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
		id: button5321_2407
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
		id: button5322_2408
		x: 0
		y: 675
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Done"
		clickHandler.onClicked: onActivate5322()
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
		id: button4658_2409
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
		id: button4569_2410
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Please repeat"
		clickHandler.onClicked: onActivate4569()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_2411
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
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
		id: button1785_2412
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	ButtonSq {
		id: button5030_2413
		x: 18
		y: 460
		width: 1651
		height: 609
		buttonbackground: "skins/Basic/2"
		buttontext: ""
		clickHandler.onClicked: onActivate5030()
	}
	function onActivate4938() 
	{
	 // Calling Command 5 (Goto DesignObj) with CommandGroup_Parameters from 16902
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 5 3 \"4922\" ")
	}
	StyledText {
		x: 1444
		y: 900
		height: 211
		width: 346
		textLabel: "Next ->"
	}
	ButtonSq {
		id: button4938_2414
		x: 1444
		y: 900
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/next.png"
		buttontext: "Next ->"
		clickHandler.onClicked: onActivate4938()
	}
}