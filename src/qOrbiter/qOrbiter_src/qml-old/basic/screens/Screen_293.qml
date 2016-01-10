// UI1 Screen Game Atari 2600 Monitor Mode (293)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
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
		id: button1785_3049
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate1569() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13739
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18035
		setCurrentScreen("Screen_1.qml")
	}
	ButtonSq {
		id: button1569_3050
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
	function onActivate3351() 
	{
	 // Calling Command 29 (Simulate Mouse Click) with CommandGroup_Parameters from 14282
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 29 11 \"<%=XS%>\" 12 \"<%=YS%>\" ")
	}
	ButtonSq {
		id: button3351_3051
		x: 23
		y: 14
		width: 1642
		height: 886
		buttonbackground: "skins/Basic/<%=NPD%>"
		buttontext: ""
		clickHandler.onClicked: onActivate3351()
	}
	function onActivate2289() 
	{
	 // Calling Command 97 (Mute) with CommandGroup_Parameters from 13657
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 97 ")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "Mute"
	}
	ButtonSq {
		id: button2289_3052
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_mute.png"
		buttontext: "Mute"
		clickHandler.onClicked: onActivate2289()
	}
	function onActivate1850() 
	{
	 // Calling Command 89 (Vol Up) with CommandGroup_Parameters from 13671
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 89 ")
	}
	ButtonSq {
		id: button1850_3053
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1850()
	}
	function onActivate1851() 
	{
	 // Calling Command 90 (Vol Down) with CommandGroup_Parameters from 13672
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 90 ")
	}
	ButtonSq {
		id: button1851_3054
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1851()
	}
	function onActivate4296() 
	{
	 // Calling Command 200 (Move Up) with CommandGroup_Parameters from 16013
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 200 ")
	}
	ButtonSq {
		id: button4296_3055
		x: 240
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4296()
	}
	function onActivate4293() 
	{
	 // Calling Command 201 (Move Down) with CommandGroup_Parameters from 16014
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 201 ")
	}
	ButtonSq {
		id: button4293_3056
		x: 480
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4293()
	}
	function onActivate4294() 
	{
	 // Calling Command 202 (Move Left) with CommandGroup_Parameters from 16015
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 202 ")
	}
	ButtonSq {
		id: button4294_3057
		x: 720
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/left.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4294()
	}
	function onActivate4295() 
	{
	 // Calling Command 203 (Move Right) with CommandGroup_Parameters from 16012
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 203 ")
	}
	ButtonSq {
		id: button4295_3058
		x: 960
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/right.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4295()
	}
	function onActivate1662() 
	{
	 // Calling Command 41 (Change Playback Speed) with CommandGroup_Parameters from 14288
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 41 43 \"+1000\" ")
	 // Calling Command 190 (Enter/Go) with CommandGroup_Parameters from 14346
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 190 ")
	}
	StyledText {
		x: 1198
		y: 900
		height: 1
		width: 1
		textLabel: "Play"
	}
	ButtonSq {
		id: button1662_3059
		x: 1198
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/play.png"
		buttontext: "Play"
		clickHandler.onClicked: onActivate1662()
	}
	function onActivate5654() 
	{
	 // Calling Command 87 (Goto Media Menu) with CommandGroup_Parameters from 17859
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 87 64 \"0\" ")
	}
	ButtonSq {
		id: button5654_3060
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5654()
	}
	function onActivate5775() 
	{
	 // Calling Command 950 (Game Select) with CommandGroup_Parameters from 17865
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 950 ")
	}
	StyledText {
		x: 0
		y: 900
		height: 240
		width: 180
		textLabel: "Select"
	}
	ButtonSq {
		id: button5775_3061
		x: 0
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "Select"
		clickHandler.onClicked: onActivate5775()
	}
	function onActivate5616() 
	{
	 // Calling Command 943 (Game 1P Start) with CommandGroup_Parameters from 17659
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 943 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17884
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 1440
		y: 900
		height: 240
		width: 180
		textLabel: "P1"
	}
	ButtonSq {
		id: button5616_3062
		x: 1440
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "P1"
		clickHandler.onClicked: onActivate5616()
	}
}