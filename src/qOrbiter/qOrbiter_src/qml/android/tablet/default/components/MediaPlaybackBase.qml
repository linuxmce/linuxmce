// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:mediaPlaybackBase
    width: manager.appWidth
    height: manager.appHeight
    color: "transparent"
    Component.onCompleted: manager.setBoundStatus(true)
    property alias metadataComponent:mediaTypeMetaData.sourceComponent
    property alias scrollBarComponent:mediaScrollerTarget.sourceComponent
    property alias controlComponent: controlsLoader.sourceComponent
    property alias playlistSource:playlist.model
    property alias playlistDelegate:playlist.delegate
    Rectangle{
        id:metadataSection
        width: manager.appWidth
        height: scaleY(30)
        color: "transparent"
        anchors.verticalCenter: parent.verticalCenter

        Rectangle{
            id:opacityMask
            anchors.fill: parent
            color: "darkgrey"
            opacity: .50
        }

        PlaylistTemplateCoverFlow{
            id:playlist          
            anchors.centerIn: parent
        }

        StyledText {
            id: updating_time
            text: dceTimecode.qsCurrentTime
            fontSize:32
            anchors.left: parent.left
             anchors.bottom: metadataSection.bottom
        }

        NowPlayingImage{
            id:npImage
            anchors.centerIn: metadataSection
            anchors.horizontalCenterOffset: scaleX(-3)
        }

        Image{
            id:contextImage
            anchors.fill: parent
            source: "http"
        }

        Loader{
            id:mediaTypeMetaData
            anchors.left: npImage.right
            anchors.leftMargin: scaleX(-1)
        }

        StyledText {
            id: totalTime
            text: dceTimecode.qsTotalTime
            fontSize:32
            anchors.right: parent.right

            anchors.bottom: metadataSection.bottom
        }


    }

    Loader{
        id:mediaScrollerTarget
        anchors.top: metadataSection.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }


    Rectangle{
        id:bottomControls
        width: manager.appWidth
        height: controlsLoader.height
        color: "transparent"
        anchors.bottom: parent.bottom
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
}
