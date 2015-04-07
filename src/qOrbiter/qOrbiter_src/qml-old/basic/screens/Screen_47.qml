// UI1 Screen FileList_Music_Movies_Video (47)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Buttons/Keyboard/background.png"
		width: 1920
		height: 1080
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
		id: button1785_545
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate2552() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16962
		setCurrentScreen("Screen_220.qml")
	}
	StyledText {
		x: 1680
		y: 180
		height: 240
		width: 180
		textLabel: "Remote"
	}
	ButtonSq {
		id: button2552_546
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/s1.png"
		buttontext: "Remote"
		clickHandler.onClicked: onActivate2552()
	}
	function onActivate3223() 
	{
	 // Calling Command 17 (Seek Data Grid) with CommandGroup_Parameters from 14008
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 17 9 \"<%=17%>\" 15 \"MediaFile_<%=!%>\" ")
	}
	function onActivate5344() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 17323
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"A\" ")
	}
	StyledText {
		x: 0
		y: 0
		height: 120
		width: 90
		textLabel: "AB"
	}
	ButtonSq {
		id: button5344_547
		x: 0
		y: 0
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "AB"
		clickHandler.onClicked: onActivate5344()
	}
	function onActivate5345() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 17322
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"C\" ")
	}
	StyledText {
		x: 0
		y: 90
		height: 120
		width: 90
		textLabel: "CD"
	}
	ButtonSq {
		id: button5345_548
		x: 0
		y: 90
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "CD"
		clickHandler.onClicked: onActivate5345()
	}
	function onActivate5346() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 17321
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"E\" ")
	}
	StyledText {
		x: 0
		y: 180
		height: 120
		width: 90
		textLabel: "EF"
	}
	ButtonSq {
		id: button5346_549
		x: 0
		y: 180
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "EF"
		clickHandler.onClicked: onActivate5346()
	}
	function onActivate5347() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 17320
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"G\" ")
	}
	StyledText {
		x: 0
		y: 270
		height: 120
		width: 90
		textLabel: "GH"
	}
	ButtonSq {
		id: button5347_550
		x: 0
		y: 270
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "GH"
		clickHandler.onClicked: onActivate5347()
	}
	function onActivate5348() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 17319
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"I\" ")
	}
	StyledText {
		x: 0
		y: 360
		height: 120
		width: 90
		textLabel: "IJ"
	}
	ButtonSq {
		id: button5348_551
		x: 0
		y: 360
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "IJ"
		clickHandler.onClicked: onActivate5348()
	}
	function onActivate5349() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 17330
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"K\" ")
	}
	StyledText {
		x: 0
		y: 450
		height: 120
		width: 90
		textLabel: "KL"
	}
	ButtonSq {
		id: button5349_552
		x: 0
		y: 450
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "KL"
		clickHandler.onClicked: onActivate5349()
	}
	function onActivate5350() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 17329
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"M\" ")
	}
	StyledText {
		x: 0
		y: 540
		height: 120
		width: 90
		textLabel: "MN"
	}
	ButtonSq {
		id: button5350_553
		x: 0
		y: 540
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "MN"
		clickHandler.onClicked: onActivate5350()
	}
	function onActivate5351() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 17328
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"O\" ")
	}
	StyledText {
		x: 0
		y: 630
		height: 120
		width: 90
		textLabel: "OPQ"
	}
	ButtonSq {
		id: button5351_554
		x: 0
		y: 630
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "OPQ"
		clickHandler.onClicked: onActivate5351()
	}
	function onActivate5352() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 17326
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"R\" ")
	}
	StyledText {
		x: 0
		y: 720
		height: 120
		width: 90
		textLabel: "RS"
	}
	ButtonSq {
		id: button5352_555
		x: 0
		y: 720
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "RS"
		clickHandler.onClicked: onActivate5352()
	}
	function onActivate5353() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 17327
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"T\" ")
	}
	StyledText {
		x: 0
		y: 810
		height: 120
		width: 90
		textLabel: "TU"
	}
	ButtonSq {
		id: button5353_556
		x: 0
		y: 810
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "TU"
		clickHandler.onClicked: onActivate5353()
	}
	function onActivate5354() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 17325
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"V\" ")
	}
	StyledText {
		x: 0
		y: 900
		height: 120
		width: 90
		textLabel: "VW"
	}
	ButtonSq {
		id: button5354_557
		x: 0
		y: 900
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "VW"
		clickHandler.onClicked: onActivate5354()
	}
	function onActivate5355() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 17324
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"X\" ")
	}
	StyledText {
		x: 0
		y: 990
		height: 120
		width: 90
		textLabel: "XYZ"
	}
	ButtonSq {
		id: button5355_558
		x: 0
		y: 990
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "XYZ"
		clickHandler.onClicked: onActivate5355()
	}
	function onActivate4949() 
	{
	 // Calling Command 398 (Remove Popup) with CommandGroup_Parameters from 16912
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 398 50 \"mediabrowser\" ")
	}
	ButtonSq {
		id: button4949_559
		x: 963
		y: 0
		width: 722
		height: 1083
		buttonbackground: "skins/Basic/8"
		buttontext: ""
		clickHandler.onClicked: onActivate4949()
	}
	function onActivate5358() 
	{
	 // Calling Command 9 (Scroll Grid) with CommandGroup_Parameters from 17331
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 9 3 \"4949\" 30 \"1\" ")
	}
	ButtonSq {
		id: button5358_560
		x: 1680
		y: 900
		width: 240
		height: 90
		buttonbackground: "skins/Basic/Buttons/TabletControls/up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5358()
	}
	function onActivate5357() 
	{
	 // Calling Command 9 (Scroll Grid) with CommandGroup_Parameters from 17332
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 9 3 \"4949\" 30 \"2\" ")
	}
	ButtonSq {
		id: button5357_561
		x: 1680
		y: 990
		width: 240
		height: 90
		buttonbackground: "skins/Basic/Buttons/TabletControls/down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5357()
	}
	ButtonSq {
		id: button5086_562
		x: 120
		y: 0
		width: 843
		height: 1083
		buttonbackground: "skins/Basic/2"
		buttontext: ""
		clickHandler.onClicked: onActivate5086()
	}
	function onActivate5359() 
	{
	 // Calling Command 5 (Goto DesignObj) with CommandGroup_Parameters from 17336
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 5 3 \"5362.<%=MTB%>.0\" ")
	}
	StyledText {
		x: 1680
		y: 360
		height: 240
		width: 180
		textLabel: "Options"
	}
	ButtonSq {
		id: button5359_563
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/s2.png"
		buttontext: "Options"
		clickHandler.onClicked: onActivate5359()
	}
	function onActivate2554() 
	{
	 // Calling Command 43 (MH Play Media) with CommandGroup_Parameters from 13878
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 43 13 \"!G<%=!%>\" 45 \"<%=E%>\" 253 \"<%=DD282%>\" ")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "Play All"
	}
	ButtonSq {
		id: button2554_564
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/s3.png"
		buttontext: "Play All"
		clickHandler.onClicked: onActivate2554()
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "<- Back"
	}
	ButtonSq {
		id: button5175_565
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/squareb.png"
		buttontext: "<- Back"
		clickHandler.onClicked: onActivate5175()
	}
}