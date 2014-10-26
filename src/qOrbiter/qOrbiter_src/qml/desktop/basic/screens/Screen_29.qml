// UI1 Screen Sleeping (29)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
	}
	ButtonSq {
		id: button3310_340
		x: 90
		y: 68
		width: 960
		height: 720
		buttonbackground: "skins/Basic/3"
		buttontext: ""
		clickHandler.onClicked: onActivate3310()
	}
	StyledText {
		x: 90
		y: 68
		height: 480
		width: 360
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button3313_341
		x: 90
		y: 68
		width: 480
		height: 360
		buttonbackground: "skins/Basic/Buttons/squared.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate3313()
	}
	ButtonSq {
		id: button3390_342
		x: 300
		y: 149
		width: 240
		height: 180
		buttonbackground: "skins/Basic/../../scenarios/<%=5:0%>.png|<%=IM%>"
		buttontext: ""
		clickHandler.onClicked: onActivate3390()
	}
	function onActivate3312() 
	{
	 // Calling Command 260 (Set Main Menu) with CommandGroup_Parameters from 14204
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 260 9 \"N\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14205
		setCurrentScreen("Screen_1.qml")
	}
	ButtonSq {
		id: button3312_343
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3312()
	}
	function onActivate3311() 
	{
	 // Calling Command 263 (Toggle Event Handler) with CommandGroup_Parameters from 14210
		manager.sendDceMessage(srouterip + " "+deviceid + " -109 1 263 107 \"<%=2%>\" ")
	 // Calling Command 35 (Populate Datagrid) with CommandGroup_Parameters from 14212
		manager.sendDceMessage(srouterip + " "+deviceid + " -102 1 35 15 \"Sleeping_Alarms_<%=!%>\" 38 \"29\" 39 \"<%=R%>\" ")
	}
	ButtonSq {
		id: button3311_344
		x: 1095
		y: 102
		width: 480
		height: 720
		buttonbackground: "skins/Basic/4"
		buttontext: ""
		clickHandler.onClicked: onActivate3311()
	}
}