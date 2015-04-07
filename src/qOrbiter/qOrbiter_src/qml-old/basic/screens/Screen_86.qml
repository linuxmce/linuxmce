// UI1 Screen FileList - Popup_Container (86)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/popup_leon.png"
		width: 1920
		height: 1080
	}
	ButtonSq {
		id: button3511_1020
		x: 63
		y: 47
		width: 1543
		height: 1080
		buttonbackground: "skins/Basic/Backgrounds/filelist_popup.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3511()
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
		x: 253
		y: 189
		height: 120
		width: 90
		textLabel: "AB"
	}
	ButtonSq {
		id: button5344_1021
		x: 253
		y: 189
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
		x: 253
		y: 279
		height: 120
		width: 90
		textLabel: "CD"
	}
	ButtonSq {
		id: button5345_1022
		x: 253
		y: 279
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
		x: 253
		y: 369
		height: 120
		width: 90
		textLabel: "EF"
	}
	ButtonSq {
		id: button5346_1023
		x: 253
		y: 369
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
		x: 253
		y: 459
		height: 120
		width: 90
		textLabel: "GH"
	}
	ButtonSq {
		id: button5347_1024
		x: 253
		y: 459
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
		x: 253
		y: 549
		height: 120
		width: 90
		textLabel: "IJ"
	}
	ButtonSq {
		id: button5348_1025
		x: 253
		y: 549
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
		x: 253
		y: 639
		height: 120
		width: 90
		textLabel: "KL"
	}
	ButtonSq {
		id: button5349_1026
		x: 253
		y: 639
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
		x: 253
		y: 729
		height: 120
		width: 90
		textLabel: "MN"
	}
	ButtonSq {
		id: button5350_1027
		x: 253
		y: 729
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
		x: 253
		y: 819
		height: 120
		width: 90
		textLabel: "OPQ"
	}
	ButtonSq {
		id: button5351_1028
		x: 253
		y: 819
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
		x: 253
		y: 909
		height: 120
		width: 90
		textLabel: "RS"
	}
	ButtonSq {
		id: button5352_1029
		x: 253
		y: 909
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
		x: 253
		y: 999
		height: 120
		width: 90
		textLabel: "TU"
	}
	ButtonSq {
		id: button5353_1030
		x: 253
		y: 999
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
		x: 253
		y: 1089
		height: 120
		width: 90
		textLabel: "VW"
	}
	ButtonSq {
		id: button5354_1031
		x: 253
		y: 1089
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
		x: 253
		y: 1179
		height: 120
		width: 90
		textLabel: "XYZ"
	}
	ButtonSq {
		id: button5355_1032
		x: 253
		y: 1179
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "XYZ"
		clickHandler.onClicked: onActivate5355()
	}
	function onActivate1597() 
	{
	}
	ButtonSq {
		id: button1597_1033
		x: 560
		y: 68
		width: 1020
		height: 1042
		buttonbackground: "skins/Basic/7"
		buttontext: ""
		clickHandler.onClicked: onActivate1597()
	}
	function onActivate3514() 
	{
	 // Calling Command 398 (Remove Popup) with CommandGroup_Parameters from 14956
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 398 16 \"<%=S%>\" 50 \"filelist\" ")
	}
	function onActivate3514() 
	{
	 // Calling Command 398 (Remove Popup) with CommandGroup_Parameters from 14956
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 398 16 \"<%=S%>\" 50 \"filelist\" ")
	}
	function onActivate3514() 
	{
	 // Calling Command 398 (Remove Popup) with CommandGroup_Parameters from 14956
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 398 16 \"<%=S%>\" 50 \"filelist\" ")
	}
	function onActivate3514() 
	{
	 // Calling Command 398 (Remove Popup) with CommandGroup_Parameters from 14956
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 398 16 \"<%=S%>\" 50 \"filelist\" ")
	}
}