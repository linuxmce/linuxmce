// UI1 Screen DVDFullScreenOpt (139)
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
		id: button1785_1535
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate1569() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13739
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18035
		setCurrentScreen("Screen_1.qml")
	}
	ButtonSq {
		id: button1569_1536
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
	function onActivate2338() 
	{
	 // Calling Command 65 (Jump Position In Playlist) with CommandGroup_Parameters from 13915
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 65 5 \"<%=35%>\" ")
	}
	function onActivate2457() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 12830
		setCurrentScreen("Screen_52.qml")
	}
	StyledText {
		x: 1440
		y: 900
		height: 240
		width: 180
		textLabel: "adv.
remote"
	}
	ButtonSq {
		id: button2457_1537
		x: 1440
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/br.png"
		buttontext: "adv.
remote"
		clickHandler.onClicked: onActivate2457()
	}
	function onActivate2598() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 12831
		setCurrentScreen("Screen_58.qml")
	}
	StyledText {
		x: 960
		y: 900
		height: 240
		width: 180
		textLabel: "DVD Options"
	}
	ButtonSq {
		id: button2598_1538
		x: 960
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "DVD Options"
		clickHandler.onClicked: onActivate2598()
	}
	function onActivate2289() 
	{
	 // Calling Command 97 (Mute) with CommandGroup_Parameters from 13657
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 97 ")
	}
	StyledText {
		x: 720
		y: 720
		height: 240
		width: 180
		textLabel: "Mute"
	}
	ButtonSq {
		id: button2289_1539
		x: 720
		y: 720
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
		id: button1850_1540
		x: 720
		y: 540
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
		id: button1851_1541
		x: 720
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1851()
	}
	function onActivate1421() 
	{
	 // Calling Command 87 (Goto Media Menu) with CommandGroup_Parameters from 14289
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 87 64 \"0\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17235
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 960
		y: 540
		height: 240
		width: 180
		textLabel: "Menu"
	}
	ButtonSq {
		id: button1421_1542
		x: 960
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "Menu"
		clickHandler.onClicked: onActivate1421()
	}
	function onActivate3425() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14837
		setCurrentScreen("Screen_81.qml")
	}
	StyledText {
		x: 1440
		y: 540
		height: 240
		width: 180
		textLabel: "attrib."
	}
	ButtonSq {
		id: button3425_1543
		x: 1440
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tr.png"
		buttontext: "attrib."
		clickHandler.onClicked: onActivate3425()
	}
	function onActivate2441() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14732
		manager.sendDceMessage(srouterip + " "+deviceid + " -104 1 184 76 \"+10\" ")
	}
	ButtonSq {
		id: button2441_1544
		x: 480
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2441()
	}
	function onActivate2443() 
	{
	 // Calling Command 408 (Show Floorplan) with CommandGroup_Parameters from 15042
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 408 11 \"100\" 12 \"100\" 14 \"light\" ")
	}
	StyledText {
		x: 480
		y: 720
		height: 5
		width: 3
		textLabel: "Lights"
	}
	ButtonSq {
		id: button2443_1545
		x: 480
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_map.png"
		buttontext: "Lights"
		clickHandler.onClicked: onActivate2443()
	}
	function onActivate2442() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14733
		manager.sendDceMessage(srouterip + " "+deviceid + " -104 1 184 76 \"-10\" ")
	}
	ButtonSq {
		id: button2442_1546
		x: 480
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2442()
	}
	function onActivate1940() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 15069
		setCurrentScreen("Screen_19.qml")
	}
	StyledText {
		x: 0
		y: 900
		height: 240
		width: 180
		textLabel: "Copy Disc"
	}
	ButtonSq {
		id: button1940_1547
		x: 0
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_1.png"
		buttontext: "Copy Disc"
		clickHandler.onClicked: onActivate1940()
	}
	function onActivate3558() 
	{
	}
	function onActivate3547() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 15050
		setCurrentScreen("Screen_87.qml")
	}
	StyledText {
		x: 240
		y: 900
		height: 240
		width: 180
		textLabel: "Book
marks"
	}
	ButtonSq {
		id: button3547_1548
		x: 240
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Book
marks"
		clickHandler.onClicked: onActivate3547()
	}
	function onActivate2456() 
	{
	}
	function onActivate1572() 
	{
	 // Calling Command 41 (Change Playback Speed) with CommandGroup_Parameters from 14165
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 41 43 \"-2\" ")
	}
	ButtonSq {
		id: button1572_1549
		x: 960
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_left.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1572()
	}
	function onActivate1575() 
	{
	 // Calling Command 65 (Jump Position In Playlist) with CommandGroup_Parameters from 13767
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 65 5 \"-1\" ")
	}
	ButtonSq {
		id: button1575_1550
		x: 1200
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1575()
	}
	function onActivate1576() 
	{
	 // Calling Command 65 (Jump Position In Playlist) with CommandGroup_Parameters from 13768
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 65 5 \"+1\" ")
	}
	ButtonSq {
		id: button1576_1551
		x: 1200
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1576()
	}
	function onActivate1581() 
	{
	 // Calling Command 41 (Change Playback Speed) with CommandGroup_Parameters from 14164
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 41 43 \"+2\" ")
	}
	ButtonSq {
		id: button1581_1552
		x: 1440
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_right.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1581()
	}
	function onActivate1582() 
	{
	 // Calling Command 39 (Pause Media) with CommandGroup_Parameters from 14163
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 39 ")
	}
	ButtonSq {
		id: button1582_1553
		x: 1195
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/enter.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1582()
	}
	function onActivate2028() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 9488
		setCurrentScreen("Screen_52.qml")
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "Resend
A/V
codes"
	}
	ButtonSq {
		id: button2028_1554
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Resend
A/V
codes"
		clickHandler.onClicked: onActivate2028()
	}
	function onActivate2458() 
	{
	 // Calling Command 401 (Show File List) with CommandGroup_Parameters from 15962
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 401 29 \"<%=MT%>\" ")
	}
	StyledText {
		x: 1680
		y: 180
		height: 240
		width: 180
		textLabel: "file
list"
	}
	ButtonSq {
		id: button2458_1555
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "file
list"
		clickHandler.onClicked: onActivate2458()
	}
	StyledText {
		x: 9
		y: 547
		height: 208
		width: 346
		textLabel: "<%=NP%>"
	}
	StyledText {
		x: 0
		y: 709
		height: 164
		width: 360
		textLabel: "<%=NP_SEC%>"
	}
	StyledText {
		x: 0
		y: 832
		height: 90
		width: 271
		textLabel: "<%=NP_TIME_SHORT%>"
	}
	StyledText {
		x: 361
		y: 832
		height: 90
		width: 89
		textLabel: "<%=NP_SPEED%>"
	}
	ButtonSq {
		id: button2455_1556
		x: 0
		y: 540
		width: 480
		height: 360
		buttonbackground: "skins/Basic/532"
		buttontext: "<%=NP_SPEED%>"
		clickHandler.onClicked: onActivate2455()
	}
	function onActivate5396() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17707
		setCurrentScreen("Screen_257.qml")
	}
	StyledText {
		x: 1680
		y: 360
		height: 240
		width: 180
		textLabel: "Thumbnail"
	}
	ButtonSq {
		id: button5396_1557
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "Thumbnail"
		clickHandler.onClicked: onActivate5396()
	}
	function onActivate5469() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17400
		setCurrentScreen("Screen_265.qml")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "Zoom & Aspect"
	}
	ButtonSq {
		id: button5469_1558
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Zoom & Aspect"
		clickHandler.onClicked: onActivate5469()
	}
}