import QtQuick 2.2

Rectangle {

    id: np_box
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

   Component.onCompleted: manager.BindMediaRemote(true)

   Image {
      id: nowplayingimage
     anchors.fill: np_box
      fillMode: Image.PreserveAspectFit
  height:scaleX(50)
width:scaleX(50)
      source: ""
     
  }
   Text {
       id: np
       text:dceTimecode.qsCurrentTime
       font.pixelSize: scaleY(2.5)
       anchors.bottom: np_box.bottom
       wrapMode: Text.WrapAtWordBoundaryOrAnywhere
       anchors.horizontalCenter: np_box.horizontalCenter
       color: "white"
       visible:  dceTimecode.qsCurrentTime ==="0" ? false: true
   }

   /*
   Image{
        id:np_image
        fillMode: Image.PreserveAspectCrop
        source:"../img/transparencymask.png"
        height:  manager.appHeight
        width: manager.appWidth
        anchors.centerIn: np_box
        asynchronous: true
        opacity: .25

    }

    Text {
        id: np_label
        text: qsTr("Now Playing ")
        font.pixelSize: scaleY(3)
        anchors.top: np_box.top
        font.bold: true
        anchors.topMargin: 15
        anchors.left: np_box.left
        anchors.leftMargin: 20
        color: "Green"
    }

    Text{
        text: dcenowplaying.qs_mainTitle
        font.pixelSize: scaleY(3)
        anchors.top: np_label.top

        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color: "silver"
        width: np_box.width - 40
        anchors.left: np_label.right
        font.bold: true
    }
*/



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
