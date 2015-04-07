// UI1 Screen TV Manufacturer (196)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/wizard_smallXine.png"
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
		id: button4681_2255
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
		id: button4622_2256
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
		id: button4642_2257
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
		id: button4646_2258
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
		id: button5084_2259
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
		id: button5320_2260
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
		id: button5321_2261
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
		id: button5322_2262
		x: 0
		y: 675
		width: 360
		height: 90
		buttonbackground: "skins/Basic/Buttons/midsmallbutton.png"
		buttontext: "Done"
		clickHandler.onClicked: onActivate5322()
	}
	function onActivate3558() 
	{
	}
	function onActivate4745() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16624
		setCurrentScreen("Screen_197.qml")
	}
	StyledText {
		x: 1444
		y: 914
		height: 211
		width: 237
		textLabel: "don't control my TV"
	}
	ButtonSq {
		id: button4745_2263
		x: 1444
		y: 914
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/next.png"
		buttontext: "don't control my TV"
		clickHandler.onClicked: onActivate4745()
	}
	function onActivate5077() 
	{
	 // Calling Command 5 (Goto DesignObj) with CommandGroup_Parameters from 17034
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 5 3 \"4554\" ")
	}
	StyledText {
		x: 1444
		y: 755
		height: 211
		width: 237
		textLabel: "Use Infrared"
	}
	ButtonSq {
		id: button5077_2264
		x: 1444
		y: 755
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/next.png"
		buttontext: "Use Infrared"
		clickHandler.onClicked: onActivate5077()
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
		id: button4569_2265
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Please repeat"
		clickHandler.onClicked: onActivate4569()
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
		id: button4658_2266
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Stop video"
		clickHandler.onClicked: onActivate4658()
	}
	StyledText {
		x: 0
		y: 900
		height: 240
		width: 180
		textLabel: "Show direct connect models"
	}
	ButtonSq {
		id: button5078_2267
		x: 0
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "Show direct connect models"
		clickHandler.onClicked: onActivate5078()
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
		id: button5067_2268
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareOn.png"
		buttontext: "Cancel Wizard"
		clickHandler.onClicked: onActivate5067()
	}
}