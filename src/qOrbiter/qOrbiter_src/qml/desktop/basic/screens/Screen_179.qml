// UI1 Screen DialogCheckingDrive (179)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
	}
	function onActivate2038() 
	{
	 // Calling Command 272 (Restart DCERouter) with CommandGroup_Parameters from 14233
		manager.sendDceMessage(srouterip + " "+deviceid + " -103 1 272 ")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "Quick reload router"
	}
	ButtonSq {
		id: button2038_2038
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Quick reload router"
		clickHandler.onClicked: onActivate2038()
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
		id: button1785_2039
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
		id: button1784_2040
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate3419() 
	{
	 // Calling Command 389 (Send Message) with CommandGroup_Parameters from 14828
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 389 9 \"<%=46%>\" 144 \"1\" ")
	}
	StyledText {
		x: 252
		y: 677
		height: 211
		width: 346
		textLabel: "<%=0:1%>"
	}
	ButtonSq {
		id: button3419_2041
		x: 252
		y: 677
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "<%=0:1%>"
		clickHandler.onClicked: onActivate3419()
	}
	function onActivate3420() 
	{
	 // Calling Command 389 (Send Message) with CommandGroup_Parameters from 14829
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 389 9 \"<%=47%>\" 144 \"1\" ")
	}
	StyledText {
		x: 965
		y: 677
		height: 211
		width: 346
		textLabel: "<%=0:1%>"
	}
	ButtonSq {
		id: button3420_2042
		x: 965
		y: 677
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "<%=0:1%>"
		clickHandler.onClicked: onActivate3420()
	}
	function onActivate3421() 
	{
	 // Calling Command 389 (Send Message) with CommandGroup_Parameters from 14830
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 389 9 \"<%=48%>\" 144 \"1\" ")
	}
	StyledText {
		x: 252
		y: 900
		height: 211
		width: 346
		textLabel: "<%=0:1%>"
	}
	ButtonSq {
		id: button3421_2043
		x: 252
		y: 900
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "<%=0:1%>"
		clickHandler.onClicked: onActivate3421()
	}
	function onActivate3422() 
	{
	 // Calling Command 389 (Send Message) with CommandGroup_Parameters from 14831
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 389 9 \"<%=49%>\" 144 \"1\" ")
	}
	StyledText {
		x: 965
		y: 900
		height: 211
		width: 346
		textLabel: "<%=0:1%>"
	}
	ButtonSq {
		id: button3422_2044
		x: 965
		y: 900
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "<%=0:1%>"
		clickHandler.onClicked: onActivate3422()
	}
	function onActivate5133() 
	{
	 // Calling Command 397 (Show Popup) with CommandGroup_Parameters from 17100
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 397 3 \"5132\" ")
	}
}