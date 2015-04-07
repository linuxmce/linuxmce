// UI1 Screen Rip Options (285)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate5628() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17669
		setCurrentScreen("Screen_25.qml")
	}
	StyledText {
		x: 54
		y: 135
		height: 180
		width: 203
		textLabel: "Tracks"
	}
	ButtonSq {
		id: button5628_2994
		x: 54
		y: 135
		width: 1805
		height: 135
		buttonbackground: "skins/Basic/2000"
		buttontext: "Tracks"
		clickHandler.onClicked: onActivate5628()
	}
	StyledText {
		x: 1453
		y: 176
		height: 72
		width: 275
		textLabel: "Change"
	}
	ButtonSq {
		id: button5634_2995
		x: 1453
		y: 176
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/main%20menu%20acts.png"
		buttontext: "Change"
		clickHandler.onClicked: onActivate5634()
	}
	StyledText {
		x: 54
		y: 271
		height: 180
		width: 203
		textLabel: "Use drive"
	}
	ButtonSq {
		id: button5629_2996
		x: 54
		y: 271
		width: 1805
		height: 135
		buttonbackground: "skins/Basic/2000"
		buttontext: "Use drive"
		clickHandler.onClicked: onActivate5629()
	}
	function onActivate5635() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17670
		setCurrentScreen("Screen_241.qml")
	}
	StyledText {
		x: 1453
		y: 311
		height: 72
		width: 275
		textLabel: "Change"
	}
	ButtonSq {
		id: button5635_2997
		x: 1453
		y: 311
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/main%20menu%20acts.png"
		buttontext: "Change"
		clickHandler.onClicked: onActivate5635()
	}
	StyledText {
		x: 54
		y: 406
		height: 180
		width: 203
		textLabel: "In folder"
	}
	ButtonSq {
		id: button5630_2998
		x: 54
		y: 406
		width: 1805
		height: 135
		buttonbackground: "skins/Basic/2000"
		buttontext: "In folder"
		clickHandler.onClicked: onActivate5630()
	}
	function onActivate5636() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17671
		setCurrentScreen("Screen_240.qml")
	}
	StyledText {
		x: 1453
		y: 447
		height: 72
		width: 275
		textLabel: "Change"
	}
	ButtonSq {
		id: button5636_2999
		x: 1453
		y: 447
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/main%20menu%20acts.png"
		buttontext: "Change"
		clickHandler.onClicked: onActivate5636()
	}
	StyledText {
		x: 54
		y: 541
		height: 180
		width: 203
		textLabel: "Filename"
	}
	ButtonSq {
		id: button5631_3000
		x: 54
		y: 541
		width: 1805
		height: 135
		buttonbackground: "skins/Basic/2000"
		buttontext: "Filename"
		clickHandler.onClicked: onActivate5631()
	}
	function onActivate5637() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17672
		setCurrentScreen("Screen_97.qml")
	}
	StyledText {
		x: 1453
		y: 582
		height: 72
		width: 275
		textLabel: "Change"
	}
	ButtonSq {
		id: button5637_3001
		x: 1453
		y: 582
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/main%20menu%20acts.png"
		buttontext: "Change"
		clickHandler.onClicked: onActivate5637()
	}
	StyledText {
		x: 54
		y: 677
		height: 180
		width: 203
		textLabel: "Format"
	}
	ButtonSq {
		id: button5632_3002
		x: 54
		y: 677
		width: 1805
		height: 135
		buttonbackground: "skins/Basic/2000"
		buttontext: "Format"
		clickHandler.onClicked: onActivate5632()
	}
	function onActivate5638() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17673
		setCurrentScreen("Screen_286.qml")
	}
	StyledText {
		x: 1453
		y: 717
		height: 72
		width: 275
		textLabel: "Change"
	}
	ButtonSq {
		id: button5638_3003
		x: 1453
		y: 717
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/main%20menu%20acts.png"
		buttontext: "Change"
		clickHandler.onClicked: onActivate5638()
	}
	StyledText {
		x: 54
		y: 812
		height: 180
		width: 203
		textLabel: "Privacy"
	}
	ButtonSq {
		id: button5633_3004
		x: 54
		y: 812
		width: 1805
		height: 135
		buttonbackground: "skins/Basic/2000"
		buttontext: "Privacy"
		clickHandler.onClicked: onActivate5633()
	}
	function onActivate5639() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17674
		setCurrentScreen("Screen_287.qml")
	}
	StyledText {
		x: 1453
		y: 853
		height: 72
		width: 275
		textLabel: "Change"
	}
	ButtonSq {
		id: button5639_3005
		x: 1453
		y: 853
		width: 366
		height: 54
		buttonbackground: "skins/Basic/menu2/main%20menu%20acts.png"
		buttontext: "Change"
		clickHandler.onClicked: onActivate5639()
	}
	StyledText {
		x: 105
		y: 924
		height: 211
		width: 346
		textLabel: "Start rip"
	}
	ButtonSq {
		id: button5649_3006
		x: 105
		y: 924
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Start rip"
		clickHandler.onClicked: onActivate5649()
	}
}