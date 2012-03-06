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

   Image {
      id: nowplayingimage
     anchors.fill: np_box
      fillMode: Image.PreserveAspectFit
      source: "image://listprovider/updateobject/"+securityvideo.timestamp
  }

   /*
   Image{
        id:np_image
        fillMode: Image.PreserveAspectCrop
        source:"../img/transparencymask.png"
        height:  appH
        width: appW
        anchors.centerIn: np_box
        asynchronous: true
        opacity: .25

    }
    */
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
      visible:  dcenowplaying.channel ==="" ? false: true
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
        visible:  dcenowplaying.program ==="" ? false: true
        color: "white"
    }

    Text {
        id: np
        text:dcenowplaying.timecode
        font.pixelSize: scaleY(2.5)
        anchors.bottom: np_label.bottom
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        anchors.horizontalCenter: np_box.horizontalCenter
        color: "white"
        visible:  dcenowplaying.timecode ==="0" ? false: true
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
