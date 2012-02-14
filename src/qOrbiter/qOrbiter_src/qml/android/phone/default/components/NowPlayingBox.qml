import QtQuick 1.0

Rectangle {

    id: now_playing
    height: scaleY(40)
    width: scaleX(75)
    visible: dcenowplaying.b_mediaPlaying ? true : false
    color:"transparent"
    clip:true
    Timer{
        id:singleshot
        repeat: false
        interval: 2000
        triggeredOnStart: false
        running: true

        onTriggered: np_image.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }

    Image {
        id: np_image
        source: "image://listprovider/updateobject/"+securityvideo.timestamp
        fillMode: Image.PreserveAspectCrop
        anchors.centerIn: np_box
        height:scaleY(30)
        width:scaleX(40)

    }
    Connections
    {
        target: dcenowplaying
        onImageChanged: np_image.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }

    Image {
        id: np_box
        source: "../img/nowplayingbox.png"
       height: parent.height
       width: parent.width
        opacity: .5
    }
    /* Image {
        id: nowplayingimage
        height: style.orbiterh
        fillMode: Image.PreserveAspectCrop
        width: style.orbiterw
        anchors.fill: np_box
     //   source: "image://listprovider/updateobject/"+dcenowplaying.m_iplaylistPosition

    } */

    Text {
        id: np_label
        text: qsTr("NowPlaying")
        font.pixelSize: 12
        anchors.top: np_box.top
        anchors.topMargin: 15
        anchors.left: np_box.left
        anchors.leftMargin: 20
        color: "Green"
    }

    Text{
        text: dcenowplaying.qs_mainTitle
        font.pixelSize: 12
        anchors.top: np_label.bottom
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: "white"
        width: np_box.width - 40
        anchors.horizontalCenter: np_box.horizontalCenter
    }

    MouseArea{
        id: mousearea1
        anchors.fill: parent
        onClicked:screenchange(dcenowplaying.qs_screen)
    }

    Text {
        id: np
        text:dcenowplaying.timecode
        font.pixelSize: 12
        anchors.bottom: np_label.bottom
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        anchors.horizontalCenter: np_box.horizontalCenter
        color: "white"
    }

    states: [
        State {
            name: "livetv"

            PropertyChanges {
                target: column1
                x: 0
                y: 30
                anchors.horizontalCenterOffset: 0
                anchors.topMargin: 0
            }

            PropertyChanges {
                target: textrect
                x: -38
                y: 27
            }

            PropertyChanges {
                target: mousearea1
                x: 0
                y: 0
                width: 231
                height: 133
                anchors.topMargin: 0
                anchors.rightMargin: 0
            }

            PropertyChanges {
                target: np_box
                x: 0
                y: 0
            }
        },
        State {
            name: "storedaudio"

            PropertyChanges {
                target: column1
                x: -39
                y: 29
            }

            PropertyChanges {
                target: textrect
                x: -27
                y: 18
            }
        },
        State {
            name: "storedvideo"
        },
        State {
            name: "game"
        },
        State {
            name: "dvd"
        }
    ]


}
