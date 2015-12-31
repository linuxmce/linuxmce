// UI1 Screen PVR Recordings Full Screen Options (272)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
	}
	function onActivate3558() 
	{
	}
	function onActivate1785() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17174
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 1680
		y: 900
		height: 1
		width: 1
		textLabel: "Main menu"
	}
	ButtonSq {
		id: button1785_2807
		x: 1680
		y: 900
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
		id: button1569_2808
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
	function onActivate1574() 
	{
	 // Calling Command 42 (Jump to Position in Stream) with CommandGroup_Parameters from 16682
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 42 5 \"-10\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17311
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 961
		y: 541
		height: 240
		width: 180
		textLabel: "back"
	}
	ButtonSq {
		id: button1574_2809
		x: 961
		y: 541
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "back"
		clickHandler.onClicked: onActivate1574()
	}
	function onActivate2445() 
	{
	 // Calling Command 126 (Guide) with CommandGroup_Parameters from 16669
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 126 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17308
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 1678
		y: 541
		height: 240
		width: 180
		textLabel: "guide"
	}
	ButtonSq {
		id: button2445_2810
		x: 1678
		y: 541
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "guide"
		clickHandler.onClicked: onActivate2445()
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
		id: button2289_2811
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
		id: button1850_2812
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
		id: button1851_2813
		x: 720
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1851()
	}
	function onActivate1566() 
	{
	 // Calling Command 102 (Record) with CommandGroup_Parameters from 13636
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 102 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18381
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	ButtonSq {
		id: button1566_2814
		x: 240
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/record.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1566()
	}
	function onActivate2441() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14732
		manager.sendDceMessage(srouterip + " "+deviceid + " -104 1 184 76 \"+10\" ")
	}
	ButtonSq {
		id: button2441_2815
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
		id: button2443_2816
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
		id: button2442_2817
		x: 480
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2442()
	}
	function onActivate4770() 
	{
	 // Calling Command 123 (Info) with CommandGroup_Parameters from 16671
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 123 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17312
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 960
		y: 900
		height: 240
		width: 180
		textLabel: "info"
	}
	ButtonSq {
		id: button4770_2818
		x: 960
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "info"
		clickHandler.onClicked: onActivate4770()
	}
	function onActivate1572() 
	{
	 // Calling Command 41 (Change Playback Speed) with CommandGroup_Parameters from 14165
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 41 43 \"-2\" ")
	}
	ButtonSq {
		id: button1572_2819
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
		id: button1575_2820
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
		id: button1576_2821
		x: 1200
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1576()
	}
	function onActivate1582() 
	{
	 // Calling Command 39 (Pause Media) with CommandGroup_Parameters from 14163
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 39 ")
	}
	ButtonSq {
		id: button1582_2822
		x: 1200
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/enter.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1582()
	}
	function onActivate1581() 
	{
	 // Calling Command 41 (Change Playback Speed) with CommandGroup_Parameters from 14164
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 41 43 \"+2\" ")
	}
	ButtonSq {
		id: button1581_2823
		x: 1440
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_right.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1581()
	}
	function onActivate2028() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 9488
		setCurrentScreen("Screen_52.qml")
	}
	StyledText {
		x: 0
		y: 900
		height: 240
		width: 180
		textLabel: "Resend
A/V
codes"
	}
	ButtonSq {
		id: button2028_2824
		x: 0
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Resend
A/V
codes"
		clickHandler.onClicked: onActivate2028()
	}
	function onActivate4771() 
	{
	 // Calling Command 548 (Menu) with CommandGroup_Parameters from 16672
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 548 9 \"old_recordings\" ")
	}
	StyledText {
		x: 0
		y: 540
		height: 240
		width: 180
		textLabel: "Recordings"
	}
	ButtonSq {
		id: button4771_2825
		x: 0
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Recordings
"
		clickHandler.onClicked: onActivate4771()
	}
	function onActivate4772() 
	{
	 // Calling Command 548 (Menu) with CommandGroup_Parameters from 16673
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 548 9 \"future_recordings\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17309
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 240
		y: 540
		height: 240
		width: 180
		textLabel: "Schedule"
	}
	ButtonSq {
		id: button4772_2826
		x: 240
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Schedule
"
		clickHandler.onClicked: onActivate4772()
	}
	function onActivate4775() 
	{
	 // Calling Command 762 (Live TV) with CommandGroup_Parameters from 16683
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 762 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17310
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 130
		y: 717
		height: 240
		width: 180
		textLabel: "live"
	}
	ButtonSq {
		id: button4775_2827
		x: 130
		y: 717
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "live"
		clickHandler.onClicked: onActivate4775()
	}
	function onActivate2578() 
	{
	 // Calling Command 187 (Tune to channel) with CommandGroup_Parameters from 14701
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 187 68 \"<%=17%>\" ")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14715
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" ")
	}
	StyledText {
		x: 343
		y: 122
		height: 240
		width: 180
		textLabel: "enter"
	}
	ButtonSq {
		id: button2578_2828
		x: 343
		y: 122
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "enter"
		clickHandler.onClicked: onActivate2578()
	}
}