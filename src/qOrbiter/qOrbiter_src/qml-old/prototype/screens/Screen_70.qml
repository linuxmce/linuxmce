import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
MediaPlaybackBase{
    id:storedVideoRemote
    metadataComponent: ProgramData{id:textRect}
    scrollBarComponent: MediaScrollBar{}
    controlComponent: VideoControls{}

    VideoButtons{
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
    }
}



//Rectangle {

//    // property alias synText:
//    id: storedvideoremote
//    height: manager.appHeight
//    width: manager.appWidth
//    radius: 0
//    opacity: 1
//    color: "transparent"
//    z:5

//   Rectangle{
//       id:header
//       width: parent.width
//       height: scaleY(8)
//       color: "transparent"
//       anchors.top: parent.top
//       Image {
//           id: headerbg
//           source: "../img/ui3/header.png"
//           anchors.centerIn: info
//           height: scaleY(8)
//           width: scaleX(99)

//       }
//       Row{
//           id:info
//           spacing:10
//           width: childrenRect.width
//           height: childrenRect.height
//           anchors.centerIn: parent

//           Label{
//               text:"Stored Video"
//           }

//           Label{
//               text:dceTimecode.qsCurrentTime + qsTr(" of ") + dceTimecode.qsTotalTime
//           }

//           Label{
//               text: qsTr("Speed: ") + dceTimecode.playbackSpeed
//           }

//       }

//   }
//    Column{
//        anchors.right: storedvideoremote.right
//        anchors.rightMargin: scaleX(1)
//        anchors.top: header.bottom
//        Remote_lighting_controls{ id: remote_lighting_controls1; }
//        Remote_Audio_controls{ id: remote1; }
//    }
//    Connections{
//        target:dcenowplaying
//        onImageChanged: {
//            nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp;
//            console.log("now playing changed")
//        }
//    }


//    Row{
//        id:mainrow
//        height: scaleY(60)
//        width: scaleX(95)
//        spacing: scaleX(1)
//        anchors.top:header.bottom
//        anchors.horizontalCenter: parent.horizontalCenter

//        NonEPGPlaylist{
//            id:playlist
//            anchors.left: parent.left
//            anchors.leftMargin: scaleX(1)
//        }

//        Column
//        {
//            id:metadata
//            height: scaleY(50)
//            width: scaleX(55)
//            spacing: scaleY(1)
//            Image {
//                id: nowplayingimage
//                anchors.top:gradientheader.bottom
//                anchors.topMargin: scaleY(.5)
////                width: dcenowplaying.aspect=="wide"? scaleX(30) : scaleX(20)
////                height:dcenowplaying.aspect=="wide"? scaleY(30) : scaleY(50)
//                height: scaleY(25)
//                fillMode: Image.PreserveAspectFit
//                width: scaleX(55)
//                source: ""
//                Component.onCompleted: manager.setBoundStatus(true)
//            }

//            ProgramData {
//                id: textrect
//            }
//        }
//    }
//    Image {
//        id: transitPreview
//        source: ""
//        width: dcenowplaying.aspect=="wide"? scaleX(10) : scaleX(7)
//        height:dcenowplaying.aspect=="wide"? scaleY(10) : scaleY(17)
//        anchors.bottom: media_transit.top
//        anchors.horizontalCenter: parent.horizontalCenter
//        visible: dceTimecode.playbackSpeed !=1000 ? true : false
//        Connections{
//            target: manager
//            onMediaScreenShotReady: transitPreview.source="image://listprovider/screenshot/"+securityvideo.timestamp
//        }
//        Timer{
//            id:previewTimer
//            interval: 1000
//            repeat: true
//            running: dceTimecode.playbackSpeed !=1000 ? true : false
//            onTriggered: manager.getVideoFrame()
//        }

//    }

//    MediaScrollBar{id:media_transit; anchors.bottom: controlrow.top; anchors.bottomMargin: 10; anchors.horizontalCenter: controlrow.horizontalCenter}

//    Row{
//        id:controlrow
//        anchors.top: mainrow.bottom
//        anchors.topMargin: scaleY(2)
//        height: childrenRect.height
//        width: childrenRect.width
//        anchors.horizontalCenter: parent.horizontalCenter
//        z:0
//        Column{
//            height: childrenRect.height
//            width: childrenRect.width
//            spacing: scaleY(1.5)

//            VideoControls {
//                id: videocontrols1
//                anchors.horizontalCenter: parent.horizontalCenter
//            }
//            VideoButtons {
//                id:videoButtons
//            }
//        }

//    }

//}


