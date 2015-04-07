// UI1 Screen Game Screen FS Options (289)
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
		id: button1785_3014
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
		id: button1569_3015
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
	ButtonSq {
		id: button5624_3016
		x: 960
		y: 540
		width: 720
		height: 540
		buttonbackground: "skins/Basic/798"
		buttontext: ""
		clickHandler.onClicked: onActivate5624()
	}
	function onActivate1582() 
	{
	 // Calling Command 39 (Pause Media) with CommandGroup_Parameters from 14163
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 39 ")
	}
	ButtonSq {
		id: button1582_3017
		x: 1200
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/enter.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1582()
	}
	function onActivate5616() 
	{
	 // Calling Command 943 (Game 1P Start) with CommandGroup_Parameters from 17659
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 943 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17884
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 960
		y: 540
		height: 240
		width: 180
		textLabel: "P1"
	}
	ButtonSq {
		id: button5616_3018
		x: 960
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "P1"
		clickHandler.onClicked: onActivate5616()
	}
	function onActivate5617() 
	{
	 // Calling Command 944 (Game 2P Start) with CommandGroup_Parameters from 17660
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 944 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17885
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 1440
		y: 540
		height: 240
		width: 180
		textLabel: "P2"
	}
	ButtonSq {
		id: button5617_3019
		x: 1440
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tr.png"
		buttontext: "P2"
		clickHandler.onClicked: onActivate5617()
	}
	function onActivate5618() 
	{
	 // Calling Command 945 (Game 3P Start) with CommandGroup_Parameters from 17661
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 945 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17886
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 960
		y: 900
		height: 240
		width: 180
		textLabel: "P3"
	}
	ButtonSq {
		id: button5618_3020
		x: 960
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "P3"
		clickHandler.onClicked: onActivate5618()
	}
	function onActivate5619() 
	{
	 // Calling Command 946 (Game 4P Start) with CommandGroup_Parameters from 17662
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 946 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17887
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 1440
		y: 900
		height: 240
		width: 180
		textLabel: "P4"
	}
	ButtonSq {
		id: button5619_3021
		x: 1440
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/br.png"
		buttontext: "P4"
		clickHandler.onClicked: onActivate5619()
	}
	function onActivate5620() 
	{
	 // Calling Command 947 (Game Insert Coin) with CommandGroup_Parameters from 17663
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 947 ")
	}
	StyledText {
		x: 960
		y: 720
		height: 240
		width: 180
		textLabel: "Insert Coin"
	}
	ButtonSq {
		id: button5620_3022
		x: 960
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/roundblack.png"
		buttontext: "Insert Coin"
		clickHandler.onClicked: onActivate5620()
	}
	function onActivate5621() 
	{
	 // Calling Command 948 (Game Service) with CommandGroup_Parameters from 17664
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 948 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18232
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 1440
		y: 720
		height: 240
		width: 180
		textLabel: "Service"
	}
	ButtonSq {
		id: button5621_3023
		x: 1440
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/roundblack.png"
		buttontext: "Service"
		clickHandler.onClicked: onActivate5621()
	}
	function onActivate2289() 
	{
	 // Calling Command 97 (Mute) with CommandGroup_Parameters from 13657
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 97 ")
	}
	StyledText {
		x: 720
		y: 720
		height: 240
		width: 180
		textLabel: "Mute"
	}
	ButtonSq {
		id: button2289_3024
		x: 720
		y: 720
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
		id: button1850_3025
		x: 720
		y: 540
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
		id: button1851_3026
		x: 720
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1851()
	}
	function onActivate2441() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14732
		manager.sendDceMessage(srouterip + " "+deviceid + " -104 1 184 76 \"+10\" ")
	}
	ButtonSq {
		id: button2441_3027
		x: 480
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2441()
	}
	function onActivate2443() 
	{
	 // Calling Command 408 (Show Floorplan) with CommandGroup_Parameters from 15042
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 408 11 \"100\" 12 \"100\" 14 \"light\" ")
	}
	StyledText {
		x: 480
		y: 720
		height: 5
		width: 3
		textLabel: "Lights"
	}
	ButtonSq {
		id: button2443_3028
		x: 480
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_map.png"
		buttontext: "Lights"
		clickHandler.onClicked: onActivate2443()
	}
	function onActivate2442() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14733
		manager.sendDceMessage(srouterip + " "+deviceid + " -104 1 184 76 \"-10\" ")
	}
	ButtonSq {
		id: button2442_3029
		x: 480
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2442()
	}
	StyledText {
		x: 9
		y: 547
		height: 208
		width: 346
		textLabel: "<%=NP%>"
	}
	StyledText {
		x: 0
		y: 709
		height: 164
		width: 360
		textLabel: "<%=NP_SEC%>"
	}
	StyledText {
		x: 0
		y: 832
		height: 90
		width: 271
		textLabel: "<%=NP_TIME_SHORT%>"
	}
	StyledText {
		x: 361
		y: 832
		height: 90
		width: 89
		textLabel: "<%=NP_SPEED%>"
	}
	ButtonSq {
		id: button2455_3030
		x: 0
		y: 540
		width: 480
		height: 360
		buttonbackground: "skins/Basic/532"
		buttontext: "<%=NP_SPEED%>"
		clickHandler.onClicked: onActivate2455()
	}
	function onActivate3285() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14143
		setCurrentScreen("Screen_20.qml")
	}
	StyledText {
		x: 0
		y: 900
		height: 240
		width: 180
		textLabel: "Manage
playlist"
	}
	ButtonSq {
		id: button3285_3031
		x: 0
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Manage
playlist"
		clickHandler.onClicked: onActivate3285()
	}
	function onActivate3547() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 15050
		setCurrentScreen("Screen_87.qml")
	}
	StyledText {
		x: 240
		y: 900
		height: 240
		width: 180
		textLabel: "Book
marks"
	}
	ButtonSq {
		id: button3547_3032
		x: 240
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Book
marks"
		clickHandler.onClicked: onActivate3547()
	}
	function onActivate2338() 
	{
	 // Calling Command 65 (Jump Position In Playlist) with CommandGroup_Parameters from 13915
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 65 5 \"<%=35%>\" ")
	}
	function onActivate3558() 
	{
	}
	function onActivate5623() 
	{
	 // Calling Command 87 (Goto Media Menu) with CommandGroup_Parameters from 17666
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 87 64 \"1\" ")
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "Monitor Mode"
	}
	ButtonSq {
		id: button5623_3033
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Monitor Mode"
		clickHandler.onClicked: onActivate5623()
	}
	function onActivate5622() 
	{
	 // Calling Command 952 (Game Reset) with CommandGroup_Parameters from 17665
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 952 ")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "Reset"
	}
	ButtonSq {
		id: button5622_3034
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Reset"
		clickHandler.onClicked: onActivate5622()
	}
}