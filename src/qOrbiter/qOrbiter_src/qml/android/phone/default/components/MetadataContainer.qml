import QtQuick 1.1
import "../../../../skins-common/lib/handlers"
Item{
    id:metadataContainer
    height: parent.height
    width: parent.width
    focus:true
    Rectangle{
        anchors.fill: parent
        color: "black"
        opacity: .55
    }
    
    property int trigger: x+width/2
    
    Keys.onReleased: {
        switch(event.key){
        case Qt.Key_VolumeDown:
            manager.adjustVolume(-2)
            mediaPlaybackBase.state = "volume"
            break;
            
        case Qt.Key_VolumeUp:
            manager.adjustVolume(+2)
            mediaPlaybackBase.state = "volume"
            break;
            
        default:
            console.log("Unknown Key ==> "+ event.key)
        }
        event.accepted = true
    }
    
    onXChanged:{
        if (x >  width*.65 ){
            mediaPlaybackBase.state = "playlist"
        }else if ( x+width < ( width *.20) ){
            console.log("Advanced Mode")
        }
    }
    
    NowPlayingRemoteImage {
        id: nowplayingimage
    }
    
    Loader{
        id:mediaMetadata
        source:"Metadata_"+manager.i_current_mediaType+".qml"
    }
    
    
    MouseArea{
        id:md_drag
        anchors.fill: parent
        drag.target: metadataContainer
        drag.axis: Drag.XAxis
        drag.filterChildren: true
        onReleased: if(enabled){
                        mediaPlaybackBase.state="controls"
                    }
        
    }
    
    MediaScrollBar{
        id:media_scroller;
        anchors.bottom: controlsTarget.top
        anchors.horizontalCenter: metadataContainer.horizontalCenter
        anchors.topMargin: scaleY(2)
    }

    Loader{
        id:controlsTarget
        anchors.bottom: metadataContainer.bottom
        sourceComponent:AudioRemote{}
    }

    states: [
        State {
            name: "nonposter"
            AnchorChanges{
                target:nowplayingimage
                anchors{
                    top:parent.top
                    left:parent.left
                }
            }
            AnchorChanges{
                target: mediaMetadata
                anchors.top: nowplayingimage.bottom
                anchors.horizontalCenter: nowplayingimage.horizontalCenter
            }

        },
        State {
            name: "poster"
            AnchorChanges{
                target:nowplayingimage
                anchors{
                    top:parent.top
                    left:parent.left
                }
            }
            AnchorChanges{
                target: mediaMetadata
                anchors.top: nowplayingimage.bottom
                anchors.horizontalCenter: nowplayingimage.horizontalCenter
            }
        }
    ]
    
}
