// UI1 Screen NAS Options (202)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate4723() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17106
		setCurrentScreen("Screen_234.qml")
	 // Calling Command 804 (Set Pnp Options) with CommandGroup_Parameters from 17002
		manager.sendDceMessage(srouterip + " "+deviceid + " -111 1 804 5 \"1\" 52 \"134\" 224 \"<%=ID%>\" ")
	 // Calling Command 8 (Remove Screen From History) with CommandGroup_Parameters from 17170
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 8 10 \"<%=ID%>\" 159 \"202\" ")
	}
	StyledText {
		x: 496
		y: 474
		height: 213
		width: 698
		textLabel: "Yes"
	}
	ButtonSq {
		id: button4723_2339
		x: 496
		y: 474
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Yes"
		clickHandler.onClicked: onActivate4723()
	}
	function onActivate4724() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17107
		setCurrentScreen("Screen_234.qml")
	 // Calling Command 804 (Set Pnp Options) with CommandGroup_Parameters from 17003
		manager.sendDceMessage(srouterip + " "+deviceid + " -111 1 804 5 \"0\" 52 \"134\" 224 \"<%=ID%>\" ")
	 // Calling Command 8 (Remove Screen From History) with CommandGroup_Parameters from 17169
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 8 10 \"<%=ID%>\" 159 \"202\" ")
	}
	StyledText {
		x: 496
		y: 677
		height: 213
		width: 698
		textLabel: "No, use it only when specifically told to"
	}
	ButtonSq {
		id: button4724_2340
		x: 496
		y: 677
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "No, use it only when specifically told to"
		clickHandler.onClicked: onActivate4724()
	}
}