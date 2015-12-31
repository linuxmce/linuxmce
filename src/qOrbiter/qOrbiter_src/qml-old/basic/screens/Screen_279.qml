// UI1 Screen VDR Recordings (279)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/PVR/PVRremote.png"
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
		id: button1785_2893
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
		id: button1569_2894
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
	function onActivate4775() 
	{
	 // Calling Command 762 (Live TV) with CommandGroup_Parameters from 16683
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 762 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17310
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 240
		y: 900
		height: 240
		width: 180
		textLabel: "live"
	}
	ButtonSq {
		id: button4775_2895
		x: 240
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "live"
		clickHandler.onClicked: onActivate4775()
	}
	function onActivate4771() 
	{
	 // Calling Command 548 (Menu) with CommandGroup_Parameters from 16672
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 548 9 \"old_recordings\" ")
	}
	StyledText {
		x: 720
		y: 900
		height: 240
		width: 180
		textLabel: "Recordings"
	}
	ButtonSq {
		id: button4771_2896
		x: 720
		y: 900
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
		x: 960
		y: 900
		height: 240
		width: 180
		textLabel: "Schedule"
	}
	ButtonSq {
		id: button4772_2897
		x: 960
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Schedule
"
		clickHandler.onClicked: onActivate4772()
	}
	function onActivate2289() 
	{
	 // Calling Command 97 (Mute) with CommandGroup_Parameters from 13657
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 97 ")
	}
	StyledText {
		x: 960
		y: 540
		height: 240
		width: 180
		textLabel: "Mute"
	}
	ButtonSq {
		id: button2289_2898
		x: 960
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
		id: button1850_2899
		x: 960
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
		id: button1851_2900
		x: 960
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1851()
	}
	function onActivate2441() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14732
		manager.sendDceMessage(srouterip + " "+deviceid + " -104 1 184 76 \"+10\" ")
	}
	ButtonSq {
		id: button2441_2901
		x: 720
		y: 360
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
		x: 720
		y: 540
		height: 5
		width: 3
		textLabel: "Lights"
	}
	ButtonSq {
		id: button2443_2902
		x: 720
		y: 540
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
		id: button2442_2903
		x: 720
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2442()
	}
	StyledText {
		x: 729
		y: 7
		height: 208
		width: 346
		textLabel: "<%=NP%>"
	}
	StyledText {
		x: 720
		y: 169
		height: 164
		width: 360
		textLabel: "<%=NP_SEC%>"
	}
	StyledText {
		x: 720
		y: 292
		height: 90
		width: 271
		textLabel: "<%=NP_TIME_SHORT%>"
	}
	StyledText {
		x: 1081
		y: 292
		height: 90
		width: 89
		textLabel: "<%=NP_SPEED%>"
	}
	ButtonSq {
		id: button2455_2904
		x: 720
		y: 0
		width: 480
		height: 360
		buttonbackground: "skins/Basic/532"
		buttontext: "<%=NP_SPEED%>"
		clickHandler.onClicked: onActivate2455()
	}
	function onActivate2445() 
	{
	 // Calling Command 126 (Guide) with CommandGroup_Parameters from 16669
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 126 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17308
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 1680
		y: 180
		height: 240
		width: 180
		textLabel: "guide"
	}
	ButtonSq {
		id: button2445_2905
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "guide"
		clickHandler.onClicked: onActivate2445()
	}
	function onActivate1572() 
	{
	 // Calling Command 41 (Change Playback Speed) with CommandGroup_Parameters from 14165
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 41 43 \"-2\" ")
	}
	ButtonSq {
		id: button1572_2906
		x: 1200
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_left.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1572()
	}
	function onActivate1581() 
	{
	 // Calling Command 41 (Change Playback Speed) with CommandGroup_Parameters from 14164
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 41 43 \"+2\" ")
	}
	ButtonSq {
		id: button1581_2907
		x: 1680
		y: 540
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
		id: button1582_2908
		x: 1440
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/enter.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1582()
	}
	function onActivate1574() 
	{
	 // Calling Command 42 (Jump to Position in Stream) with CommandGroup_Parameters from 16682
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 42 5 \"-10\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17311
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 1200
		y: 360
		height: 240
		width: 180
		textLabel: "back"
	}
	ButtonSq {
		id: button1574_2909
		x: 1200
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "back"
		clickHandler.onClicked: onActivate1574()
	}
	function onActivate4770() 
	{
	 // Calling Command 123 (Info) with CommandGroup_Parameters from 16671
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 123 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17312
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 1200
		y: 720
		height: 240
		width: 180
		textLabel: "info"
	}
	ButtonSq {
		id: button4770_2910
		x: 1200
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "info"
		clickHandler.onClicked: onActivate4770()
	}
	function onActivate2393() 
	{
	}
	ButtonSq {
		id: button2390_2911
		x: 90
		y: 0
		width: 480
		height: 292
		buttonbackground: "skins/Basic/<%=79:1%>"
		buttontext: ""
		clickHandler.onClicked: onActivate2390()
	}
	function onActivate2392() 
	{
	}
	ButtonSq {
		id: button2391_2912
		x: 1200
		y: 0
		width: 480
		height: 292
		buttonbackground: "skins/Basic/<%=81:1%>"
		buttontext: ""
		clickHandler.onClicked: onActivate2391()
	}
	function onActivate1566() 
	{
	 // Calling Command 102 (Record) with CommandGroup_Parameters from 13636
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 102 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18381
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	ButtonSq {
		id: button1566_2913
		x: 1440
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/record.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1566()
	}
	function onActivate2028() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 9488
		setCurrentScreen("Screen_52.qml")
	}
	StyledText {
		x: 1200
		y: 900
		height: 240
		width: 180
		textLabel: "Resend
A/V
codes"
	}
	ButtonSq {
		id: button2028_2914
		x: 1200
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Resend
A/V
codes"
		clickHandler.onClicked: onActivate2028()
	}
	function onActivate2399() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 11857
		setCurrentScreen("Screen_61.qml")
	}
	StyledText {
		x: 541
		y: 271
		height: 120
		width: 135
		textLabel: "More"
	}
	ButtonSq {
		id: button2399_2915
		x: 541
		y: 271
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "More"
		clickHandler.onClicked: onActivate2399()
	}
	function onActivate2400() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 17510
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"167\" 50 \"period\" ")
	}
	StyledText {
		x: 1534
		y: 271
		height: 120
		width: 90
		textLabel: "."
	}
	ButtonSq {
		id: button2400_2916
		x: 1534
		y: 271
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "."
		clickHandler.onClicked: onActivate2400()
	}
}