// UI1 Screen AV Devices (201)
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
		id: button4569_2324
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
		id: button5067_2325
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
		id: button4658_2326
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Stop video"
		clickHandler.onClicked: onActivate4658()
	}
	function onActivate4581() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17116
		setCurrentScreen("Screen_247.qml")
	}
	StyledText {
		x: 1444
		y: 981
		height: 211
		width: 257
		textLabel: "No more A/V devices"
	}
	StyledText {
		x: 1444
		y: 914
		height: 90
		width: 346
		textLabel: "Next ->"
	}
	ButtonSq {
		id: button4581_2327
		x: 1444
		y: 914
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/next.png"
		buttontext: "Next ->"
		clickHandler.onClicked: onActivate4581()
	}
	function onActivate4582() 
	{
	 // Calling Command 5 (Goto DesignObj) with CommandGroup_Parameters from 16337
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 5 3 \"4676\" ")
	}
	StyledText {
		x: 1444
		y: 755
		height: 211
		width: 237
		textLabel: "Add Device"
	}
	ButtonSq {
		id: button4582_2328
		x: 1444
		y: 755
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/next.png"
		buttontext: "Add Device"
		clickHandler.onClicked: onActivate4582()
	}
	ButtonSq {
		id: button4762_2329
		x: 360
		y: 788
		width: 960
		height: 291
		buttonbackground: "skins/Basic/2"
		buttontext: ""
		clickHandler.onClicked: onActivate4762()
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
		id: button4681_2330
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
		id: button4622_2331
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
		id: button4642_2332
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
		id: button4646_2333
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
		id: button5084_2334
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
		id: button5320_2335
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
		id: button5321_2336
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
		id: button5322_2337
		x: 0
		y: 675
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Done"
		clickHandler.onClicked: onActivate5322()
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
		id: button5437_2338
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "KDE desktop"
		clickHandler.onClicked: onActivate5437()
	}
}