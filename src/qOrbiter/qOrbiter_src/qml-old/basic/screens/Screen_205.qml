// UI1 Screen Need Regen Orbiter (205)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate4769() 
	{
	 // Calling Command 8 (Remove Screen From History) with CommandGroup_Parameters from 17231
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 8 159 \"205\" ")
	}
	StyledText {
		x: 496
		y: 677
		height: 213
		width: 698
		textLabel: "No, I'll do it later"
	}
	ButtonSq {
		id: button4769_2343
		x: 496
		y: 677
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "No, I'll do it later"
		clickHandler.onClicked: onActivate4769()
	}
	function onActivate4767() 
	{
	 // Calling Command 25 (Set Text) with CommandGroup_Parameters from 16662
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 25 3 \"2265\" 9 \"<%=T1140%>\" 25 \"670\" ")
	 // Calling Command 266 (Regen Orbiter) with CommandGroup_Parameters from 16664
		manager.sendDceMessage(srouterip + " "+deviceid + " -101 1 266 2 \"<%=!%>\" ")
	 // Calling Command 8 (Remove Screen From History) with CommandGroup_Parameters from 17230
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 8 159 \"205\" ")
	}
	StyledText {
		x: 496
		y: 271
		height: 213
		width: 698
		textLabel: "Yes, regenerate the screens for  this Orbiter"
	}
	ButtonSq {
		id: button4767_2344
		x: 496
		y: 271
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Yes, regenerate the screens for  this Orbiter"
		clickHandler.onClicked: onActivate4767()
	}
	function onActivate4768() 
	{
	 // Calling Command 25 (Set Text) with CommandGroup_Parameters from 16665
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 25 3 \"2265\" 9 \"<%=T1130%>\" 25 \"670\" ")
	 // Calling Command 266 (Regen Orbiter) with CommandGroup_Parameters from 16667
		manager.sendDceMessage(srouterip + " "+deviceid + " -101 1 266 ")
	 // Calling Command 8 (Remove Screen From History) with CommandGroup_Parameters from 17229
		manager.sendDceMessage(srouterip + " "+deviceid + " -305 1 8 159 \"205\" ")
	}
	StyledText {
		x: 496
		y: 474
		height: 213
		width: 698
		textLabel: "Yes, regenerate all Orbiters in my home"
	}
	ButtonSq {
		id: button4768_2345
		x: 496
		y: 474
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Yes, regenerate all Orbiters in my home"
		clickHandler.onClicked: onActivate4768()
	}
	function onActivate5203() 
	{
	 // Calling Command 8 (Remove Screen From History) with CommandGroup_Parameters from 17234
		manager.sendDceMessage(srouterip + " "+deviceid + " -305 1 8 159 \"205\" ")
	}
	StyledText {
		x: 496
		y: 880
		height: 213
		width: 698
		textLabel: "No for all Orbiters"
	}
	ButtonSq {
		id: button5203_2346
		x: 496
		y: 880
		width: 930
		height: 160
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "No for all Orbiters"
		clickHandler.onClicked: onActivate5203()
	}
}