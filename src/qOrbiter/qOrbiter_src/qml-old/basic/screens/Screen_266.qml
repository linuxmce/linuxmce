// UI1 Screen Legacy PVR Cable Box (266)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
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
		id: button1785_2738
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_2739
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate2416() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 15943
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 28 26 \"181\" 50 \"/\" ")
	}
	StyledText {
		x: 271
		y: 271
		height: 90
		width: 338
		textLabel: "fav."
	}
	ButtonSq {
		id: button2416_2740
		x: 271
		y: 271
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/tr.png"
		buttontext: "fav."
		clickHandler.onClicked: onActivate2416()
	}
	function onActivate4775() 
	{
	 // Calling Command 762 (Live TV) with CommandGroup_Parameters from 16683
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 762 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17310
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 271
		y: 541
		height: 90
		width: 338
		textLabel: "live"
	}
	ButtonSq {
		id: button4775_2741
		x: 271
		y: 541
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "live"
		clickHandler.onClicked: onActivate4775()
	}
	function onActivate2445() 
	{
	 // Calling Command 126 (Guide) with CommandGroup_Parameters from 16669
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 126 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17308
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 271
		y: 68
		height: 90
		width: 338
		textLabel: "guide"
	}
	ButtonSq {
		id: button2445_2742
		x: 271
		y: 68
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "guide"
		clickHandler.onClicked: onActivate2445()
	}
	function onActivate4771() 
	{
	 // Calling Command 548 (Menu) with CommandGroup_Parameters from 16672
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 548 9 \"old_recordings\" ")
	}
	StyledText {
		x: 271
		y: 135
		height: 90
		width: 338
		textLabel: "Recordings"
	}
	ButtonSq {
		id: button4771_2743
		x: 271
		y: 135
		width: 451
		height: 68
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
		x: 271
		y: 203
		height: 90
		width: 338
		textLabel: "Schedule"
	}
	ButtonSq {
		id: button4772_2744
		x: 271
		y: 203
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Schedule
"
		clickHandler.onClicked: onActivate4772()
	}
	function onActivate4288() 
	{
	 // Calling Command 91 (Input Select) with CommandGroup_Parameters from 17353
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 91 ")
	}
	StyledText {
		x: 271
		y: 677
		height: 90
		width: 338
		textLabel: "Change Inputs"
	}
	ButtonSq {
		id: button4288_2745
		x: 271
		y: 677
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/roundblack.png"
		buttontext: "Change Inputs"
		clickHandler.onClicked: onActivate4288()
	}
	function onActivate1574() 
	{
	 // Calling Command 42 (Jump to Position in Stream) with CommandGroup_Parameters from 16682
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 42 5 \"-10\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17311
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 271
		y: 338
		height: 90
		width: 338
		textLabel: "back"
	}
	ButtonSq {
		id: button1574_2746
		x: 271
		y: 338
		width: 451
		height: 68
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
		x: 271
		y: 0
		height: 90
		width: 338
		textLabel: "info"
	}
	ButtonSq {
		id: button4770_2747
		x: 271
		y: 0
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "info"
		clickHandler.onClicked: onActivate4770()
	}
	function onActivate2028() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 9488
		setCurrentScreen("Screen_52.qml")
	}
	StyledText {
		x: 271
		y: 474
		height: 90
		width: 338
		textLabel: "Resend
A/V
codes"
	}
	ButtonSq {
		id: button2028_2748
		x: 271
		y: 474
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Resend
A/V
codes"
		clickHandler.onClicked: onActivate2028()
	}
	function onActivate5365() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 17338
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	}
	StyledText {
		x: 361
		y: 744
		height: 90
		width: 338
		textLabel: "Off"
	}
	ButtonSq {
		id: button5365_2749
		x: 271
		y: 744
		width: 451
		height: 68
		buttonbackground: "skins/Basic/menu2/Buttons/NowPlaying/shortbutton2.png"
		buttontext: "Off"
		clickHandler.onClicked: onActivate5365()
	}
	function onActivate5396() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17707
		setCurrentScreen("Screen_257.qml")
	}
	StyledText {
		x: 271
		y: 406
		height: 240
		width: 180
		textLabel: "Thumbnail"
	}
	ButtonSq {
		id: button5396_2750
		x: 271
		y: 406
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "Thumbnail"
		clickHandler.onClicked: onActivate5396()
	}
	function onActivate5681() 
	{
	 // Calling Command 409 (Save Bookmark) with CommandGroup_Parameters from 17776
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 409 17 \"<%=U%>\" 29 \"1\" 45 \"<%=E%>\" 163 \"Bookmark added from orbiter <%=!%>\" 188 \" CHAN:<%=35%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18208
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	ButtonSq {
		id: button5681_2751
		x: 271
		y: 609
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/br.png"
		buttontext: ""
		clickHandler.onClicked: onActivate5681()
	}
	function onActivate5509() 
	{
	 // Calling Command 476 (Day Down) with CommandGroup_Parameters from 17496
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 476 ")
	}
	StyledText {
		x: 1173
		y: 0
		height: 90
		width: 338
		textLabel: "Next day"
	}
	ButtonSq {
		id: button5509_2752
		x: 1173
		y: 0
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "Next day"
		clickHandler.onClicked: onActivate5509()
	}
	function onActivate5510() 
	{
	 // Calling Command 477 (Day Up) with CommandGroup_Parameters from 17497
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 477 ")
	}
	StyledText {
		x: 1173
		y: 68
		height: 90
		width: 338
		textLabel: "Previous day"
	}
	ButtonSq {
		id: button5510_2753
		x: 1173
		y: 68
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "Previous day"
		clickHandler.onClicked: onActivate5510()
	}
	function onActivate5511() 
	{
	 // Calling Command 571 (Page Down) with CommandGroup_Parameters from 17500
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 571 ")
	}
	StyledText {
		x: 1173
		y: 135
		height: 90
		width: 338
		textLabel: "Page down"
	}
	ButtonSq {
		id: button5511_2754
		x: 1173
		y: 135
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "Page down"
		clickHandler.onClicked: onActivate5511()
	}
	function onActivate5512() 
	{
	 // Calling Command 572 (Page Up) with CommandGroup_Parameters from 17501
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 572 ")
	}
	StyledText {
		x: 1173
		y: 203
		height: 90
		width: 338
		textLabel: "Page up"
	}
	ButtonSq {
		id: button5512_2755
		x: 1173
		y: 203
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "Page up"
		clickHandler.onClicked: onActivate5512()
	}
	function onActivate2445() 
	{
	 // Calling Command 126 (Guide) with CommandGroup_Parameters from 16669
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 126 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17308
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 722
		y: 0
		height: 90
		width: 338
		textLabel: "guide"
	}
	ButtonSq {
		id: button2445_2756
		x: 722
		y: 0
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "guide"
		clickHandler.onClicked: onActivate2445()
	}
	function onActivate5504() 
	{
	 // Calling Command 548 (Menu) with CommandGroup_Parameters from 17482
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 548 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17483
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 741 ")
	}
	StyledText {
		x: 722
		y: 68
		height: 90
		width: 338
		textLabel: "Menu"
	}
	ButtonSq {
		id: button5504_2757
		x: 722
		y: 68
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "Menu"
		clickHandler.onClicked: onActivate5504()
	}
	function onActivate5507() 
	{
	 // Calling Command 124 (Favorites) with CommandGroup_Parameters from 17484
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 124 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17485
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 722
		y: 135
		height: 90
		width: 338
		textLabel: "fav."
	}
	ButtonSq {
		id: button5507_2758
		x: 722
		y: 135
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "fav."
		clickHandler.onClicked: onActivate5507()
	}
	function onActivate4775() 
	{
	 // Calling Command 762 (Live TV) with CommandGroup_Parameters from 16683
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 762 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17310
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 722
		y: 203
		height: 90
		width: 338
		textLabel: "live"
	}
	ButtonSq {
		id: button4775_2759
		x: 722
		y: 203
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "live"
		clickHandler.onClicked: onActivate4775()
	}
	function onActivate1574() 
	{
	 // Calling Command 42 (Jump to Position in Stream) with CommandGroup_Parameters from 16682
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 42 5 \"-10\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17311
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 722
		y: 271
		height: 90
		width: 338
		textLabel: "back"
	}
	ButtonSq {
		id: button1574_2760
		x: 722
		y: 271
		width: 451
		height: 68
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
		x: 722
		y: 338
		height: 90
		width: 338
		textLabel: "info"
	}
	ButtonSq {
		id: button4770_2761
		x: 722
		y: 338
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "info"
		clickHandler.onClicked: onActivate4770()
	}
	function onActivate2028() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 9488
		setCurrentScreen("Screen_52.qml")
	}
	StyledText {
		x: 722
		y: 406
		height: 90
		width: 338
		textLabel: "Resend
A/V
codes"
	}
	ButtonSq {
		id: button2028_2762
		x: 722
		y: 406
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Resend
A/V
codes"
		clickHandler.onClicked: onActivate2028()
	}
	function onActivate1566() 
	{
	 // Calling Command 102 (Record) with CommandGroup_Parameters from 13636
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 102 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18381
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	ButtonSq {
		id: button1566_2763
		x: 722
		y: 474
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/record.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1566()
	}
	function onActivate5412() 
	{
	 // Calling Command 847 (Live AV Path) with CommandGroup_Parameters from 17380
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 847 45 \"<%=E%>\" 252 \"1\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17381
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 722
		y: 541
		height: 90
		width: 338
		textLabel: "Direct A/V"
	}
	ButtonSq {
		id: button5412_2764
		x: 722
		y: 541
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Direct A/V"
		clickHandler.onClicked: onActivate5412()
	}
	function onActivate5506() 
	{
	 // Calling Command 919 (On Demand) with CommandGroup_Parameters from 17486
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 919 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17487
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 722
		y: 609
		height: 90
		width: 338
		textLabel: "On Demand"
	}
	ButtonSq {
		id: button5506_2765
		x: 722
		y: 609
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "On Demand"
		clickHandler.onClicked: onActivate5506()
	}
	function onActivate5469() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17400
		setCurrentScreen("Screen_265.qml")
	}
	StyledText {
		x: 722
		y: 677
		height: 90
		width: 338
		textLabel: "Zoom & Aspect"
	}
	ButtonSq {
		id: button5469_2766
		x: 722
		y: 677
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Zoom & Aspect"
		clickHandler.onClicked: onActivate5469()
	}
	function onActivate5513() 
	{
	 // Calling Command 368 (Help) with CommandGroup_Parameters from 17498
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 368 ")
	}
	StyledText {
		x: 722
		y: 744
		height: 90
		width: 338
		textLabel: "Help"
	}
	ButtonSq {
		id: button5513_2767
		x: 722
		y: 744
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "Help"
		clickHandler.onClicked: onActivate5513()
	}
	function onActivate5505() 
	{
	 // Calling Command 592 (PVR Menu) with CommandGroup_Parameters from 17502
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 592 ")
	}
	StyledText {
		x: 722
		y: 812
		height: 90
		width: 338
		textLabel: "PVR Menu"
	}
	ButtonSq {
		id: button5505_2768
		x: 722
		y: 812
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "PVR Menu"
		clickHandler.onClicked: onActivate5505()
	}
	function onActivate5508() 
	{
	 // Calling Command 528 (Instant Replay) with CommandGroup_Parameters from 17499
		manager.sendDceMessage(srouterip + " "+deviceid + " -304 1 528 ")
	}
	StyledText {
		x: 722
		y: 880
		height: 90
		width: 338
		textLabel: "back"
	}
	ButtonSq {
		id: button5508_2769
		x: 722
		y: 880
		width: 451
		height: 68
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "back"
		clickHandler.onClicked: onActivate5508()
	}
	function onActivate5159() 
	{
	 // Calling Command 200 (Move Up) with CommandGroup_Parameters from 17137
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 200 ")
	}
	ButtonSq {
		id: button5159_2770
		x: 1440
		y: 540
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
		id: button5160_2771
		x: 1440
		y: 900
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
		id: button5161_2772
		x: 1680
		y: 720
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
		id: button5162_2773
		x: 1200
		y: 720
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
		x: 1440
		y: 720
		height: 240
		width: 180
		textLabel: "enter"
	}
	ButtonSq {
		id: button5163_2774
		x: 1440
		y: 720
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
		x: 1200
		y: 540
		height: 240
		width: 180
		textLabel: "Menu"
	}
	ButtonSq {
		id: button5504_2775
		x: 1200
		y: 540
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
		x: 1200
		y: 900
		height: 240
		width: 180
		textLabel: "<-"
	}
	ButtonSq {
		id: button5907_2776
		x: 1200
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "<-"
		clickHandler.onClicked: onActivate5907()
	}
}