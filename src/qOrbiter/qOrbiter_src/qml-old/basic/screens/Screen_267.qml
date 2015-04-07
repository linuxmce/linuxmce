// UI1 Screen Active Calls (267)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate5526() 
	{
	 // Calling Command 35 (Populate Datagrid) with CommandGroup_Parameters from 17515
		manager.sendDceMessage(srouterip + " "+deviceid + " -102 1 35 15 \"ACTIVECHANNELS_ON_CALL_<%=!%>\" 38 \"100\" 39 \"<%=59%>\" ")
	}
	ButtonSq {
		id: button5527_2777
		x: 632
		y: 180
		width: 960
		height: 677
		buttonbackground: "skins/Basic/6"
		buttontext: ""
		clickHandler.onClicked: onActivate5527()
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
		id: button1785_2778
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
		id: button1784_2779
		x: 1440
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate5528() 
	{
	}
	StyledText {
		x: 0
		y: 853
		height: 240
		width: 180
		textLabel: "Join"
	}
	ButtonSq {
		id: button5528_2780
		x: 0
		y: 853
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Join"
		clickHandler.onClicked: onActivate5528()
	}
	function onActivate5529() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17517
		setCurrentScreen("Screen_11.qml")
	}
	StyledText {
		x: 451
		y: 853
		height: 240
		width: 180
		textLabel: "Add to active Call"
	}
	ButtonSq {
		id: button5529_2781
		x: 451
		y: 853
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Add to active Call"
		clickHandler.onClicked: onActivate5529()
	}
	function onActivate5530() 
	{
	 // Calling Command 336 (Phone_Drop) with CommandGroup_Parameters from 17513
		manager.sendDceMessage(srouterip + " "+deviceid + " -107 1 336 ")
	}
	StyledText {
		x: 691
		y: 853
		height: 240
		width: 180
		textLabel: "Remove from active Call"
	}
	ButtonSq {
		id: button5530_2782
		x: 691
		y: 853
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Remove from active Call"
		clickHandler.onClicked: onActivate5530()
	}
	function onActivate5531() 
	{
	}
	StyledText {
		x: 1218
		y: 853
		height: 211
		width: 346
		textLabel: "Call in Progress"
	}
	ButtonSq {
		id: button5531_2783
		x: 1218
		y: 853
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Call in Progress"
		clickHandler.onClicked: onActivate5531()
	}
	StyledText {
		x: 1680
		y: 180
		height: 240
		width: 180
		textLabel: "Active Calls"
	}
	ButtonSq {
		id: button5535_2784
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard_on.png"
		buttontext: "Active Calls"
		clickHandler.onClicked: onActivate5535()
	}
	function onActivate1320() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2210
		setCurrentScreen("Screen_11.qml")
	}
	StyledText {
		x: 1678
		y: 540
		height: 240
		width: 180
		textLabel: "Dial
Direct"
	}
	ButtonSq {
		id: button1320_2785
		x: 1678
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Dial
Direct"
		clickHandler.onClicked: onActivate1320()
	}
	function onActivate1319() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2500
		setCurrentScreen("Screen_9.qml")
	}
	StyledText {
		x: 1680
		y: 360
		height: 240
		width: 180
		textLabel: "Phone
Book"
	}
	ButtonSq {
		id: button1319_2786
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Phone
Book"
		clickHandler.onClicked: onActivate1319()
	}
	function onActivate1321() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2207
		setCurrentScreen("Screen_10.qml")
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "Speed
Dial"
	}
	ButtonSq {
		id: button1321_2787
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Speed
Dial"
		clickHandler.onClicked: onActivate1321()
	}
	function onActivate1682() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2416
		setCurrentScreen("Screen_23.qml")
	}
	StyledText {
		x: 1680
		y: 900
		height: 240
		width: 180
		textLabel: "Call
Intercom"
	}
	ButtonSq {
		id: button1682_2788
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Call
Intercom"
		clickHandler.onClicked: onActivate1682()
	}
}