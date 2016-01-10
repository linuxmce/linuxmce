// UI1 Screen NonPluto BluRay FS Options (300)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
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
		id: button2455_3154
		x: 0
		y: 540
		width: 480
		height: 360
		buttonbackground: "skins/Basic/532"
		buttontext: "<%=NP_SPEED%>"
		clickHandler.onClicked: onActivate2455()
	}
	function onActivate2441() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14732
		manager.sendDceMessage(srouterip + " "+deviceid + " -104 1 184 76 \"+10\" ")
	}
	ButtonSq {
		id: button2441_3155
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
		id: button2443_3156
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
		id: button2442_3157
		x: 480
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2442()
	}
	function onActivate2289() 
	{
	 // Calling Command 97 (Mute) with CommandGroup_Parameters from 13657
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 97 ")
	}
	StyledText {
		x: 715
		y: 716
		height: 240
		width: 180
		textLabel: "Mute"
	}
	ButtonSq {
		id: button2289_3158
		x: 715
		y: 716
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
		id: button1850_3159
		x: 715
		y: 536
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
		id: button1851_3160
		x: 715
		y: 896
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1851()
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
		id: button1785_3161
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
		id: button1569_3162
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
		id: button1572_3163
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
		id: button1575_3164
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
		id: button1576_3165
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
		id: button1581_3166
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
		id: button1582_3167
		x: 1195
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/enter.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1582()
	}
	function onActivate3384() 
	{
	 // Calling Command 86 (Menu (Show Menu)) with CommandGroup_Parameters from 18219
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 86 ")
	}
	StyledText {
		x: 960
		y: 540
		height: 240
		width: 180
		textLabel: "Menu"
	}
	ButtonSq {
		id: button3384_3168
		x: 960
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "Menu"
		clickHandler.onClicked: onActivate3384()
	}
	function onActivate3385() 
	{
	 // Calling Command 142 (Angle) with CommandGroup_Parameters from 14562
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 142 ")
	}
	StyledText {
		x: 1440
		y: 540
		height: 240
		width: 180
		textLabel: "Angles"
	}
	ButtonSq {
		id: button3385_3169
		x: 1440
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tr.png"
		buttontext: "Angles"
		clickHandler.onClicked: onActivate3385()
	}
	function onActivate3386() 
	{
	 // Calling Command 140 (Audio Track) with CommandGroup_Parameters from 14564
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 140 ")
	}
	StyledText {
		x: 960
		y: 900
		height: 240
		width: 180
		textLabel: "Audio Options"
	}
	ButtonSq {
		id: button3386_3170
		x: 960
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "Audio Options"
		clickHandler.onClicked: onActivate3386()
	}
	function onActivate3387() 
	{
	 // Calling Command 141 (Subtitle) with CommandGroup_Parameters from 14566
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 141 ")
	}
	StyledText {
		x: 1440
		y: 900
		height: 240
		width: 180
		textLabel: "Subtitles"
	}
	ButtonSq {
		id: button3387_3171
		x: 1440
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/br.png"
		buttontext: "Subtitles"
		clickHandler.onClicked: onActivate3387()
	}
	function onActivate5412() 
	{
	 // Calling Command 847 (Live AV Path) with CommandGroup_Parameters from 17380
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 847 45 \"<%=E%>\" 252 \"1\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17381
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 0
		y: 900
		height: 240
		width: 180
		textLabel: "Direct A/V"
	}
	ButtonSq {
		id: button5412_3172
		x: 0
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Direct A/V"
		clickHandler.onClicked: onActivate5412()
	}
	function onActivate5899() 
	{
	 // Calling Command 126 (Guide) with CommandGroup_Parameters from 18025
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 126 ")
	}
	StyledText {
		x: 240
		y: 900
		height: 240
		width: 180
		textLabel: "Home"
	}
	ButtonSq {
		id: button5899_3173
		x: 240
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Home"
		clickHandler.onClicked: onActivate5899()
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
		id: button2028_3174
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
	function onActivate3558() 
	{
	}
	function onActivate5159() 
	{
	 // Calling Command 200 (Move Up) with CommandGroup_Parameters from 17137
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 200 ")
	}
	ButtonSq {
		id: button5159_3175
		x: 240
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_move_up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5159()
	}
	function onActivate5160() 
	{
	 // Calling Command 201 (Move Down) with CommandGroup_Parameters from 17138
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 201 ")
	}
	ButtonSq {
		id: button5160_3176
		x: 240
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_move_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5160()
	}
	function onActivate5161() 
	{
	 // Calling Command 203 (Move Right) with CommandGroup_Parameters from 17139
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 203 ")
	}
	ButtonSq {
		id: button5161_3177
		x: 480
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_move_right.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5161()
	}
	function onActivate5162() 
	{
	 // Calling Command 202 (Move Left) with CommandGroup_Parameters from 17140
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 202 ")
	}
	ButtonSq {
		id: button5162_3178
		x: 0
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/arrow_move_left.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5162()
	}
	function onActivate5163() 
	{
	 // Calling Command 190 (Enter/Go) with CommandGroup_Parameters from 17141
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 190 ")
	}
	StyledText {
		x: 240
		y: 180
		height: 240
		width: 180
		textLabel: "enter"
	}
	ButtonSq {
		id: button5163_3179
		x: 240
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/roundblack.png"
		buttontext: "enter"
		clickHandler.onClicked: onActivate5163()
	}
	function onActivate5504() 
	{
	 // Calling Command 548 (Menu) with CommandGroup_Parameters from 17482
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 548 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17483
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 741 ")
	}
	StyledText {
		x: 0
		y: 0
		height: 240
		width: 180
		textLabel: "Menu"
	}
	ButtonSq {
		id: button5504_3180
		x: 0
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "Menu"
		clickHandler.onClicked: onActivate5504()
	}
	function onActivate5907() 
	{
	 // Calling Command 240 (Back / Prior Menu) with CommandGroup_Parameters from 18028
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 240 ")
	}
	StyledText {
		x: 0
		y: 360
		height: 240
		width: 180
		textLabel: "<-"
	}
	ButtonSq {
		id: button5907_3181
		x: 0
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "<-"
		clickHandler.onClicked: onActivate5907()
	}
}