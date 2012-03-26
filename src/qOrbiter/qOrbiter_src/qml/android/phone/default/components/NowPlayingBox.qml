import QtQuick 1.0

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

   Component.onCompleted: dcerouter.BindMediaRemote(true)

   Image {
      id: nowplayingimage
     anchors.fill: np_box
      fillMode: Image.PreserveAspectFit
      source: ""
      opacity: 0
      onSourceChanged: PropertyAnimation { target: nowplayingimage; property: "opacity"; to: 1; duration: 1500}
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
