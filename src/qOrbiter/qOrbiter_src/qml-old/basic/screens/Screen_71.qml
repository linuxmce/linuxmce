// UI1 Screen FileList_Pictures_Docs (71)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Buttons/Keyboard/background.png"
		width: 1920
		height: 1080
	}
	function onActivate1597() 
	{
	}
	ButtonSq {
		id: button1597_903
		x: 497
		y: 20
		width: 1164
		height: 1039
		buttonbackground: "skins/Basic/7"
		buttontext: ""
		clickHandler.onClicked: onActivate1597()
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
		id: button1785_904
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
		id: button2552_905
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/s1.png"
		buttontext: "Remote"
		clickHandler.onClicked: onActivate2552()
	}
	function onActivate3283() 
	{
	 // Calling Command 43 (MH Play Media) with CommandGroup_Parameters from 14137
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 43 13 \"<%=7%>\" 29 \"<%=14%>\" 45 \"<%=E%>\" ")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "View"
	}
	ButtonSq {
		id: button3283_906
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "View"
		clickHandler.onClicked: onActivate3283()
	}
	function onActivate2447() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 12619
		setCurrentScreen("Screen_65.qml")
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "Search"
	}
	ButtonSq {
		id: button2447_907
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/s4.png"
		buttontext: "Search"
		clickHandler.onClicked: onActivate2447()
	}
	function onActivate1569() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13739
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18035
		setCurrentScreen("Screen_1.qml")
	}
	ButtonSq {
		id: button1569_908
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
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
		id: button5344_909
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
		id: button5345_910
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
		id: button5346_911
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
		id: button5347_912
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
		id: button5348_913
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
		id: button5349_914
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
		id: button5350_915
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
		id: button5351_916
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
		id: button5352_917
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
		id: button5353_918
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
		id: button5354_919
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
		id: button5355_920
		x: 0
		y: 990
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "XYZ"
		clickHandler.onClicked: onActivate5355()
	}
}