import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: satcableboxremote
    height: appH-hdr.height
    width: appW
    state: ""
    color: "transparent"

    Image {
        id: bg
        source: b_orientation ? "../img/storedMediaP.png" : "../img/storedMediaW.png"
        height: appH
        width: appW
    }

    NowPlayingBox{
        id:np_box
        anchors.top: satcableboxremote.top
        anchors.horizontalCenter: parent.horizontalCenter
        visible: true
    }

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
        width: b_orientation ? scaleX(45) : scaleX(100)
        height: childrenRect.height
        anchors.top: satcableboxremote.top
        anchors.left: satcableboxremote.left
        anchors.leftMargin: scaleX(5)
        color: "transparent"
        visible:false

        Column { // Info, Up, Menu, Ch+
            spacing: 1
            Row {
                spacing: 1
                ButtonSq {
                    id: btInfo
                    buttontext: "Info"
                    onActivated: manager.extraButton("info")
                }
                ButtonSq {
                    id: btUp
                    buttontext: "Up"
                    onActivated: manager.moveArrowDirection(1) //up
                }
                ButtonSq {
                    id: btMenu
                    buttontext: "Menu"
                    onActivated: manager.extraButton("menu")
                }
                ButtonSq {
                    id: btChannelUp
                    buttontext: "Ch+"
                    onActivated: manager.changeTrack("+1")
                }
            }
            Row {
                spacing: 1
                ButtonSq {
                    id: btLeft
                    buttontext: "Left"
                    onActivated: manager.moveArrowDirection(3)
                }
                ButtonSq {
                    id: btOk
                    buttontext: "Ok"
                    onActivated: manager.moveArrowDirection(5) //ok
                }
                ButtonSq {
                    id: btRight
                    buttontext: "Right"
                    onActivated: manager.moveArrowDirection(4)
                }
                ButtonSq {
                    id: btChannelDown
                    buttontext: "Ch-"
                    onActivated: manager.changeTrack("-1")
                }
            }
            Row { // Exit, Down, Guide, VolUp
                spacing: 1
                ButtonSq {
                    id: btExit
                    buttontext: "Exit"
                    onActivated: manager.extraButton("exit")
                }
                ButtonSq {
                    id: btDown
                    buttontext: "Down"
                    onActivated: manager.moveArrowDirection(2) //down
                }
                ButtonSq {
                    id: btGuide
                    buttontext: "Guide"
                    onActivated: manager.extraButton("guide")
                }
                ButtonSq {
                    id: btVolUp
                    buttontext: "Vol+"
                    onActivated: manager.adjustVolume(+1)
                }
            }
            Row { // Stop, Pause, Play, VolDown
                spacing: 1
                ButtonSq {
                    id: btStop
                    buttontext: "Stop"
                    onActivated: manager.extraButton("exit")
                }
                ButtonSq {
                    id: btPause
                    buttontext: "Pause"
                    onActivated: manager.pause()
                }
                ButtonSq {
                    id: btPlay
                    buttontext: "Play"
                    onActivated: manager.play()
                }
                ButtonSq {
                    id: btVolDown
                    buttontext: "Vol-"
                    onActivated: manager.adjustVolume(-1)
                }
            }

            Row { // SkipBack, Rewind, FastForward, SkipFwd
                spacing: 1
                ButtonSq {
                    id: btSkipBack
                    buttontext: ""
                }
                ButtonSq {
                    id: btRewind
                    buttontext: "Rewind"
		    onActivated: manager.setPlaybackSpeed(-1)
                }
                ButtonSq {
                    id: btFFwd
                    buttontext: "FFwd"
		    onActivated: manager.setPlaybackSpeed(1)
                }
                ButtonSq {
                    id: btSkipNext
                    buttontext: ""
                }
            }
    
            Row { // Red, Green, Yellow, Blue
                spacing: 1
                ButtonSq {
                    id: btRed
                    buttontext: "Red"
                    color: "red"
                    radius : style.stdbuttonw / 3
                    onActivated: manager.redButtonPress()
                }
                ButtonSq {
                    id: btGreen
                    buttontext: "Green"
                    color: "green"
                    radius : style.stdbuttonw / 3
                    onActivated: manager.greenButton()
                }
                ButtonSq {
                    id: btYellow
                    buttontext: "Yellow"
                    color: "yellow"
                    radius : style.stdbuttonw / 3
                    onActivated: manager.yellowButton()
                }
                ButtonSq {
                    id: btBlue
                    buttontext: "Blue"
                    color: "blue"
                    radius : style.stdbuttonw / 3
                    onActivated: manager.blueButton()
                }
            }
        }
    }


    Row // Info, #, EPG, Remote
    {
        id:extra_buttons
        height: childrenRect.height
        width: childrenRect.width
        anchors.bottom: satcableboxremote.bottom

        ButtonSq {
            id: homebutton
            buttontext: "Info"
            buttontextbold: true
            color: style.button_system_color_hover
            onActivated: satcableboxremote.state=""
        }

        ButtonSq {
            id: keypadSelector
            buttontext: "#"
            onActivated: satcableboxremote.state="NUMBERS"
        }

        ButtonSq {
            id: showepg
            buttontext: "EPG"
            onActivated: satcableboxremote.state="GRID"
        }
        ButtonSq {
            id: remoteSelector
            buttontext: "Remote"
            onActivated: satcableboxremote.state="REMOTE"
        }
    }

    RemoteNumberPad {
        id: remotenumberpad1
        anchors.horizontalCenter: satcableboxremote.horizontalCenter
        anchors.top: satcableboxremote.top
        anchors.topMargin: scaleX(22)
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
        anchors.top: satcableboxremote.top
        anchors.left:satcableboxremote.left
        anchors.leftMargin: 5
	width: parent.width
	height: satcableboxremote.height-extra_buttons.height
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

            PropertyChanges
            {
                target: playlist
                visible:true
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
