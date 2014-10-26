// UI1 Screen MapMedia (51)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Backgrounds/map.png"
		width: 1920
		height: 1080
	}
	function onActivate1785() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17174
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 1676
		y: 0
		height: 1
		width: 1
		textLabel: "Main menu"
	}
	ButtonSq {
		id: button1785_630
		x: 1676
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate2218() 
	{
	}
	ButtonSq {
		id: button2218_631
		x: 27
		y: 20
		width: 1146
		height: 853
		buttonbackground: "skins/Basic/5"
		buttontext: ""
		clickHandler.onClicked: onActivate2218()
	}
	function onActivate2220() 
	{
	 // Calling Command 241 (MH Move Media) with CommandGroup_Parameters from 14001
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 241 41 \"<%=23%>\" 45 \"<%=SD%>\" ")
	 // Calling Command 258 (Clear Selected Devices) with CommandGroup_Parameters from 14134
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 258 3 \"2228.0.<%=12%>.2218\" ")
	}
	ButtonSq {
		id: button2304_632
		x: 1440
		y: 180
		width: 480
		height: 0
		buttonbackground: "skins/Basic/15"
		buttontext: ""
		clickHandler.onClicked: onActivate2304()
	}
	function onActivate2303() 
	{
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 13981
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"2228.0.<%=12%>.2218\" 8 \"0\" ")
	 // Calling Command 27 (Set Variable) with CommandGroup_Parameters from 13982
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 27 4 \"12\" 5 \"<%=5%>\" ")
	 // Calling Command 6 (Show Object) with CommandGroup_Parameters from 13983
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 6 3 \"2228.0.<%=12%>.2218\" ")
	}
	StyledText {
		x: 1440
		y: 180
		height: 236
		width: 160
		textLabel: "<%=6:0%>"
	}
	ButtonSq {
		id: button2303_633
		x: 1440
		y: 180
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: "<%=6:0%>"
		clickHandler.onClicked: onActivate2303()
	}
	function onActivate2227() 
	{
	 // Calling Command 59 (Set Entertainment Area) with CommandGroup_Parameters from 16784
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 59 45 \"<%=SD%>\" ")
	}
	function onActivate2221() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 14726
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=SD%>\" ")
	}
	ButtonSq {
		id: button2221_634
		x: 0
		y: 873
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2221()
	}
	ButtonSq {
		id: button2284_635
		x: 0
		y: 1746
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Icons/map/smallpower.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2284()
	}
	function onActivate2222() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17008
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	ButtonSq {
		id: button2222_636
		x: 0
		y: 978
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2222()
	}
	ButtonSq {
		id: button2285_637
		x: 0
		y: 1851
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Icons/map/remote.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2285()
	}
	function onActivate2223() 
	{
	 // Calling Command 372 (MH Set Volume) with CommandGroup_Parameters from 16734
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 372 45 \"<%=SD%>\" 76 \"+1\" ")
	}
	StyledText {
		x: 126
		y: 927
		height: 136
		width: 93
		textLabel: "Vol"
	}
	ButtonSq {
		id: button2223_638
		x: 126
		y: 873
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "Vol"
		clickHandler.onClicked: onActivate2223()
	}
	ButtonSq {
		id: button2286_639
		x: 126
		y: 1746
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Icons/map/plus.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2286()
	}
	function onActivate2224() 
	{
	 // Calling Command 372 (MH Set Volume) with CommandGroup_Parameters from 14730
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 372 45 \"<%=SD%>\" 76 \"-1\" ")
	}
	StyledText {
		x: 126
		y: 985
		height: 90
		width: 93
		textLabel: "Vol"
	}
	ButtonSq {
		id: button2224_640
		x: 126
		y: 978
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "Vol"
		clickHandler.onClicked: onActivate2224()
	}
	ButtonSq {
		id: button2287_641
		x: 126
		y: 1851
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Icons/map/minus.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2287()
	}
	function onActivate2225() 
	{
	 // Calling Command 65 (Jump Position In Playlist) with CommandGroup_Parameters from 14002
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 65 5 \"+1\" ")
	}
	StyledText {
		x: 253
		y: 927
		height: 136
		width: 93
		textLabel: "Ch"
	}
	ButtonSq {
		id: button2225_642
		x: 253
		y: 873
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "Ch"
		clickHandler.onClicked: onActivate2225()
	}
	ButtonSq {
		id: button2286_643
		x: 253
		y: 1746
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Icons/map/plus.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2286()
	}
	function onActivate2226() 
	{
	 // Calling Command 65 (Jump Position In Playlist) with CommandGroup_Parameters from 16733
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 65 5 \"-1\" ")
	}
	StyledText {
		x: 253
		y: 985
		height: 90
		width: 93
		textLabel: "Ch"
	}
	ButtonSq {
		id: button2226_644
		x: 253
		y: 978
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Buttons/smallsquare.png"
		buttontext: "Ch"
		clickHandler.onClicked: onActivate2226()
	}
	ButtonSq {
		id: button2287_645
		x: 253
		y: 1851
		width: 124
		height: 102
		buttonbackground: "skins/Basic/Icons/map/minus.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2287()
	}
	function onActivate3296() 
	{
	 // Calling Command 261 (Set Follow-Me) with CommandGroup_Parameters from 14353
		manager.sendDceMessage(srouterip + " "+deviceid + " -101 1 261 2 \"<%=!%>\" 9 \"M<%=GD2228.0.0.2607%>\" 17 \"<%=U%>\" ")
	}
	ButtonSq {
		id: button3296_646
		x: 1200
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3296()
	}
	ButtonSq {
		id: button2607_647
		x: 1218
		y: 3
		width: 198
		height: 171
		buttonbackground: "skins/Basic/mobile/icons/dontfolllowme.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2607()
	}
	function onActivate3417() 
	{
	 // Calling Command 388 (Set Media Private) with CommandGroup_Parameters from 14826
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 388 45 \"<%=E%>\" 119 \"<%=GD2228.0.0.3418%>\" ")
	}
	ButtonSq {
		id: button3417_648
		x: 1440
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/squareb.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3417()
	}
	ButtonSq {
		id: button3418_649
		x: 1458
		y: 3
		width: 198
		height: 171
		buttonbackground: "skins/Basic/Icons/media/notprivate.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3418()
	}
}