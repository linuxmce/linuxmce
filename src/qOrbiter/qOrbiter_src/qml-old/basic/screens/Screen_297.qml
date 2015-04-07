// UI1 Screen Game Atari 2600 Controller Options (297)
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
		id: button1785_3104
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
		id: button1784_3105
		x: 1678
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	ButtonSq {
		id: button5867_3106
		x: 0
		y: 180
		width: 960
		height: 180
		buttonbackground: "skins/Basic/1064"
		buttontext: ""
		clickHandler.onClicked: onActivate5867()
	}
	function onActivate5855() 
	{
	 // Calling Command 982 (Set Game Options) with CommandGroup_Parameters from 17967
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 982 5 \"c1,joystick\" ")
	}
	StyledText {
		x: 0
		y: 180
		height: 240
		width: 180
		textLabel: "Joystick"
	}
	ButtonSq {
		id: button5855_3107
		x: 0
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Joystick"
		clickHandler.onClicked: onActivate5855()
	}
	function onActivate5856() 
	{
	 // Calling Command 982 (Set Game Options) with CommandGroup_Parameters from 17969
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 982 5 \"c1,paddle\" ")
	}
	StyledText {
		x: 240
		y: 180
		height: 240
		width: 180
		textLabel: "Paddle"
	}
	ButtonSq {
		id: button5856_3108
		x: 240
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Paddle"
		clickHandler.onClicked: onActivate5856()
	}
	function onActivate5857() 
	{
	 // Calling Command 982 (Set Game Options) with CommandGroup_Parameters from 17971
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 982 5 \"c1,driving\" ")
	}
	StyledText {
		x: 480
		y: 180
		height: 240
		width: 180
		textLabel: "Driving"
	}
	ButtonSq {
		id: button5857_3109
		x: 480
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Driving"
		clickHandler.onClicked: onActivate5857()
	}
	function onActivate5858() 
	{
	 // Calling Command 982 (Set Game Options) with CommandGroup_Parameters from 17973
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 982 5 \"c1,keypad\" ")
	}
	StyledText {
		x: 720
		y: 180
		height: 240
		width: 180
		textLabel: "Keypad"
	}
	ButtonSq {
		id: button5858_3110
		x: 720
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Keypad"
		clickHandler.onClicked: onActivate5858()
	}
	ButtonSq {
		id: button5868_3111
		x: 0
		y: 540
		width: 960
		height: 180
		buttonbackground: "skins/Basic/1064"
		buttontext: ""
		clickHandler.onClicked: onActivate5868()
	}
	function onActivate5859() 
	{
	 // Calling Command 982 (Set Game Options) with CommandGroup_Parameters from 17975
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 982 5 \"c2,joystick\" ")
	}
	StyledText {
		x: 0
		y: 540
		height: 240
		width: 180
		textLabel: "Joystick"
	}
	ButtonSq {
		id: button5859_3112
		x: 0
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Joystick"
		clickHandler.onClicked: onActivate5859()
	}
	function onActivate5860() 
	{
	 // Calling Command 982 (Set Game Options) with CommandGroup_Parameters from 17977
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 982 5 \"c2,paddle\" ")
	}
	StyledText {
		x: 240
		y: 540
		height: 240
		width: 180
		textLabel: "Paddle"
	}
	ButtonSq {
		id: button5860_3113
		x: 240
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Paddle"
		clickHandler.onClicked: onActivate5860()
	}
	function onActivate5861() 
	{
	 // Calling Command 982 (Set Game Options) with CommandGroup_Parameters from 17979
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 982 5 \"c2,driving\" ")
	}
	StyledText {
		x: 480
		y: 540
		height: 240
		width: 180
		textLabel: "Driving"
	}
	ButtonSq {
		id: button5861_3114
		x: 480
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Driving"
		clickHandler.onClicked: onActivate5861()
	}
	function onActivate5862() 
	{
	 // Calling Command 982 (Set Game Options) with CommandGroup_Parameters from 17981
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 982 5 \"c2,keypad\" ")
	}
	StyledText {
		x: 720
		y: 540
		height: 240
		width: 180
		textLabel: "Keypad"
	}
	ButtonSq {
		id: button5862_3115
		x: 720
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Keypad"
		clickHandler.onClicked: onActivate5862()
	}
	ButtonSq {
		id: button5869_3116
		x: 1080
		y: 180
		width: 480
		height: 180
		buttonbackground: "skins/Basic/532"
		buttontext: ""
		clickHandler.onClicked: onActivate5869()
	}
	function onActivate5863() 
	{
	 // Calling Command 982 (Set Game Options) with CommandGroup_Parameters from 17983
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 982 5 \"c1,difficultya\" ")
	}
	StyledText {
		x: 1080
		y: 180
		height: 240
		width: 180
		textLabel: "Difficulty A"
	}
	ButtonSq {
		id: button5863_3117
		x: 1080
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Difficulty A"
		clickHandler.onClicked: onActivate5863()
	}
	function onActivate5864() 
	{
	 // Calling Command 982 (Set Game Options) with CommandGroup_Parameters from 17985
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 982 5 \"c1,difficultyb\" ")
	}
	StyledText {
		x: 1320
		y: 180
		height: 240
		width: 180
		textLabel: "Difficulty B"
	}
	ButtonSq {
		id: button5864_3118
		x: 1320
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Difficulty B"
		clickHandler.onClicked: onActivate5864()
	}
	ButtonSq {
		id: button5870_3119
		x: 1080
		y: 540
		width: 480
		height: 180
		buttonbackground: "skins/Basic/532"
		buttontext: ""
		clickHandler.onClicked: onActivate5870()
	}
	function onActivate5865() 
	{
	 // Calling Command 982 (Set Game Options) with CommandGroup_Parameters from 17987
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 982 5 \"c2,difficultya\" ")
	}
	StyledText {
		x: 1080
		y: 540
		height: 240
		width: 180
		textLabel: "Difficulty A"
	}
	ButtonSq {
		id: button5865_3120
		x: 1080
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Difficulty A"
		clickHandler.onClicked: onActivate5865()
	}
	function onActivate5866() 
	{
	 // Calling Command 982 (Set Game Options) with CommandGroup_Parameters from 17989
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 982 5 \"c2,difficultyb\" ")
	}
	StyledText {
		x: 1320
		y: 540
		height: 240
		width: 180
		textLabel: "Difficulty B"
	}
	ButtonSq {
		id: button5866_3121
		x: 1320
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Difficulty B"
		clickHandler.onClicked: onActivate5866()
	}
}