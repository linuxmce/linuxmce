// UI1 Screen PCFileTools (43)
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
		x: 559
		y: 34
		height: 180
		width: 508
		textLabel: "Save To Private"
	}
	ButtonSq {
		id: button2014_503
		x: 343
		y: 20
		width: 930
		height: 167
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Save To Private"
		clickHandler.onClicked: onActivate2014()
	}
	ButtonSq {
		id: button2020_504
		x: 343
		y: 10
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Icons/other/disk.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2020()
	}
	StyledText {
		x: 559
		y: 203
		height: 180
		width: 508
		textLabel: "Save To Public"
	}
	ButtonSq {
		id: button2015_505
		x: 343
		y: 189
		width: 930
		height: 167
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Save To Public"
		clickHandler.onClicked: onActivate2015()
	}
	ButtonSq {
		id: button2020_506
		x: 343
		y: 179
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Icons/other/disk.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2020()
	}
	StyledText {
		x: 559
		y: 372
		height: 180
		width: 508
		textLabel: "Save to Specific Folder"
	}
	ButtonSq {
		id: button2016_507
		x: 343
		y: 359
		width: 930
		height: 166
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Save to Specific Folder"
		clickHandler.onClicked: onActivate2016()
	}
	ButtonSq {
		id: button1751_508
		x: 343
		y: 348
		width: 213
		height: 176
		buttonbackground: "skins/Basic/Icons/changefolder.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1751()
	}
	StyledText {
		x: 559
		y: 541
		height: 180
		width: 508
		textLabel: "Print"
	}
	ButtonSq {
		id: button2017_509
		x: 343
		y: 528
		width: 930
		height: 167
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Print"
		clickHandler.onClicked: onActivate2017()
	}
	ButtonSq {
		id: button2019_510
		x: 343
		y: 518
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Icons/other/printer.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2019()
	}
	StyledText {
		x: 559
		y: 711
		height: 180
		width: 508
		textLabel: "Email"
	}
	ButtonSq {
		id: button2018_511
		x: 343
		y: 697
		width: 930
		height: 167
		buttonbackground: "skins/Basic/Buttons/fullbutton.png"
		buttontext: "Email"
		clickHandler.onClicked: onActivate2018()
	}
	ButtonSq {
		id: button2021_512
		x: 343
		y: 687
		width: 213
		height: 177
		buttonbackground: "skins/Basic/Icons/other/mail.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2021()
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_513
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
}