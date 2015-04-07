// UI1 Screen TryCodes (73)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
	}
	function onActivate3300() 
	{
	 // Calling Command 35 (Populate Datagrid) with CommandGroup_Parameters from 14182
		manager.sendDceMessage(srouterip + " "+deviceid + " -102 1 35 10 \"irgrp_<%=!%>\" 15 \"irgrp_<%=!%>\" 38 \"26\" ")
	 // Calling Command 35 (Populate Datagrid) with CommandGroup_Parameters from 14197
		manager.sendDceMessage(srouterip + " "+deviceid + " -102 1 35 15 \"irtype_<%=!%>\" 38 \"27\" 39 \"<%=12%>\" ")
	 // Calling Command 35 (Populate Datagrid) with CommandGroup_Parameters from 14200
		manager.sendDceMessage(srouterip + " "+deviceid + " -102 1 35 15 \"ircodes_<%=!%>\" 38 \"28\" ")
	}
	function onActivate3306() 
	{
	 // Calling Command 35 (Populate Datagrid) with CommandGroup_Parameters from 14198
		manager.sendDceMessage(srouterip + " "+deviceid + " -102 1 35 15 \"irgrp_<%=!%>\" 38 \"26\" 39 \"<%=12%>,<%=13%>\" ")
	 // Calling Command 35 (Populate Datagrid) with CommandGroup_Parameters from 14199
		manager.sendDceMessage(srouterip + " "+deviceid + " -102 1 35 15 \"ircodes_<%=!%>\" 38 \"28\" ")
	}
	function onActivate3301() 
	{
	 // Calling Command 35 (Populate Datagrid) with CommandGroup_Parameters from 14183
		manager.sendDceMessage(srouterip + " "+deviceid + " -102 1 35 15 \"ircodes_<%=!%>\" 38 \"28\" 39 \"<%=2%>,<%=31%>\" ")
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
		id: button1785_922
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_923
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
}