// UI1 Screen MakeCallPhonebook2 (22)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookbgB.png"
		width: 1920
		height: 1080
	}
	function onActivate1618() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14147
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"<%=2%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2330
		setCurrentScreen("Screen_21.qml")
	}
	ButtonSq {
		id: button1618_286
		x: 573
		y: 244
		width: 541
		height: 778
		buttonbackground: "skins/Basic/3"
		buttontext: ""
		clickHandler.onClicked: onActivate1618()
	}
	StyledText {
		x: 0
		y: 0
		height: 236
		width: 250
		textLabel: "Phone
Book"
	}
	ButtonSq {
		id: button1315_287
		x: 0
		y: 0
		width: 334
		height: 177
		buttonbackground: "skins/Basic/Buttons/Media/standard_on.png"
		buttontext: "Phone
Book"
		clickHandler.onClicked: onActivate1315()
	}
	function onActivate1322() 
	{
	 // Calling Command 5 (Goto DesignObj) with CommandGroup_Parameters from 2501
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 5 3 \"1290\" ")
	}
	StyledText {
		x: 1002
		y: 0
		height: 236
		width: 250
		textLabel: "Call Manager"
	}
	ButtonSq {
		id: button1322_288
		x: 1002
		y: 0
		width: 334
		height: 177
		buttonbackground: "skins/Basic/Buttons/Comm/square.png"
		buttontext: "Call Manager"
		clickHandler.onClicked: onActivate1322()
	}
	function onActivate1682() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2416
		setCurrentScreen("Screen_23.qml")
	}
	StyledText {
		x: 1335
		y: 0
		height: 236
		width: 250
		textLabel: "Call
Intercom"
	}
	ButtonSq {
		id: button1682_289
		x: 1335
		y: 0
		width: 334
		height: 177
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Call
Intercom"
		clickHandler.onClicked: onActivate1682()
	}
	function onActivate1785() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17174
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 1678
		y: 0
		height: 1
		width: 1
		textLabel: "Main menu"
	}
	ButtonSq {
		id: button1785_290
		x: 1678
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate1320() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2210
		setCurrentScreen("Screen_11.qml")
	}
	StyledText {
		x: 334
		y: 0
		height: 236
		width: 250
		textLabel: "Dial
Direct"
	}
	ButtonSq {
		id: button1320_291
		x: 334
		y: 0
		width: 334
		height: 177
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Dial
Direct"
		clickHandler.onClicked: onActivate1320()
	}
	function onActivate1321() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 2207
		setCurrentScreen("Screen_10.qml")
	}
	StyledText {
		x: 668
		y: 0
		height: 236
		width: 250
		textLabel: "Speed
Dial"
	}
	ButtonSq {
		id: button1321_292
		x: 668
		y: 0
		width: 334
		height: 177
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Speed
Dial"
		clickHandler.onClicked: onActivate1321()
	}
}