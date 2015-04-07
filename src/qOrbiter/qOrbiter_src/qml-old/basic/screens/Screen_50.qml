// UI1 Screen DVDMenu (50)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
	}
	function onActivate1662() 
	{
	 // Calling Command 41 (Change Playback Speed) with CommandGroup_Parameters from 14288
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 41 43 \"+1000\" ")
	 // Calling Command 190 (Enter/Go) with CommandGroup_Parameters from 14346
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 190 ")
	}
	StyledText {
		x: 1200
		y: 900
		height: 1
		width: 1
		textLabel: "Play"
	}
	ButtonSq {
		id: button1662_616
		x: 1200
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/play.png"
		buttontext: "Play"
		clickHandler.onClicked: onActivate1662()
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
		id: button1785_617
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate3351() 
	{
	 // Calling Command 29 (Simulate Mouse Click) with CommandGroup_Parameters from 14282
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 29 11 \"<%=XS%>\" 12 \"<%=YS%>\" ")
	}
	ButtonSq {
		id: button3351_618
		x: 23
		y: 14
		width: 1642
		height: 886
		buttonbackground: "skins/Basic/<%=NPD%>"
		buttontext: ""
		clickHandler.onClicked: onActivate3351()
	}
	function onActivate4296() 
	{
	 // Calling Command 200 (Move Up) with CommandGroup_Parameters from 16013
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 200 ")
	}
	ButtonSq {
		id: button4296_619
		x: 240
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4296()
	}
	function onActivate4293() 
	{
	 // Calling Command 201 (Move Down) with CommandGroup_Parameters from 16014
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 201 ")
	}
	ButtonSq {
		id: button4293_620
		x: 480
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4293()
	}
	function onActivate4294() 
	{
	 // Calling Command 202 (Move Left) with CommandGroup_Parameters from 16015
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 202 ")
	}
	ButtonSq {
		id: button4294_621
		x: 720
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/left.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4294()
	}
	function onActivate4295() 
	{
	 // Calling Command 203 (Move Right) with CommandGroup_Parameters from 16012
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 203 ")
	}
	ButtonSq {
		id: button4295_622
		x: 960
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/right.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4295()
	}
	function onActivate2289() 
	{
	 // Calling Command 97 (Mute) with CommandGroup_Parameters from 13657
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 97 ")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "Mute"
	}
	ButtonSq {
		id: button2289_623
		x: 1680
		y: 540
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
		id: button1850_624
		x: 1680
		y: 360
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
		id: button1851_625
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1851()
	}
	function onActivate1940() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 15069
		setCurrentScreen("Screen_19.qml")
	}
	StyledText {
		x: 1680
		y: 180
		height: 240
		width: 180
		textLabel: "Copy Disc"
	}
	ButtonSq {
		id: button1940_626
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_1.png"
		buttontext: "Copy Disc"
		clickHandler.onClicked: onActivate1940()
	}
	function onActivate2598() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 12831
		setCurrentScreen("Screen_58.qml")
	}
	StyledText {
		x: 0
		y: 900
		height: 240
		width: 180
		textLabel: "DVD Options"
	}
	ButtonSq {
		id: button2598_627
		x: 0
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "DVD Options"
		clickHandler.onClicked: onActivate2598()
	}
	function onActivate3425() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14837
		setCurrentScreen("Screen_81.qml")
	}
	StyledText {
		x: 1440
		y: 900
		height: 240
		width: 180
		textLabel: "attrib."
	}
	ButtonSq {
		id: button3425_628
		x: 1440
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tr.png"
		buttontext: "attrib."
		clickHandler.onClicked: onActivate3425()
	}
	function onActivate1569() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13739
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18035
		setCurrentScreen("Screen_1.qml")
	}
	ButtonSq {
		id: button1569_629
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
	function onActivate3363() 
	{
	 // Calling Command 200 (Move Up) with CommandGroup_Parameters from 14411
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 200 ")
	}
	function onActivate3364() 
	{
	 // Calling Command 201 (Move Down) with CommandGroup_Parameters from 14412
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 201 ")
	}
	function onActivate3365() 
	{
	 // Calling Command 202 (Move Left) with CommandGroup_Parameters from 14413
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 202 ")
	}
	function onActivate3366() 
	{
	 // Calling Command 203 (Move Right) with CommandGroup_Parameters from 14414
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 203 ")
	}
	function onActivate3367() 
	{
	 // Calling Command 190 (Enter/Go) with CommandGroup_Parameters from 14415
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 190 ")
	}
}