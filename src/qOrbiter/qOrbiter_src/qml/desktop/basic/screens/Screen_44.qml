// UI1 Screen AdvancedOptions (44)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Phone/phonebookB.png"
		width: 1920
		height: 1080
	}
	function onActivate2026() 
	{
	}
	ButtonSq {
		id: button2026_514
		x: 117
		y: 575
		width: 1444
		height: 487
		buttonbackground: "skins/Basic/10"
		buttontext: ""
		clickHandler.onClicked: onActivate2026()
	}
	function onActivate2025() 
	{
	 // Calling Command 323 (Halt Device) with CommandGroup_Parameters from 14552
		manager.sendDceMessage(srouterip + " "+deviceid + " -103 1 323 2 \"<%=5%>\" 21 \"R\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14840
		setCurrentScreen("Screen_53.qml")
	 // Calling Command 25 (Set Text) with CommandGroup_Parameters from 14841
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 25 3 \"2265\" 9 \"<%=T1154%>\" 25 \"670\" ")
	 // Calling Command 324 (Set Timeout) with CommandGroup_Parameters from 14842
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 324 3 \"2265\" 102 \"15\" ")
	}
	StyledText {
		x: 117
		y: 575
		height: 211
		width: 346
		textLabel: "Reset
<%=6:0%>"
	}
	ButtonSq {
		id: button2025_515
		x: 117
		y: 575
		width: 462
		height: 158
		buttonbackground: "skins/Basic/Buttons/midbutton.png"
		buttontext: "Reset
<%=6:0%>"
		clickHandler.onClicked: onActivate2025()
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
		id: button1785_516
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate2039() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 14735
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"2\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14736
		setCurrentScreen("Screen_38.qml")
	}
	StyledText {
		x: 0
		y: 180
		height: 240
		width: 180
		textLabel: "Infrared
codes"
	}
	ButtonSq {
		id: button2039_517
		x: 0
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Infrared
codes"
		clickHandler.onClicked: onActivate2039()
	}
	function onActivate3322() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14219
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 741 163 \"<%=T1140%>\" 181 \"0\" 182 \"0\" 183 \"1\" ")
	 // Calling Command 266 (Regen Orbiter) with CommandGroup_Parameters from 14217
		manager.sendDceMessage(srouterip + " "+deviceid + " -101 1 266 2 \"<%=!%>\" ")
	}
	StyledText {
		x: 1440
		y: 180
		height: 240
		width: 180
		textLabel: "Regen this Orbiter"
	}
	ButtonSq {
		id: button3322_518
		x: 1440
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Regen this Orbiter"
		clickHandler.onClicked: onActivate3322()
	}
	function onActivate2038() 
	{
	 // Calling Command 272 (Restart DCERouter) with CommandGroup_Parameters from 14233
		manager.sendDceMessage(srouterip + " "+deviceid + " -103 1 272 ")
	}
	StyledText {
		x: 1680
		y: 360
		height: 240
		width: 180
		textLabel: "Quick reload router"
	}
	ButtonSq {
		id: button2038_519
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Quick reload router"
		clickHandler.onClicked: onActivate2038()
	}
	function onActivate3325() 
	{
	 // Calling Command 276 (Add GC100) with CommandGroup_Parameters from 14252
		manager.sendDceMessage(srouterip + " "+deviceid + " -110 1 276 ")
	}
	StyledText {
		x: 240
		y: 180
		height: 120
		width: 180
		textLabel: "Add
GC100"
	}
	ButtonSq {
		id: button3325_520
		x: 240
		y: 180
		width: 240
		height: 90
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Add
GC100"
		clickHandler.onClicked: onActivate3325()
	}
	function onActivate3329() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14246
		setCurrentScreen("Screen_53.qml")
	 // Calling Command 266 (Regen Orbiter) with CommandGroup_Parameters from 14244
		manager.sendDceMessage(srouterip + " "+deviceid + " -101 1 266 ")
	}
	StyledText {
		x: 1200
		y: 180
		height: 240
		width: 180
		textLabel: "Regen
all
Orbiters"
	}
	ButtonSq {
		id: button3329_521
		x: 1200
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Regen
all
Orbiters"
		clickHandler.onClicked: onActivate3329()
	}
	function onActivate3330() 
	{
	 // Calling Command 265 (Quit) with CommandGroup_Parameters from 14250
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 265 ")
	}
	StyledText {
		x: 1680
		y: 900
		height: 240
		width: 180
		textLabel: "Exit
Orbiter"
	}
	ButtonSq {
		id: button3330_522
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Exit
Orbiter"
		clickHandler.onClicked: onActivate3330()
	}
	function onActivate3429() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14859
		setCurrentScreen("Screen_82.qml")
	}
	StyledText {
		x: 0
		y: 360
		height: 240
		width: 180
		textLabel: "Pending Tasks"
	}
	ButtonSq {
		id: button3429_523
		x: 0
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Pending Tasks"
		clickHandler.onClicked: onActivate3429()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_524
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate4325() 
	{
	 // Calling Command 266 (Regen Orbiter) with CommandGroup_Parameters from 16074
		manager.sendDceMessage(srouterip + " "+deviceid + " -101 1 266 2 \"<%=!%>\" 24 \"Y\" ")
	}
	StyledText {
		x: 1440
		y: 360
		height: 240
		width: 180
		textLabel: "Reload & Regen Orbiter"
	}
	ButtonSq {
		id: button4325_525
		x: 1440
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Reload & Regen Orbiter"
		clickHandler.onClicked: onActivate4325()
	}
	function onActivate4326() 
	{
	 // Calling Command 266 (Regen Orbiter) with CommandGroup_Parameters from 16075
		manager.sendDceMessage(srouterip + " "+deviceid + " -101 1 266 24 \"Y\" ")
	}
	StyledText {
		x: 1200
		y: 360
		height: 240
		width: 180
		textLabel: "Reload & Regen All Orbiters"
	}
	ButtonSq {
		id: button4326_526
		x: 1200
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Reload & Regen All Orbiters"
		clickHandler.onClicked: onActivate4326()
	}
	function onActivate4328() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16084
		setCurrentScreen("Screen_98.qml")
	}
	StyledText {
		x: 240
		y: 270
		height: 120
		width: 180
		textLabel: "Show Ping Test"
	}
	ButtonSq {
		id: button4328_527
		x: 240
		y: 270
		width: 240
		height: 90
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Show Ping Test"
		clickHandler.onClicked: onActivate4328()
	}
	function onActivate4485() 
	{
	 // Calling Command 272 (Restart DCERouter) with CommandGroup_Parameters from 16250
		manager.sendDceMessage(srouterip + " "+deviceid + " -103 1 272 21 \"Y\" ")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "Force Reload Router"
	}
	ButtonSq {
		id: button4485_528
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Force Reload Router"
		clickHandler.onClicked: onActivate4485()
	}
	function onActivate4568() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16322
		setCurrentScreen("Screen_175.qml")
	}
	StyledText {
		x: 480
		y: 360
		height: 240
		width: 180
		textLabel: "Setup Wizard"
	}
	ButtonSq {
		id: button4568_529
		x: 480
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Setup Wizard"
		clickHandler.onClicked: onActivate4568()
	}
	function onActivate4833() 
	{
	 // Calling Command 789 (Force Update Packages) with CommandGroup_Parameters from 16802
		manager.sendDceMessage(srouterip + " "+deviceid + " -103 1 789 ")
	}
	StyledText {
		x: 720
		y: 180
		height: 240
		width: 180
		textLabel: "Force Upgrade Now"
	}
	ButtonSq {
		id: button4833_530
		x: 720
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Force Upgrade Now"
		clickHandler.onClicked: onActivate4833()
	}
	function onActivate5136() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17127
		setCurrentScreen("Screen_239.qml")
	}
	StyledText {
		x: 240
		y: 360
		height: 240
		width: 180
		textLabel: "Add Software"
	}
	ButtonSq {
		id: button5136_531
		x: 240
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Add Software"
		clickHandler.onClicked: onActivate5136()
	}
	function onActivate5388() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17371
		setCurrentScreen("Screen_255.qml")
	}
	StyledText {
		x: 960
		y: 360
		height: 240
		width: 180
		textLabel: "Remote Assistance"
	}
	ButtonSq {
		id: button5388_532
		x: 960
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Remote Assistance"
		clickHandler.onClicked: onActivate5388()
	}
	function onActivate5437() 
	{
	 // Calling Command 912 (Activate PC Desktop) with CommandGroup_Parameters from 17388
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 912 119 \"1\" ")
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "KDE desktop"
	}
	ButtonSq {
		id: button5437_533
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "KDE desktop"
		clickHandler.onClicked: onActivate5437()
	}
	function onActivate5438() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17389
		setCurrentScreen("Screen_263.qml")
	}
	StyledText {
		x: 1680
		y: 900
		height: 240
		width: 180
		textLabel: "Adjust spacing and offset"
	}
	ButtonSq {
		id: button5438_534
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Adjust spacing and offset"
		clickHandler.onClicked: onActivate5438()
	}
	function onActivate5083() 
	{
	}
	StyledText {
		x: 720
		y: 360
		height: 240
		width: 180
		textLabel: "Video Res & Audio Setup"
	}
	ButtonSq {
		id: button5083_535
		x: 720
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Video Res & Audio Setup"
		clickHandler.onClicked: onActivate5083()
	}
	function onActivate5559() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17571
		setCurrentScreen("Screen_271.qml")
	}
	StyledText {
		x: 480
		y: 180
		height: 240
		width: 180
		textLabel: "Network Settings"
	}
	ButtonSq {
		id: button5559_536
		x: 480
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Network Settings"
		clickHandler.onClicked: onActivate5559()
	}
	function onActivate5961() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18089
		setCurrentScreen("Screen_174.qml")
	}
	StyledText {
		x: 960
		y: 180
		height: 240
		width: 180
		textLabel: "Clean Screen"
	}
	ButtonSq {
		id: button5961_537
		x: 960
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Clean Screen"
		clickHandler.onClicked: onActivate5961()
	}
}