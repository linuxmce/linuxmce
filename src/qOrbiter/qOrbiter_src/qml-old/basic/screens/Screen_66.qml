// UI1 Screen MediaAttributeSearchResult (66)
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
		id: button1785_838
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
		id: button1784_839
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate2552() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16962
		setCurrentScreen("Screen_220.qml")
	}
	StyledText {
		x: 1680
		y: 360
		height: 240
		width: 180
		textLabel: "Remote"
	}
	ButtonSq {
		id: button2552_840
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/s1.png"
		buttontext: "Remote"
		clickHandler.onClicked: onActivate2552()
	}
	function onActivate2539() 
	{
	 // Calling Command 43 (MH Play Media) with CommandGroup_Parameters from 13876
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 43 13 \"<%=12%>\" 45 \"<%=E%>\" ")
	}
	StyledText {
		x: 1680
		y: 540
		height: 1
		width: 1
		textLabel: "Play"
	}
	ButtonSq {
		id: button2539_841
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/play.png"
		buttontext: "Play"
		clickHandler.onClicked: onActivate2539()
	}
	function onActivate2540() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 12693
		setCurrentScreen("Screen_66.qml")
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "View"
	}
	ButtonSq {
		id: button2540_842
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "View"
		clickHandler.onClicked: onActivate2540()
	}
	function onActivate1664() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 12512
		setCurrentScreen("Screen_1.qml")
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13956
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 ")
	}
	ButtonSq {
		id: button1664_843
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1664()
	}
	function onActivate2536() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13866
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"12\" 5 \"!F<%=2%>\" ")
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 13868
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"2540\" 8 \"1\" ")
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 13873
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"2539\" 8 \"1\" ")
	}
	function onActivate2541() 
	{
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 13870
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"2540\" 8 \"1\" ")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13872
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"12\" 5 \"<%=2%>\" ")
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 13889
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"2557\" ")
	}
	function onActivate2537() 
	{
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 13869
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"2540\" 8 \"1\" ")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13871
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"12\" 5 \"!A<%=2%>\" ")
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 13874
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"2539\" 8 \"1\" ")
	}
}