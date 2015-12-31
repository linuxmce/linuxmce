// UI1 Screen UserStatus (8)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/userstatusB.png"
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
		id: button1785_51
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate5178() 
	{
	 // Calling Command 43 (MH Play Media) with CommandGroup_Parameters from 17803
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 43 13 \"<%=2%>\" 29 \"4\" 45 \"<%=E%>\" ")
	}
	ButtonSq {
		id: button5178_52
		x: 97
		y: 292
		width: 915
		height: 720
		buttonbackground: "skins/Basic/10"
		buttontext: ""
		clickHandler.onClicked: onActivate5178()
	}
	function onActivate1569() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13739
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18035
		setCurrentScreen("Screen_1.qml")
	}
	ButtonSq {
		id: button1569_53
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
	StyledText {
		x: 1050
		y: 60
		height: 120
		width: 360
		textLabel: "<%=NP%>"
	}
	StyledText {
		x: 1050
		y: 150
		height: 240
		width: 360
		textLabel: "<%=NP_TIME_SHORT%>"
	}
	ButtonSq {
		id: button5703_54
		x: 1050
		y: 60
		width: 480
		height: 180
		buttonbackground: "skins/Basic/532"
		buttontext: "<%=NP_TIME_SHORT%>"
		clickHandler.onClicked: onActivate5703()
	}
	function onActivate2456() 
	{
	}
	function onActivate1572() 
	{
	 // Calling Command 41 (Change Playback Speed) with CommandGroup_Parameters from 14165
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 41 43 \"-2\" ")
	}
	ButtonSq {
		id: button1572_55
		x: 1036
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_left.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1572()
	}
	function onActivate1575() 
	{
	 // Calling Command 65 (Jump Position In Playlist) with CommandGroup_Parameters from 13767
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 65 5 \"-1\" ")
	}
	ButtonSq {
		id: button1575_56
		x: 1276
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1575()
	}
	function onActivate1576() 
	{
	 // Calling Command 65 (Jump Position In Playlist) with CommandGroup_Parameters from 13768
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 65 5 \"+1\" ")
	}
	ButtonSq {
		id: button1576_57
		x: 1276
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1576()
	}
	function onActivate1581() 
	{
	 // Calling Command 41 (Change Playback Speed) with CommandGroup_Parameters from 14164
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 41 43 \"+2\" ")
	}
	ButtonSq {
		id: button1581_58
		x: 1516
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_right.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1581()
	}
	function onActivate1582() 
	{
	 // Calling Command 39 (Pause Media) with CommandGroup_Parameters from 14163
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 39 ")
	}
	ButtonSq {
		id: button1582_59
		x: 1270
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/enter.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1582()
	}
	function onActivate1296() 
	{
	 // Calling Command 744 (Set User Mode) with CommandGroup_Parameters from 16693
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 744 17 \"<%=39%>\" 194 \"1\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16701
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 108
		y: 68
		height: 236
		width: 160
		textLabel: "At home"
	}
	ButtonSq {
		id: button1296_60
		x: 108
		y: 68
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Buttons/radioOff.png"
		buttontext: "At home"
		clickHandler.onClicked: onActivate1296()
	}
	ButtonSq {
		id: button1297_61
		x: 108
		y: 68
		width: 213
		height: 176
		buttonbackground: "skins/Basic/Icons/modes/athome.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1297()
	}
	function onActivate1298() 
	{
	 // Calling Command 744 (Set User Mode) with CommandGroup_Parameters from 16695
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 744 17 \"<%=39%>\" 194 \"2\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16703
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 325
		y: 68
		height: 236
		width: 160
		textLabel: "Away"
	}
	ButtonSq {
		id: button1298_62
		x: 325
		y: 68
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Buttons/radioOff.png"
		buttontext: "Away"
		clickHandler.onClicked: onActivate1298()
	}
	ButtonSq {
		id: button1299_63
		x: 325
		y: 68
		width: 213
		height: 176
		buttonbackground: "skins/Basic/Icons/modes/awayfromhome.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1299()
	}
	function onActivate1300() 
	{
	 // Calling Command 744 (Set User Mode) with CommandGroup_Parameters from 16697
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 744 17 \"<%=39%>\" 194 \"3\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16705
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 541
		y: 68
		height: 236
		width: 160
		textLabel: "Sleeping"
	}
	ButtonSq {
		id: button1300_64
		x: 541
		y: 68
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Buttons/radioOff.png"
		buttontext: "Sleeping"
		clickHandler.onClicked: onActivate1300()
	}
	ButtonSq {
		id: button1301_65
		x: 541
		y: 68
		width: 213
		height: 176
		buttonbackground: "skins/Basic/Icons/modes/sleep.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1301()
	}
	function onActivate1302() 
	{
	 // Calling Command 744 (Set User Mode) with CommandGroup_Parameters from 16699
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 744 17 \"<%=39%>\" 194 \"4\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16707
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 758
		y: 68
		height: 236
		width: 160
		textLabel: "Do not
disturb"
	}
	ButtonSq {
		id: button1302_66
		x: 758
		y: 68
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Buttons/radioOff.png"
		buttontext: "Do not
disturb"
		clickHandler.onClicked: onActivate1302()
	}
	ButtonSq {
		id: button1303_67
		x: 758
		y: 68
		width: 213
		height: 176
		buttonbackground: "skins/Basic/Icons/modes/donotdisturb.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1303()
	}
}