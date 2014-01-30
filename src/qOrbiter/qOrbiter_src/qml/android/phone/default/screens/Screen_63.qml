import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: satcableboxremote
    height: appH
    width: appW
    state: ""
    color: "transparent"

    Image {
        id: bg
        source: b_orientation ? "../img/storedMediaP.png" : "../img/storedMediaW.png"
        height: appH
        width: appW
    }

    NowPlayingBox{id:np_box
        anchors.top: satcableboxremote.top
        anchors.horizontalCenter: parent.horizontalCenter
        visible: true
    }

    HaControls{id:automation_controls; anchors.top: home.bottom; anchors.left:home.left}

    AudioRemote {
        id: audioremote1
        anchors.bottom: satcableboxremote.bottom
    }
    HomeButton{id:home; anchors.leftMargin: 5; anchors.topMargin: 5;anchors.left: satcableboxremote.left; anchors.top:satcableboxremote.top}

    Rectangle {
        id: metadatavideo
        width: b_orientation ? scaleX(45) : scaleX(65)
        height: childrenRect.height
        anchors.top: np_box.bottom
        anchors.left: automation_controls.right
        color: "transparent"
        visible:true

        Image {
            id: npgloss
            source: "../img/widegreyshape.png"
            height:parent.height
            width:parent.width
            smooth:true
            opacity: .35
        }

        Column
        {
            id:textcol
            width: scaleX(45)
            spacing: 5
            height: childrenRect.height

            Text {
                id: artist
                width: parent.width
                text: qsTr("Device: ")  + dcenowplaying.qs_mainTitle
                font.family: "Droid Sans"
                wrapMode: "NoWrap"
                font.bold: true
                smooth: true
                font.pixelSize: scaleY(3)
                color: "orange"
            }

            Text {
                id: network_id
                wrapMode: "NoWrap"
                text: qsTr("Network: ") + dcenowplaying.channelID
                font.family: "Droid Sans"
                font.bold: true
                smooth: true
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: scaleY(2)
                color: "white"
            }


            Text {
                id: channel_id
                wrapMode: "NoWrap"
                text: qsTr("Channel: ") + dcenowplaying.channel
                font.family: "Droid Sans"
                font.bold: true
                smooth: true
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: scaleY(2)
                color: "white"
            }

            Text {
                id: program_title
                wrapMode: "NoWrap"
                text: qsTr("Program:") + dcenowplaying.tvProgram
                font.family: "Droid Sans"
                font.bold: true
                smooth: true
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: scaleY(2)
                color: "white"
            }
        }
    }

    Rectangle {
        id: remote1
        width: b_orientation ? scaleX(45) : scaleX(65)
        height: childrenRect.height
        anchors.top: satcableboxremote.top
        anchors.left: home.right
        color: "transparent"
        visible:false

	Column {
            spacing: 1
            Row {
	        spacing: 1
            ButtonSq {
	        id: btInfo
		
            	buttontextfontsize: scaleY(2)
            	buttontext: "Info"
		imgSource: ""
            	MouseArea{
		    smooth: true
                    anchors.fill: parent
                    onClicked: manager.moveArrowDirection(3)
            	}
            }
            ButtonSq {
                id: btUp

                buttontextfontsize: scaleY(2)
                buttontext: "Up"
		imgSource: ""
                MouseArea{
                    smooth: true
                    anchors.fill: parent
                    onClicked: manager.moveArrowDirection(1) //up
                }
            }
            ButtonSq {
                id: bt22

                buttontextfontsize: scaleY(2)
                buttontext: ""
		imgSource: ""
                MouseArea{
                    smooth: true
                    anchors.fill: parent
                    onClicked: manager.moveArrowDirection(4)
                }
            }
            ButtonSq {
                id: btChannel

                buttontextfontsize: scaleY(2)
                buttontext: ""
		imgSource: ""
                MouseArea{
                    smooth: true
                    anchors.fill: parent
                    onClicked: manager.changeTrack("+1")
                }
            }
        }
	Row {
	        spacing: 1
            ButtonSq {
	        id: btLeft
		
            	buttontextfontsize: scaleY(2)
            	buttontext: "Left"
		imgSource: ""
            	MouseArea{
		    smooth: true
                    anchors.fill: parent
                    onClicked: manager.moveArrowDirection(3)
            	}
            }
            ButtonSq {
                id: btOk

                buttontextfontsize: scaleY(2)
                buttontext: "Ok"
		imgSource: ""
                MouseArea{
                    smooth: true
                    anchors.fill: parent
                    onClicked: manager.moveArrowDirection(5) //ok
                }
            }
            ButtonSq {
                id: btRight

                buttontextfontsize: scaleY(2)
                buttontext: "Right"
		imgSource: ""
                MouseArea{
                    smooth: true
                    anchors.fill: parent
                    onClicked: manager.moveArrowDirection(4)
                }
            }
            ButtonSq {
                id: btChannelUp

                buttontextfontsize: scaleY(2)
                buttontext: "Ch+"
		imgSource: ""
                MouseArea{
                    smooth: true
                    anchors.fill: parent
                    onClicked: manager.changeTrack("+1")
                }
            }
        }
	Row {
	        spacing: 1
            ButtonSq {
	        id: btExit
		
            	buttontextfontsize: scaleY(2)
            	buttontext: "Exit"
		imgSource: ""
            	MouseArea{
		    smooth: true
                    anchors.fill: parent
//                    onClicked: manager.()
            	}
            }
            ButtonSq {
                id: btDown

                buttontextfontsize: scaleY(2)
                buttontext: "Down"
		imgSource: ""
                MouseArea{
                    smooth: true
                    anchors.fill: parent
                    onClicked: manager.moveArrowDirection(2) //down
                }
            }
            ButtonSq {
                id: btGuide

                buttontextfontsize: scaleY(2)
                buttontext: "Guide"
		imgSource: ""
                MouseArea{
                    smooth: true
                    anchors.fill: parent
                    onClicked: manager.TvGuide()
                }
            }
            ButtonSq {
                id: btChannelDown

                buttontextfontsize: scaleY(2)
                buttontext: "Ch-"
		imgSource: ""
                MouseArea{
                    smooth: true
                    anchors.fill: parent
                    onClicked: manager.changeTrack("-1")
                }
            }
        }
	Row {
	        spacing: 1
            ButtonSq {
	        id: btRed
		
            	buttontextfontsize: scaleY(2)
            	buttontext: "Red"
	    	color: "red"
		imgSource: ""
		radius : style.stdbuttonw / 3
            	MouseArea{
		    smooth: true
                    anchors.fill: parent
                    onClicked: manager.redButtonPress()
            	}
            }
            ButtonSq {
                id: btGreen

                buttontextfontsize: scaleY(2)
                buttontext: "Green"
	        color: "green"
		imgSource: ""
		radius : style.stdbuttonw / 3
                MouseArea{
                    smooth: true
                    anchors.fill: parent
                    onClicked: manager.greenButton()
                }
            }
            ButtonSq {
                id: btYellow

                buttontextfontsize: scaleY(2)
                buttontext: "Yellow"
	        color: "yellow"
		imgSource: ""
		radius : style.stdbuttonw / 3
                MouseArea{
                    smooth: true
                    anchors.fill: parent
                    onClicked: manager.yellowButton()
                }
            }
            ButtonSq {
                id: btBlue

                buttontextfontsize: scaleY(2)
                buttontext: "Blue"
	        color: "blue"
		imgSource: ""
		radius : style.stdbuttonw / 3
                MouseArea{
                    smooth: true
                    anchors.fill: parent
                    onClicked: manager.blueButton()
                }
            }
        }
    }
}


    Row
    {
        id:extra_buttons
        height: childrenRect.height
        width: childrenRect.width
        anchors.bottom: audioremote1.top

        ButtonSq {
            id: homebutton
            buttontext: "Info"
	    color: style.button_system_color_hover
            buttontextbold: true
	    imgSource: ""
            MouseArea{
                anchors.fill:parent
                onClicked: satcableboxremote.state=""
            }
        }

        ButtonSq {
            id: keypadSelector
            buttontext: "#"
	    color: style.button_system_color
	    imgSource: ""
            buttontextfontsize: scaleY(2)
            MouseArea{
                anchors.fill: parent
                onClicked: satcableboxremote.state="NUMBERS"
            }
        }

        ButtonSq {
            id: showepg
            buttontext: "EPG"
	    color: style.button_system_color
	    imgSource: ""
            MouseArea{
                anchors.fill:parent
                onClicked: satcableboxremote.state="GRID"
            }
        }
        ButtonSq {
            id: remoteSelector
            buttontext: "Remote"
	    color: style.button_system_color
	    imgSource: ""
            MouseArea{
                anchors.fill:parent
                onClicked: satcableboxremote.state="REMOTE"
            }
        }
    }

    RemoteNumberPad {
        id: remotenumberpad1
        anchors.right: satcableboxremote.right
        anchors.top: satcableboxremote.top
        anchors.topMargin: scaleY(22)
        visible: false

        Transition {
            to: "NUMBERS"
            AnchorAnimation{
                duration: 1000
                easing.type: Easing.InBounce
            }
        }

    }

    EPGPlaylist{
        id:playlist;
        anchors.bottom: satcableboxremote.top;
        anchors.left:satcableboxremote.left
        anchors.leftMargin: scaleX(22)
        visible: false
       Transition {

            to: "GRID"
            AnchorAnimation{
                duration: 2000
                easing.type: Easing.InBounce
            }
        }
    }


    states: [
        State {
            name: ""

            AnchorChanges{
                target:remotenumberpad1
                anchors.top: undefined
                anchors.right: undefined
                anchors.horizontalCenter: satcableboxremote.horizontalCenter
            }

            PropertyChanges {
                target: homebutton
                buttontextbold: true
		color: style.button_system_color_hover
            }
            PropertyChanges {
                target: keypadSelector
                buttontextbold: false
		color: style.button_system_color
            }
            PropertyChanges{
                target: showepg
                buttontextbold: false
		color: style.button_system_color
            }
            PropertyChanges{
                target: remoteSelector
                buttontextbold: false
		color: style.button_system_color
            }
            PropertyChanges {
                target: metadatavideo
                visible:true
            }
            PropertyChanges {
                target: textcol
                visible: false
            }
            PropertyChanges {
                target: np_box
                visible: false

            }
            PropertyChanges {
                target: remotenumberpad1
                visible: false
            }
            PropertyChanges {
                target: remote1
                visible: false
            }
        },
        State {
            name: "NUMBERS"

            AnchorChanges{
                target:remotenumberpad1
                anchors.top: undefined
                anchors.right: undefined
                anchors.horizontalCenter: satcableboxremote.horizontalCenter
            }

            PropertyChanges {
                target: textcol
                visible: false
            }
            PropertyChanges {
                target: np_box
                visible: false

            }

            PropertyChanges {
                target: remotenumberpad1
                visible: true
            }

            PropertyChanges {
                target: keypadSelector
                buttontextbold: true
		color: style.button_system_color_hover
            }

            PropertyChanges {
                target: homebutton
                buttontextbold: false
		color: style.button_system_color
            }

            PropertyChanges{
                target: showepg
                visible:true
            }
            PropertyChanges{
                target: remoteSelector
                buttontextbold: false
		color: style.button_system_color
            }
            PropertyChanges {
                target: metadatavideo
                visible:false
            }
            PropertyChanges {
                target: remote1
                visible: false
            }
        },
        State{
            name:"GRID"

            AnchorChanges{
                target: playlist
                anchors.bottom: undefined
                anchors.top: satcableboxremote.top
            }

            AnchorChanges{
                target:remotenumberpad1
                anchors.right: satcableboxremote.right
                anchors.top: satcableboxremote.top
                anchors.horizontalCenter: undefined
            }

            PropertyChanges
            {
                target: playlist
                visible:true
                anchors.topMargin: scaleY(15)
            }

            PropertyChanges {
                target: metadatavideo
                visible:false

            }
            PropertyChanges {
                target: np_box
                visible: false

            }

            PropertyChanges{
                target:homebutton
                buttontextbold: false
		color: style.button_system_color
            }

            PropertyChanges {
                target: keypadSelector
		color: style.button_system_color
                buttontextbold: false
            }
            PropertyChanges{
                target: showepg
                buttontextbold: true
		color: style.button_system_color_hover
            }
            PropertyChanges{
                target: remoteSelector
                buttontextbold: false
		color: style.button_system_color
            }
            PropertyChanges {
                target: remotenumberpad1
                visible: false
            }
            PropertyChanges {
                target: remote1
                visible: false
            }

        },
        State{
            name:"REMOTE"

            AnchorChanges{
                target: playlist
                anchors.bottom: undefined
                anchors.top: satcableboxremote.top
            }

            AnchorChanges{
                target:remotenumberpad1
                anchors.right: satcableboxremote.right
                anchors.top: satcableboxremote.top
                anchors.horizontalCenter: undefined
            }

            PropertyChanges{
                target: showepg
		color: style.button_system_color
                buttontextbold: false
            }
            PropertyChanges{
                target:homebutton
                buttontextbold: false
		color: style.button_system_color
            }
            PropertyChanges {
                target: keypadSelector
		color: style.button_system_color
                buttontextbold: false
            }
            PropertyChanges{
                target: remoteSelector
                buttontextbold: true
		color: style.button_system_color_hover
            }
            PropertyChanges {
                target: remotenumberpad1
		visible: false
            }
            PropertyChanges {
                target: remote1
                visible: true
            }
            PropertyChanges {
                target: np_box
                visible: false
            }
            PropertyChanges
            {
                target: playlist
                visible:false
            }

            PropertyChanges {
                target: metadatavideo
                visible:false

            }
        }

    ]


}
