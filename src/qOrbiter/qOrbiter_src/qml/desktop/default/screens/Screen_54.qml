import QtQuick 1.0
import Qt.labs.shaders 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../effects"

MediaPlaybackBase{
    id:storedAudioRemote
    metadataComponent: AudioMetaData{ }
    scrollBarComponent: MediaScrollBar{}
    controlComponent: VideoControls{}
    playlistSource:mediaplaylist
    playlistDelegate: Rectangle{
        height: 10
        width: 10
    }
    AudioButtons{
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
    }
}

//Rectangle {
//    id: storedaudioremote

//    Text {
//        id: messages
//        text: manager.mediaResponse
//        font.pixelSize: scaleY(4)
//        anchors.bottom: parent.bottom
//    }

//    Connections{
//        target:dcenowplaying
//        onImageChanged:nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
//    }
// Component.onCompleted: manager.setBoundStatus(true)

//    height: manager.appHeight
//    width: manager.appWidth
//    color: "transparent"

//    Rectangle{
//        id:filler
//        color: "lightgrey"
//        anchors.fill:parent
//        opacity: .15

//    }

//    Text {
//        id: bignowplaying
//        text: dcenowplaying.qs_mainTitle
//        anchors.centerIn: parent
//        font.pixelSize: scaleY(5)
//        opacity: .5
//       font.bold: true

//    }

//    //main 'now playing rect containing all the other items



//    Column{
//        anchors.right: storedaudioremote.right
//        anchors.rightMargin: scaleX(1)
//        Remote_lighting_controls{ id: remote_lighting_controls1; }
//        Remote_Audio_controls{ id: remote1; }
//    }

//    Row{
//        id:headerrow
//        height:childrenRect.height
//        width: parent.width-1
//        anchors.top: parent.top
//        anchors.topMargin: scaleY(1)

//        Text {
//            id: timecode
//            height:scaleY(2.15)
//            text: qsTr("Speed: ") + dceTimecode.playbackSpeed +" || " +dceTimecode.qsCurrentTime + qsTr(" of ") + dceTimecode.qsTotalTime
//            font.family: "Droid Sans"
//            font.pixelSize: scaleY(2.15)
//            anchors.verticalCenter: parent.verticalCenter
//            anchors.horizontalCenter: parent.horizontalCenter
//            font.bold: true
//        }
//    }

//    Row{
//        id:mainrow
//        height: childrenRect.height
//        width: scaleX(95)
//        spacing: scaleX(.5)
//        anchors.top:headerrow.bottom
//        anchors.topMargin: scaleY(2)
//        anchors.horizontalCenter: parent.horizontalCenter

//        NonEPGPlaylist{ id:playlist; visible: false}

//        Row {
//            id:metarow
//            width: childrenRect.width
//            height: childrenRect.height

//            spacing: scaleX(.25)
//            NowPlayingImage {
//                id: imageholder
//            }

//            AudioMetaData {
//                id: textrect
//            }
//        }
//    }


//    MediaScrollBar{id:media_transit; anchors.bottom: controlrow.top; anchors.horizontalCenter: controlrow.horizontalCenter; anchors.bottomMargin: scaleY(2)}
//    Row{
//        id:controlrow
//        anchors.top: mainrow.bottom
//        anchors.topMargin: scaleY(5)
//        height: controlcol.height
//        width: childrenRect.width
//        anchors.horizontalCenter: parent.horizontalCenter
//        Column{
//            id:controlcol
//            height: childrenRect.height
//            width: childrenRect.width
//            spacing: scaleY(1)
//            VideoControls {
//                id: videocontrols1
//            }
//            AudioButtons {
//            }
//        }
//    }



//}

