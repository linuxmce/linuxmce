// UI1 Screen Halt System (249)
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
		id: button1785_2631
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate5369() 
	{
	 // Calling Command 3 (Display On/Off) with CommandGroup_Parameters from 17340
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 3 8 \"0\" ")
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 17343
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	StyledText {
		x: 180
		y: 474
		height: 211
		width: 346
		textLabel: "Display
Off"
	}
	ButtonSq {
		id: button5369_2632
		x: 180
		y: 474
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Display
Off"
		clickHandler.onClicked: onActivate5369()
	}
	function onActivate5370() 
	{
	 // Calling Command 323 (Halt Device) with CommandGroup_Parameters from 17341
		manager.sendDceMessage(srouterip + " "+deviceid + " -103 1 323 2 \"<%=MDH%>\" ")
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 17342
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	StyledText {
		x: 992
		y: 474
		height: 211
		width: 346
		textLabel: "Halt m/d"
	}
	ButtonSq {
		id: button5370_2633
		x: 992
		y: 474
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Halt m/d"
		clickHandler.onClicked: onActivate5370()
	}
	function onActivate5371() 
	{
	 // Calling Command 323 (Halt Device) with CommandGroup_Parameters from 17345
		manager.sendDceMessage(srouterip + " "+deviceid + " -103 1 323 2 \"<%=MDH%>\" 21 \"V\" ")
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 17346
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	StyledText {
		x: 992
		y: 812
		height: 211
		width: 346
		textLabel: "Restart
As <%=OS%>"
	}
	ButtonSq {
		id: button5371_2634
		x: 992
		y: 812
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Restart
As <%=OS%>"
		clickHandler.onClicked: onActivate5371()
	}
	function onActivate5274() 
	{
	 // Calling Command 323 (Halt Device) with CommandGroup_Parameters from 17265
		manager.sendDceMessage(srouterip + " "+deviceid + " -103 1 323 2 \"<%=MDH%>\" 21 \"R\" ")
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 17344
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	StyledText {
		x: 180
		y: 812
		height: 211
		width: 346
		textLabel: "Reset"
	}
	ButtonSq {
		id: button5274_2635
		x: 180
		y: 812
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Reset"
		clickHandler.onClicked: onActivate5274()
	}
}