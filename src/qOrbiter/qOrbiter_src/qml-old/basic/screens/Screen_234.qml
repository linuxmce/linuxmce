// UI1 Screen NAS Options when Mounting device (234)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate4726() 
	{
	 // Calling Command 804 (Set Pnp Options) with CommandGroup_Parameters from 17005
		manager.sendDceMessage(srouterip + " "+deviceid + " -111 1 804 5 \"-1\" 52 \"3\" 224 \"<%=ID%>\" ")
	 // Calling Command 8 (Remove Screen From History) with CommandGroup_Parameters from 17168
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 8 10 \"<%=ID%>\" 159 \"234\" ")
	}
	StyledText {
		x: 9
		y: 474
		height: 213
		width: 698
		textLabel: "Yes, use LinuxMCE's directory structure"
	}
	ButtonSq {
		id: button4726_2474
		x: 9
		y: 474
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Yes, use LinuxMCE's directory structure"
		clickHandler.onClicked: onActivate4726()
	}
	function onActivate4721() 
	{
	 // Calling Command 804 (Set Pnp Options) with CommandGroup_Parameters from 17004
		manager.sendDceMessage(srouterip + " "+deviceid + " -111 1 804 5 \"0\" 52 \"3\" 224 \"<%=ID%>\" ")
	 // Calling Command 8 (Remove Screen From History) with CommandGroup_Parameters from 17167
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 8 10 \"<%=ID%>\" 159 \"234\" ")
	}
	StyledText {
		x: 9
		y: 677
		height: 213
		width: 698
		textLabel: "No, just make all the content public"
	}
	ButtonSq {
		id: button4721_2475
		x: 9
		y: 677
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "No, just make all the content public"
		clickHandler.onClicked: onActivate4721()
	}
	function onActivate5372() 
	{
	 // Calling Command 804 (Set Pnp Options) with CommandGroup_Parameters from 17349
		manager.sendDceMessage(srouterip + " "+deviceid + " -111 1 804 5 \"<%=2%>\" 52 \"3\" 224 \"<%=ID%>\" ")
	 // Calling Command 8 (Remove Screen From History) with CommandGroup_Parameters from 17350
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 8 10 \"<%=ID%>\" 159 \"234\" ")
	}
	ButtonSq {
		id: button5372_2476
		x: 1083
		y: 474
		width: 812
		height: 541
		buttonbackground: "skins/Basic/2"
		buttontext: ""
		clickHandler.onClicked: onActivate5372()
	}
}