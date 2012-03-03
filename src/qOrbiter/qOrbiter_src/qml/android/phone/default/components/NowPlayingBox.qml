import QtQuick 1.0

Rectangle {

    id: now_playing
    height: scaleY(40)
    width: scaleX(75)
    visible: dcenowplaying.b_mediaPlaying ? true : false
    color:"transparent"
    clip:true

   Connections
    {
        target: dcenowplaying
        onImageChanged: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }

    Image{
        id:np_image
        fillMode: Image.PreserveAspectCrop
        source:"../img/transparencymask.png"
        height:  appH
        width: appW
        anchors.centerIn: np_box
        asynchronous: true

    }
    Image {
        id: np_box
        source: "../img/nowplayingbox.png"
       height: now_playing.height
       width: now_playing.width
        opacity: .65
    }
     Image {
        id: nowplayingimage
        height: style.orbiterh
        fillMode: Image.PreserveAspectFit
        width: style.orbiterw
        anchors.fill: np_box
       source: "image://listprovider/updateobject/"+dcenowplaying.m_iplaylistPosition

    }

    Text {
        id: np_label
        text: qsTr("NowPlaying")
        font.pixelSize: scaleY(3)
        anchors.top: np_box.top
        anchors.topMargin: 15
        anchors.left: np_box.left
        anchors.leftMargin: 20
        color: "Green"
    }

    Text{
        text: dcenowplaying.qs_mainTitle
        font.pixelSize: scaleY(3)
        anchors.top: np_label.bottom
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: "silver"
        width: np_box.width - 40
        anchors.horizontalCenter: np_box.horizontalCenter
    }


    Text {
        id: np
        text:dcenowplaying.timecode
        font.pixelSize: scaleY(2.5)
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
