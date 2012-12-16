import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: satcableboxremote
    height: 480
    width: 320
    state: ""
    color: "transparent"
    NowPlayingBox{id:np_box
        anchors.top: satcableboxremote.top
        anchors.horizontalCenter: parent.horizontalCenter
        visible: true
    }


    HaControls{ x: 8; y: 56}

    AudioRemote {
        id: audioremote1
        anchors.bottom: satcableboxremote.bottom
    }
    HomeButton{ x: 8; y: 5; anchors.leftMargin: 275; anchors.topMargin: 5;anchors.left: storedAudioRemote.left; anchors.top:parent.top}

    RemoteNumberPad {
        id: remotenumberpad1
        x: 0
        y: 0
        visible: false
    }

    ButtonSq {
        id: keypad
        width: 45
        height: 45
        anchors.right: satcableboxremote.right
        anchors.top: np_box.top
        buttontextfontsize: 28.600
        buttontext: "#"
        MouseArea{
            smooth: true
            anchors.fill: parent
            onClicked: satcableboxremote.state="NUMBERS"
        }
    }

    ButtonSq {
        id: homebutton
        x: 196
        y: 294
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
            }

            PropertyChanges {
                target: keypad
                visible: false

            }

            PropertyChanges {
                target: homebutton
                x: 265
                y: 5
                width: style.stdbuttonw
                height: style.stdbuttonh
                buttontextbold: true
                buttontext: "Display"
                visible:true
                anchors.right: satcableboxremote.right
                anchors.top:  satcableboxremote.top
            }

            PropertyChanges{
                target: showepg
                anchors.top: homebutton.bottom
                anchors.right: satcableboxremote.right
                visible:true
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
