// UI1 Screen FileList_PlayLists (69)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Buttons/Keyboard/background.png"
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
		id: button1785_846
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate2552() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 16962
		setCurrentScreen("Screen_220.qml")
	}
	StyledText {
		x: 1680
		y: 180
		height: 240
		width: 180
		textLabel: "Remote"
	}
	ButtonSq {
		id: button2552_847
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/s1.png"
		buttontext: "Remote"
		clickHandler.onClicked: onActivate2552()
	}
	function onActivate1569() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13739
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18035
		setCurrentScreen("Screen_1.qml")
	}
	ButtonSq {
		id: button1569_848
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
	function onActivate2447() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 12619
		setCurrentScreen("Screen_65.qml")
	}
	StyledText {
		x: 1680
		y: 720
		height: 240
		width: 180
		textLabel: "Search"
	}
	ButtonSq {
		id: button2447_849
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/s4.png"
		buttontext: "Search"
		clickHandler.onClicked: onActivate2447()
	}
	function onActivate3227() 
	{
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 13946
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"3228\" ")
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 16768
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"4818\" ")
	}
	function onActivate3228() 
	{
	 // Calling Command 231 (Load Playlist) with CommandGroup_Parameters from 13945
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 231 45 \"<%=E%>\" 78 \"<%=7%>\" ")
	}
	StyledText {
		x: 1680
		y: 540
		height: 240
		width: 180
		textLabel: "Load playlist"
	}
	ButtonSq {
		id: button3228_850
		x: 1680
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Load playlist"
		clickHandler.onClicked: onActivate3228()
	}
	function onActivate3293() 
	{
	 // Calling Command 17 (Seek Data Grid) with CommandGroup_Parameters from 14154
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 17 9 \"<%=17%>\" 15 \"Playlist_<%=!%>\" ")
	}
	function onActivate2452() 
	{
	}
	function onActivate2448() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13784
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"A\" ")
	}
	StyledText {
		x: 0
		y: 0
		height: 160
		width: 120
		textLabel: "A"
	}
	ButtonSq {
		id: button2448_851
		x: 0
		y: 0
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/left_alpha.png"
		buttontext: "A"
		clickHandler.onClicked: onActivate2448()
	}
	function onActivate2449() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13785
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"B\" ")
	}
	StyledText {
		x: 160
		y: 0
		height: 160
		width: 120
		textLabel: "B"
	}
	ButtonSq {
		id: button2449_852
		x: 160
		y: 0
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/center_alpha.png"
		buttontext: "B"
		clickHandler.onClicked: onActivate2449()
	}
	function onActivate2450() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13786
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"C\" ")
	}
	StyledText {
		x: 319
		y: 0
		height: 160
		width: 120
		textLabel: "C"
	}
	ButtonSq {
		id: button2450_853
		x: 319
		y: 0
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/right_alpha.png"
		buttontext: "C"
		clickHandler.onClicked: onActivate2450()
	}
	function onActivate2451() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13787
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"D\" ")
	}
	StyledText {
		x: 0
		y: 120
		height: 160
		width: 120
		textLabel: "D"
	}
	ButtonSq {
		id: button2451_854
		x: 0
		y: 120
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/left_alpha.png"
		buttontext: "D"
		clickHandler.onClicked: onActivate2451()
	}
	function onActivate2459() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13788
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"E\" ")
	}
	StyledText {
		x: 160
		y: 120
		height: 160
		width: 120
		textLabel: "E"
	}
	ButtonSq {
		id: button2459_855
		x: 160
		y: 120
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/center_alpha.png"
		buttontext: "E"
		clickHandler.onClicked: onActivate2459()
	}
	function onActivate2460() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13789
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"F\" ")
	}
	StyledText {
		x: 319
		y: 120
		height: 160
		width: 120
		textLabel: "F"
	}
	ButtonSq {
		id: button2460_856
		x: 319
		y: 120
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/right_alpha.png"
		buttontext: "F"
		clickHandler.onClicked: onActivate2460()
	}
	function onActivate2461() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13790
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"G\" ")
	}
	StyledText {
		x: 0
		y: 240
		height: 160
		width: 120
		textLabel: "G"
	}
	ButtonSq {
		id: button2461_857
		x: 0
		y: 240
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/left_alpha.png"
		buttontext: "G"
		clickHandler.onClicked: onActivate2461()
	}
	function onActivate2462() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13791
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"H\" ")
	}
	StyledText {
		x: 160
		y: 240
		height: 160
		width: 120
		textLabel: "H"
	}
	ButtonSq {
		id: button2462_858
		x: 160
		y: 240
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/center_alpha.png"
		buttontext: "H"
		clickHandler.onClicked: onActivate2462()
	}
	function onActivate2463() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13792
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"I\" ")
	}
	StyledText {
		x: 319
		y: 240
		height: 160
		width: 120
		textLabel: "I"
	}
	ButtonSq {
		id: button2463_859
		x: 319
		y: 240
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/right_alpha.png"
		buttontext: "I"
		clickHandler.onClicked: onActivate2463()
	}
	function onActivate2464() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13793
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"J\" ")
	}
	StyledText {
		x: 0
		y: 359
		height: 160
		width: 120
		textLabel: "J"
	}
	ButtonSq {
		id: button2464_860
		x: 0
		y: 359
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/left_alpha.png"
		buttontext: "J"
		clickHandler.onClicked: onActivate2464()
	}
	function onActivate2465() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13794
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"K\" ")
	}
	StyledText {
		x: 160
		y: 359
		height: 160
		width: 120
		textLabel: "K"
	}
	ButtonSq {
		id: button2465_861
		x: 160
		y: 359
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/center_alpha.png"
		buttontext: "K"
		clickHandler.onClicked: onActivate2465()
	}
	function onActivate2466() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13795
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"L\" ")
	}
	StyledText {
		x: 319
		y: 359
		height: 160
		width: 120
		textLabel: "L"
	}
	ButtonSq {
		id: button2466_862
		x: 319
		y: 359
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/right_alpha.png"
		buttontext: "L"
		clickHandler.onClicked: onActivate2466()
	}
	function onActivate2467() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13796
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"M\" ")
	}
	StyledText {
		x: 0
		y: 479
		height: 160
		width: 120
		textLabel: "M"
	}
	ButtonSq {
		id: button2467_863
		x: 0
		y: 479
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/left_alpha.png"
		buttontext: "M"
		clickHandler.onClicked: onActivate2467()
	}
	function onActivate2468() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13797
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"N\" ")
	}
	StyledText {
		x: 160
		y: 479
		height: 160
		width: 120
		textLabel: "N"
	}
	ButtonSq {
		id: button2468_864
		x: 160
		y: 479
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/center_alpha.png"
		buttontext: "N"
		clickHandler.onClicked: onActivate2468()
	}
	function onActivate2470() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13798
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"P\" ")
	}
	StyledText {
		x: 0
		y: 599
		height: 160
		width: 120
		textLabel: "P"
	}
	ButtonSq {
		id: button2470_865
		x: 0
		y: 599
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/left_alpha.png"
		buttontext: "P"
		clickHandler.onClicked: onActivate2470()
	}
	function onActivate2471() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13799
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"Q\" ")
	}
	StyledText {
		x: 160
		y: 599
		height: 160
		width: 120
		textLabel: "Q"
	}
	ButtonSq {
		id: button2471_866
		x: 160
		y: 599
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/center_alpha.png"
		buttontext: "Q"
		clickHandler.onClicked: onActivate2471()
	}
	function onActivate2472() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13800
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"R\" ")
	}
	StyledText {
		x: 319
		y: 599
		height: 160
		width: 120
		textLabel: "R"
	}
	ButtonSq {
		id: button2472_867
		x: 319
		y: 599
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/right_alpha.png"
		buttontext: "R"
		clickHandler.onClicked: onActivate2472()
	}
	function onActivate2473() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13801
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"S\" ")
	}
	StyledText {
		x: 0
		y: 719
		height: 160
		width: 120
		textLabel: "S"
	}
	ButtonSq {
		id: button2473_868
		x: 0
		y: 719
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/left_alpha.png"
		buttontext: "S"
		clickHandler.onClicked: onActivate2473()
	}
	function onActivate2474() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13802
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"T\" ")
	}
	StyledText {
		x: 160
		y: 719
		height: 160
		width: 120
		textLabel: "T"
	}
	ButtonSq {
		id: button2474_869
		x: 160
		y: 719
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/center_alpha.png"
		buttontext: "T"
		clickHandler.onClicked: onActivate2474()
	}
	function onActivate2475() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13803
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"U\" ")
	}
	StyledText {
		x: 319
		y: 719
		height: 160
		width: 120
		textLabel: "U"
	}
	ButtonSq {
		id: button2475_870
		x: 319
		y: 719
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/right_alpha.png"
		buttontext: "U"
		clickHandler.onClicked: onActivate2475()
	}
	function onActivate2476() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13804
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"V\" ")
	}
	StyledText {
		x: 0
		y: 838
		height: 160
		width: 120
		textLabel: "V"
	}
	ButtonSq {
		id: button2476_871
		x: 0
		y: 838
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/left_alpha.png"
		buttontext: "V"
		clickHandler.onClicked: onActivate2476()
	}
	function onActivate2469() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13805
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"O\" ")
	}
	StyledText {
		x: 319
		y: 479
		height: 160
		width: 120
		textLabel: "O"
	}
	ButtonSq {
		id: button2469_872
		x: 319
		y: 479
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/right_alpha.png"
		buttontext: "O"
		clickHandler.onClicked: onActivate2469()
	}
	function onActivate2477() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13806
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"W\" ")
	}
	StyledText {
		x: 160
		y: 838
		height: 160
		width: 120
		textLabel: "W"
	}
	ButtonSq {
		id: button2477_873
		x: 160
		y: 838
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/center_alpha.png"
		buttontext: "W"
		clickHandler.onClicked: onActivate2477()
	}
	function onActivate2478() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13807
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"X\" ")
	}
	StyledText {
		x: 319
		y: 838
		height: 160
		width: 120
		textLabel: "X"
	}
	ButtonSq {
		id: button2478_874
		x: 319
		y: 838
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/right_alpha.png"
		buttontext: "X"
		clickHandler.onClicked: onActivate2478()
	}
	function onActivate2479() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13808
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"Y\" ")
	}
	StyledText {
		x: 0
		y: 958
		height: 160
		width: 120
		textLabel: "Y"
	}
	ButtonSq {
		id: button2479_875
		x: 0
		y: 958
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/left_alpha.png"
		buttontext: "Y"
		clickHandler.onClicked: onActivate2479()
	}
	function onActivate2480() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13809
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"Z\" ")
	}
	StyledText {
		x: 160
		y: 958
		height: 160
		width: 120
		textLabel: "Z"
	}
	ButtonSq {
		id: button2480_876
		x: 160
		y: 958
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/center_alpha.png"
		buttontext: "Z"
		clickHandler.onClicked: onActivate2480()
	}
	function onActivate2481() 
	{
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13810
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"17\" 5 \"#\" ")
	}
	StyledText {
		x: 319
		y: 958
		height: 160
		width: 120
		textLabel: "OK"
	}
	ButtonSq {
		id: button2481_877
		x: 319
		y: 958
		width: 160
		height: 120
		buttonbackground: "skins/Basic/Buttons/Keyboard/right_alpha.png"
		buttontext: "OK"
		clickHandler.onClicked: onActivate2481()
	}
	function onActivate4818() 
	{
	 // Calling Command 780 (Remove playlist) with CommandGroup_Parameters from 16772
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 780 78 \"<%=7%>\" ")
	 // Calling Command 14 (Refresh) with CommandGroup_Parameters from 16821
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 14 15 \"*\" ")
	}
	StyledText {
		x: 1680
		y: 360
		height: 240
		width: 180
		textLabel: "Delete
Playlist"
	}
	ButtonSq {
		id: button4818_878
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Delete
Playlist"
		clickHandler.onClicked: onActivate4818()
	}
}