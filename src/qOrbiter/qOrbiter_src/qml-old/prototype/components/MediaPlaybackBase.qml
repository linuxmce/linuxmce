// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
Item {
    id:mediaPlaybackBase
    width: manager.appWidth
    height: manager.appHeight
    state:"metadata"
    Component.onCompleted: manager.setBoundStatus(true)
    property alias metadataComponent:mediaTypeMetaData.sourceComponent
    property alias scrollBarComponent:mediaScrollerTarget.sourceComponent
    property alias controlComponent: controlsLoader.sourceComponent

    property alias delegateForPlaylist:playlist.playlistDelegate

    Item{
        id:metadataSection
        width: manager.appWidth
        height: scaleY(30)

        anchors.verticalCenter: parent.verticalCenter

        Rectangle{
            id:opacityMask
            anchors.fill: parent
            color: "maroon"
            opacity: .75
        }

        //        PlaylistTemplateCoverFlow{
        //            id:playlist
        //            anchors.centerIn: parent
        //        }

        Image{
            id:contextImage
            anchors.fill: parent
            source: ""

        }

        Loader{
            id:mediaTypeMetaData
            anchors.left: npImage.right
            anchors.leftMargin: scaleX(-1)
        }

        StyledText {
            id: updating_time
            text: dceTimecode.qsCurrentTime
            fontSize:32
            anchors.right: parent.right
            anchors.top: parent.top
            color: "white"
        }

        StyledText {
            id: totalTime
            text: dceTimecode.qsTotalTime
            fontSize:32
            anchors.right: parent.right

            anchors.bottom: metadataSection.bottom
            color:"white"
        }
    }

    NowPlayingImage{
        id:npImage
        anchors.verticalCenter: parent.verticalCenter
        MouseArea{
            anchors.fill: parent
            drag.target: npImage
            drag.axis: Drag.XAxis
            drag.maximumX: scaleX(85)
            drag.minimumX: 0
            onPressed: mediaPlaybackBase.state = "imgDrag"
            onReleased: mediaPlaybackBase.state = "metadata"
        }
    }

    NonEPGPlaylist{
        id:playlist
        anchors.top: parent.top
    }

    Loader{
        id:mediaScrollerTarget
        anchors.bottom: bottomControls.top
        anchors.bottomMargin: scaleX(5)
        anchors.horizontalCenter: parent.horizontalCenter
    }


    Rectangle{
        id:bottomControls
        width: manager.appWidth
        height: 50
        anchors.bottomMargin: 20
        color: "transparent"
        anchors.bottom: mediaPlaybackBase.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        Loader{
            id:controlsLoader
            sourceComponent:VideoControls{}
            anchors.centerIn: parent
        }
    }

    Column{
        id:env_controls
        height: childrenRect.height
        width: scaleX(40)
        anchors.bottom: parent.bottom
        anchors.left: parent.left

        AudioControl{
            id:audio_controls
        }
    }

    states: [
        State {
            name: "metadata"
            PropertyChanges {
                target: metadataSection
                visible:true
            }
            AnchorChanges{
                target:npImage
                anchors.right: undefined
                anchors.left: playlist.right
            }

            AnchorChanges{
                target:playlist
                anchors.left: mediaPlaybackBase.left
            }
        },
        State {
            name: "playlistview"
            PropertyChanges {
                target: playlist
                width:scaleX(50)
            }
        },
        State {
            name: "imgDrag"
            AnchorChanges {
                target: npImage
                anchors.left: undefined
                anchors.right: undefined
            }
        }
    ]
}
