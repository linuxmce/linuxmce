// UI1 Screen Non Pluto TV Full screen options (282)
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
		id: button1785_2926
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate2028() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 9488
		setCurrentScreen("Screen_52.qml")
	}
	StyledText {
		x: 1440
		y: 900
		height: 240
		width: 180
		textLabel: "Resend
A/V
codes"
	}
	ButtonSq {
		id: button2028_2927
		x: 1440
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Resend
A/V
codes"
		clickHandler.onClicked: onActivate2028()
	}
	function onActivate1569() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13739
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18035
		setCurrentScreen("Screen_1.qml")
	}
	ButtonSq {
		id: button1569_2928
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
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
		id: button1572_2929
		x: 1200
		y: 540
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
		id: button1575_2930
		x: 1440
		y: 360
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
		id: button1576_2931
		x: 1440
		y: 720
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
		id: button1581_2932
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
		id: button1582_2933
		x: 1435
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/enter.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1582()
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
		id: button2289_2934
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
		id: button1850_2935
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
		id: button1851_2936
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
		id: button2441_2937
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
		id: button2443_2938
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
		id: button2442_2939
		x: 720
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2442()
	}
	function onActivate2567() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14691
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"16\" 50 \"\" ")
	}
	ButtonSq {
		id: button2567_2940
		x: 0
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/1.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2567()
	}
	function onActivate2568() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14692
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"17\" ")
	}
	ButtonSq {
		id: button2568_2941
		x: 240
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/2.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2568()
	}
	function onActivate2569() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14693
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"18\" ")
	}
	ButtonSq {
		id: button2569_2942
		x: 480
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/3.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2569()
	}
	function onActivate2570() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14694
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"19\" ")
	}
	ButtonSq {
		id: button2570_2943
		x: 0
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/4.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2570()
	}
	function onActivate2571() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14695
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"20\" ")
	}
	ButtonSq {
		id: button2571_2944
		x: 240
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/5.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2571()
	}
	function onActivate2572() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14696
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"21\" ")
	}
	ButtonSq {
		id: button2572_2945
		x: 480
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/6.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2572()
	}
	function onActivate2573() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14697
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"22\" ")
	}
	ButtonSq {
		id: button2573_2946
		x: 0
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/7.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2573()
	}
	function onActivate2574() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14698
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"23\" ")
	}
	ButtonSq {
		id: button2574_2947
		x: 240
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/8.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2574()
	}
	function onActivate2575() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14699
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"24\" ")
	}
	ButtonSq {
		id: button2575_2948
		x: 480
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/9.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2575()
	}
	function onActivate2566() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14700
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"25\" ")
	}
	ButtonSq {
		id: button2566_2949
		x: 240
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Number%20Pad/0.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2566()
	}
	function onActivate2578() 
	{
	 // Calling Command 187 (Tune to channel) with CommandGroup_Parameters from 14701
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 187 68 \"<%=17%>\" ")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14715
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" ")
	}
	StyledText {
		x: 480
		y: 900
		height: 240
		width: 180
		textLabel: "enter"
	}
	ButtonSq {
		id: button2578_2950
		x: 480
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "enter"
		clickHandler.onClicked: onActivate2578()
	}
	function onActivate2579() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 14702
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"112\" ")
	}
	ButtonSq {
		id: button2579_2951
		x: 0
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/clear.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2579()
	}
	function onActivate3558() 
	{
	}
	function onActivate2565() 
	{
	 // Calling Command 126 (Guide) with CommandGroup_Parameters from 13675
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 126 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14705
		setCurrentScreen("Screen_80.qml")
	}
	StyledText {
		x: 1680
		y: 180
		height: 240
		width: 180
		textLabel: "guide"
	}
	ButtonSq {
		id: button2565_2952
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "guide"
		clickHandler.onClicked: onActivate2565()
	}
	function onActivate2561() 
	{
	 // Calling Command 86 (Menu (Show Menu)) with CommandGroup_Parameters from 14570
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 86 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14706
		setCurrentScreen("Screen_80.qml")
	}
	StyledText {
		x: 120
		y: 0
		height: 240
		width: 180
		textLabel: "Menu"
	}
	ButtonSq {
		id: button2561_2953
		x: 120
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Menu"
		clickHandler.onClicked: onActivate2561()
	}
	function onActivate2562() 
	{
	 // Calling Command 240 (Back / Prior Menu) with CommandGroup_Parameters from 14571
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 240 ")
	}
	StyledText {
		x: 360
		y: 0
		height: 240
		width: 180
		textLabel: "exit"
	}
	ButtonSq {
		id: button2562_2954
		x: 360
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "exit"
		clickHandler.onClicked: onActivate2562()
	}
	function onActivate2563() 
	{
	 // Calling Command 123 (Info) with CommandGroup_Parameters from 13673
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 123 ")
	}
	StyledText {
		x: 120
		y: 180
		height: 240
		width: 180
		textLabel: "info"
	}
	ButtonSq {
		id: button2563_2955
		x: 120
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "info"
		clickHandler.onClicked: onActivate2563()
	}
	StyledText {
		x: 360
		y: 180
		height: 240
		width: 180
		textLabel: "music"
	}
	ButtonSq {
		id: button2564_2956
		x: 360
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "music"
		clickHandler.onClicked: onActivate2564()
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
		id: button1574_2957
		x: 1200
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "back"
		clickHandler.onClicked: onActivate1574()
	}
	function onActivate2580() 
	{
	 // Calling Command 124 (Favorites) with CommandGroup_Parameters from 13674
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 124 ")
	}
	StyledText {
		x: 1680
		y: 360
		height: 240
		width: 180
		textLabel: "fav."
	}
	ButtonSq {
		id: button2580_2958
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tr.png"
		buttontext: "fav."
		clickHandler.onClicked: onActivate2580()
	}
	function onActivate1577() 
	{
	}
	StyledText {
		x: 1200
		y: 720
		height: 240
		width: 180
		textLabel: "prev."
	}
	ButtonSq {
		id: button1577_2959
		x: 1200
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "prev."
		clickHandler.onClicked: onActivate1577()
	}
	function onActivate1579() 
	{
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "live"
	}
	ButtonSq {
		id: button1579_2960
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/br.png"
		buttontext: "live"
		clickHandler.onClicked: onActivate1579()
	}
}