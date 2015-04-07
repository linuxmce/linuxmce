// UI1 Screen Main (1)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/mainmenu.png"
		width: 1920
		height: 1080
	}
	function onActivate2436() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 12457
		setCurrentScreen("Screen_64.qml")
	}
	ButtonSq {
		id: button2436_1
		x: 1559
		y: 901
		width: 360
		height: 180
		buttonbackground: "skins/Basic/Buttons/user_room.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2436()
	}
	function onActivate1687() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14091
		setCurrentScreen("Screen_24.qml")
	}
	StyledText {
		x: 1199
		y: 901
		height: 240
		width: 270
		textLabel: "Select
Current
User"
	}
	ButtonSq {
		id: button1687_2
		x: 1199
		y: 901
		width: 360
		height: 180
		buttonbackground: "skins/Basic/Buttons/user_room.png"
		buttontext: "Select
Current
User"
		clickHandler.onClicked: onActivate1687()
	}
	function onActivate3308() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14201
		setCurrentScreen("Screen_39.qml")
	}
	StyledText {
		x: 239
		y: 900
		height: 240
		width: 180
		textLabel: "Computing"
	}
	ButtonSq {
		id: button3308_3
		x: 239
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Other/squareb.png"
		buttontext: "Computing"
		clickHandler.onClicked: onActivate3308()
	}
	function onActivate3309() 
	{
	 // Calling Command 260 (Set Main Menu) with CommandGroup_Parameters from 14202
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 260 9 \"S\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14203
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 479
		y: 900
		height: 240
		width: 180
		textLabel: "Sleeping menu"
	}
	ButtonSq {
		id: button3309_4
		x: 479
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Other/squareb.png"
		buttontext: "Sleeping menu"
		clickHandler.onClicked: onActivate3309()
	}
	function onActivate3320() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14255
		setCurrentScreen("Screen_76.qml")
	}
	StyledText {
		x: 719
		y: 900
		height: 240
		width: 180
		textLabel: "Power"
	}
	ButtonSq {
		id: button3320_5
		x: 719
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Other/squareb.png"
		buttontext: "Power"
		clickHandler.onClicked: onActivate3320()
	}
	StyledText {
		x: 960
		y: 914
		height: 90
		width: 180
		textLabel: "<%=CLOCK%>"
	}
	StyledText {
		x: 960
		y: 968
		height: 90
		width: 180
		textLabel: "Date:"
	}
	ButtonSq {
		id: button3321_6
		x: 960
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Other/squareb.png"
		buttontext: "Date: "
		clickHandler.onClicked: onActivate3321()
	}
	function onActivate3411() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14821
		setCurrentScreen("Screen_7.qml")
	}
	StyledText {
		x: 959
		y: 900
		height: 240
		width: 180
		textLabel: "More"
	}
	ButtonSq {
		id: button3411_7
		x: 959
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Other/squareb.png"
		buttontext: "More"
		clickHandler.onClicked: onActivate3411()
	}
	function onActivate1263() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 15162
		setCurrentScreen("Screen_2.qml")
	}
	StyledText {
		x: 239
		y: 0
		height: 240
		width: 180
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button3412_8
		x: 239
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Lights/squareb.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate3412()
	}
	ButtonSq {
		id: button3390_9
		x: 526
		y: 14
		width: 144
		height: 108
		buttonbackground: "skins/Basic/../../scenarios/<%=5:0%>.png|<%=IM%>"
		buttontext: ""
		clickHandler.onClicked: onActivate3390()
	}
	function onActivate1264() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 15163
		setCurrentScreen("Screen_3.qml")
	}
	StyledText {
		x: 239
		y: 180
		height: 240
		width: 180
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button3413_10
		x: 239
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate3413()
	}
	ButtonSq {
		id: button3390_11
		x: 526
		y: 374
		width: 144
		height: 108
		buttonbackground: "skins/Basic/../../scenarios/<%=5:0%>.png|<%=IM%>"
		buttontext: ""
		clickHandler.onClicked: onActivate3390()
	}
	StyledText {
		x: 239
		y: 360
		height: 240
		width: 180
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button3414_12
		x: 239
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Climate/squareb.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate3414()
	}
	ButtonSq {
		id: button3390_13
		x: 526
		y: 734
		width: 144
		height: 108
		buttonbackground: "skins/Basic/../../scenarios/<%=5:0%>.png|<%=IM%>"
		buttontext: ""
		clickHandler.onClicked: onActivate3390()
	}
	StyledText {
		x: 239
		y: 540
		height: 240
		width: 180
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button3415_14
		x: 239
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Comm/squareb.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate3415()
	}
	ButtonSq {
		id: button3390_15
		x: 526
		y: 1094
		width: 144
		height: 108
		buttonbackground: "skins/Basic/../../scenarios/<%=5:0%>.png|<%=IM%>"
		buttontext: ""
		clickHandler.onClicked: onActivate3390()
	}
	function onActivate2431() 
	{
	 // Calling Command 408 (Show Floorplan) with CommandGroup_Parameters from 15022
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 408 11 \"0\" 12 \"0\" 14 \"light\" ")
	}
	ButtonSq {
		id: button2431_16
		x: 0
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Lights/mainmenu.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2431()
	}
	function onActivate2432() 
	{
	 // Calling Command 408 (Show Floorplan) with CommandGroup_Parameters from 15024
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 408 14 \"media\" ")
	}
	ButtonSq {
		id: button2432_17
		x: 0
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/mainmenu.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2432()
	}
	function onActivate2433() 
	{
	 // Calling Command 408 (Show Floorplan) with CommandGroup_Parameters from 15026
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 408 14 \"climate\" ")
	}
	ButtonSq {
		id: button2433_18
		x: 0
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Climate/mainmenu.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2433()
	}
	function onActivate2434() 
	{
	 // Calling Command 408 (Show Floorplan) with CommandGroup_Parameters from 15028
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 408 14 \"telecom\" ")
	}
	ButtonSq {
		id: button2434_19
		x: 0
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Comm/mainmenu.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2434()
	}
	function onActivate2435() 
	{
	 // Calling Command 408 (Show Floorplan) with CommandGroup_Parameters from 15030
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 408 14 \"security\" ")
	}
	ButtonSq {
		id: button2435_20
		x: 0
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Security/mainmenu.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2435()
	}
	StyledText {
		x: 239
		y: 720
		height: 240
		width: 180
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button3416_21
		x: 239
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Security/squareb.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate3416()
	}
	ButtonSq {
		id: button3390_22
		x: 505
		y: 1460
		width: 180
		height: 135
		buttonbackground: "skins/Basic/../../scenarios/<%=5:0%>.png|<%=IM%>"
		buttontext: ""
		clickHandler.onClicked: onActivate3390()
	}
	function onActivate3286() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14148
		setCurrentScreen("Screen_44.qml")
	}
	ButtonSq {
		id: button3286_23
		x: 0
		y: 901
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/transparent.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3286()
	}
	function onActivate3410() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14822
		setCurrentScreen("Screen_5.qml")
	}
	function onActivate3409() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14820
		setCurrentScreen("Screen_6.qml")
	}
	function onActivate3408() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14819
		setCurrentScreen("Screen_4.qml")
	}
	function onActivate3407() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14818
		setCurrentScreen("Screen_3.qml")
	}
	function onActivate3406() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14817
		setCurrentScreen("Screen_2.qml")
	}
}