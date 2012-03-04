import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: satcableboxremote
    height: appH
    width: appW
    state: ""
    color: "transparent"
    Component.onCompleted: manager.setNowPlayingData()
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
        width: scaleX(45)
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
                id: album
                wrapMode: "NoWrap"
                text: qsTr("Channel: ") + dcenowplaying.channelID
                font.family: "Droid Sans"
                font.bold: true
                smooth: true
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: scaleY(2)
                color: "white"
            }

            Text {
                id: title
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
    RemoteNumberPad {
        id: remotenumberpad1
        x: 0
        y: 0
        visible: false
    }

    ButtonSq {
        id: keypad
        width: style.stdbuttonw
        height: style.stdbuttonh
        anchors.right: satcableboxremote.right
        anchors.top: np_box.top
        buttontextfontsize: scaleY(2)
        buttontext: "#"
        MouseArea{
            smooth: true
            anchors.fill: parent
            onClicked: satcableboxremote.state="NUMBERS"
        }
    }

    ButtonSq {
        id: homebutton
       anchors.top: parent.top
       anchors.left: parent.left
        visible: false
        buttontext: "home"
        MouseArea{
            anchors.fill:parent
            onClicked: satcableboxremote.state=""
        }
    }

    ButtonSq {
        id: showepg       
        visible: true
        anchors.right: satcableboxremote.right
        anchors.top: keypad.bottom
        buttontext: "EPG"
        MouseArea{
            anchors.fill:parent
            onClicked: satcableboxremote.state="GRID"
        }
    }

    EPGPlaylist{id:playlist; anchors.top: satcableboxremote.top; anchors.horizontalCenter: satcableboxremote.horizontalCenter; visible: false }

    states: [
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
                x: 70
                y: 0
                visible: true
                anchors.horizontalCenter: satcableboxremote.horizontalCenter
            }

            PropertyChanges {
                target: keypad
                visible: false

            }

            PropertyChanges {
                target: homebutton                
                buttontextbold: true
                buttontext: "Display"
                visible:true
                anchors.top: satcableboxremote.top
                anchors.left: satcableboxremote.left
            }

            PropertyChanges{
                target: showepg
                anchors.top: homebutton.bottom
                anchors.right: satcableboxremote.right
                visible:true
            }
            PropertyChanges {
                target: metadatavideo
                visible:false

            }
        },
        State{
            name:"GRID"
            PropertyChanges
            {
                target: playlist
                visible:true
                anchors.top: satcableboxremote.top
                anchors.topMargin: 75
            }

            PropertyChanges {
                target: metadatavideo
                visible:false

            }
            PropertyChanges{
                target: showepg
                visible:false
            }
            PropertyChanges {
                target: np_box
                visible: false

            }

            PropertyChanges{
                target:homebutton
                visible:true
                anchors.right: satcableboxremote.right
                anchors.top: satcableboxremote.top
            }

            PropertyChanges {
                target: keypad
                visible: true
                anchors.right: satcableboxremote.right
                anchors.top: homebutton.bottom

            }
            PropertyChanges {
                target: remotenumberpad1
                visible: false
            }
        }

    ]


}
