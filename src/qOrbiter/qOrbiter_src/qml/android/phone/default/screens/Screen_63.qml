import QtQuick 1.0
import "../components"

Rectangle {

    // property alias synText:
    id: satcableboxremote
    height: 480
    width: 320
    state: ""

    Column{
        id:textcol
        x: 61
        y: 0
        spacing: scaleY(1.5)
        width: nowplayingimage.width
        height: childrenRect.height
        anchors.horizontalCenterOffset: 17
        anchors.topMargin: 0
        anchors.top: pvrRemote.top
        anchors.horizontalCenter: parent.horizontalCenter
        NowPlayingBox{id:np_box

        }

        Text {
            id: generaltitle
            width: parent.width
            text:  dcenowplaying.qs_mainTitle
            font.family: "Droid Sans"
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            font.bold: true
            smooth: true
            font.pixelSize: scaleY(3)
            visible:  dcenowplaying.qs_mainTitle =="" ? false: true
        }

        Text {
            id: programtext
            width: parent.width
            text: qsTr("Program :") + dcenowplaying.tvProgram
            font.family: "Droid Sans"
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            //  font.bold: true
            smooth: true
            font.pixelSize: scaleY(1.5)
            visible:  dcenowplaying.tvProgram =="" ? false: true
        }

        Text {
            id: episode
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Episode: ") + dcenowplaying.episode
            font.family: "Droid Sans"
            //  font.bold: true
            smooth: true
            font.pixelSize: scaleY(1.5)
            visible:  dcenowplaying.episode =="" ? false: true
        }

        Text {
            id: genre
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Genre(s): ") + dcenowplaying.genre
            font.family: "Droid Sans"
            // font.bold: true
            smooth: true
            font.pixelSize: scaleY(1.5)
            visible:  dcenowplaying.genre =="" ? false: true
            MouseArea{
                anchors.fill: genre
                hoverEnabled: true
                onEntered: { genre.elide = "ElideNone" ; }
                onExited: {genre.elide = "ElideRight"; }
            }
        }


        Text {
            id: starring
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Perfomers: ") + dcenowplaying.performerlist
            font.family: "Droid Sans"
            //  font.bold: true
            smooth: true
            font.pixelSize: scaleY(2)
            elide: "ElideRight"
            visible:  dcenowplaying.performerlist =="" ? false: true

            MouseArea{
                anchors.fill: starring
                hoverEnabled: true
                onEntered: { starring.elide = "ElideNone" ; }
                onExited: {starring.elide = "ElideRight"; }
            }
        }

        Text {
            id: synopsis
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text: qsTr("Synopsis: ") + dcenowplaying.synop
            font.family: "Droid Sans"
            font.bold: true
            smooth: true
            font.pixelSize: scaleY(1.5)
            elide: "ElideRight"
            visible:  dcenowplaying.synop =="" ? false: true

            MouseArea{
                anchors.fill: synopsis
                hoverEnabled: true
                onEntered: { synopsis.elide = "ElideNone" ; }
                onExited: {synopsis.elide = "ElideRight"; }
            }
        }
    }
    color: "transparent"

    //main 'now playing rect containing all the other items
    // Remote_lighting_controls{ id: remote_lighting_controls1; x: 344; y: 351; width: 93; height: 219; anchors.topMargin: 179;anchors.top: nowplayingbox1.baseline}
    // Remote_Audio_controls{ id: remote1; x: 200; y: 351; anchors.rightMargin: 71; z: 45; anchors.right: remote_lighting_controls1.left}
    HaControls{ x: 8; y: 56}

    AudioRemote {
        id: audioremote1
        x: 11
        y: 339
    }
    HomeButton{ x: 8; y: 5; anchors.leftMargin: 275; anchors.topMargin: 5;anchors.left: storedAudioRemote.left; anchors.top:parent.top}

    RemoteNumberPad {
        id: remotenumberpad1
        x: 0
        y: 0
        opacity: 0
    }

    ButtonSq {
        id: buttonsq1
        x: 85
        y: 306
        width: 55
        height: 55
        buttontextfontsize: 28.600
        buttontext: "#"
        MouseArea{
            smooth: true
            anchors.fill: parent
            onClicked: state="NUMBERS"
        }
    }

    ButtonSq {
        id: buttonsq2
        x: 196
        y: 294
        opacity: 0
        MouseArea{
            anchors.fill:parent
            onClicked: satcableboxremote.states=""
        }
    }
    states: [
        State {
            name: "NUMBERS"

            PropertyChanges {
                target: textcol
                visible: false
            }

            PropertyChanges {
                target: remotenumberpad1
                x: 70
                y: 0
                opacity: 1
            }

            PropertyChanges {
                target: buttonsq1
                visible: false
            }

            PropertyChanges {
                target: buttonsq2
                x: 265
                y: 5
                width: 55
                height: 55
                buttontextitalic: false
                buttontextfontsize: 9.600
                buttontextbold: true
                buttontext: "Display"
                opacity: 1
            }
        }
    ]


}
