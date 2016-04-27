import QtQuick 1.1
import "../../../../skins-common/lib/handlers"
Item{
    id:metadataContainer
    focus:true
  //  state: dcenowplaying.aspect !=="poster" ? "nonposter": "poster"
    property int trigger: x+width/2
    Keys.onReleased: {
        switch(event.key){
        case Qt.Key_VolumeDown:
            manager.adjustVolume(-2)
            mediaPlaybackBase.state = "volume"
            event.accepted = true
            break;

        case Qt.Key_VolumeUp:
            manager.adjustVolume(+2)
            mediaPlaybackBase.state = "volume"
            event.accepted = true
            break;

        default:
            console.log("Uncaught Key ==> "+ event.key)
        }

    }

    Rectangle{
        anchors.fill: parent
        color: "black"
        opacity: .55
    }

    GenericMetaData{
        id:mediaMetadata
        width: parent.width /2
        anchors{
            bottom:media_scroller.top
            left:metadataContainer.left
            right:metadataContainer.right
            top:metadataContainer.top
        }
    }

ControlDiamond{

    anchors{
        right:parent.right
        bottom:media_scroller.top
    }
}


    
    MediaScrollBar{
        id:media_scroller;
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: metadataContainer.horizontalCenter
        anchors.topMargin: scaleY(2)
    }

//    GenericFooterRemote{
//        id:controlsTarget
//        anchors.bottom: parent.bottom
//    }

//    states: [
//        State {
//            name: "nonposter"

//            AnchorChanges{
//                target: mediaMetadata
//                anchors.top: nowplayingimage.bottom
//                anchors.left: parent.left
//            }

//        },
//        State {
//            name: "poster"

//            AnchorChanges{
//                target: mediaMetadata
//                anchors.top: manager.isProfile ? nowplayingimage.bottom : parent.top
//                anchors.left: manager.isProfile? parent.left : nowplayingimage.right
//            }
//        }
//    ]
    
}
