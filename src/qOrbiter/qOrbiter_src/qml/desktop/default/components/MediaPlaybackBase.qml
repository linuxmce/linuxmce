// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "../../lib/effects"

Rectangle {
    id:mediaPlaybackBase
    height: parent.height
    width: manager.appWidth
    color: "transparent"

    Component.onCompleted: manager.setBoundStatus(true)
    property alias metadataComponent:mediaTypeMetaData.sourceComponent
    property alias scrollBarComponent:mediaScrollerTarget.sourceComponent
    property alias controlComponent: controlsLoader.sourceComponent
    property alias playlistSource:playlist.model
    property alias playlistDelegate:playlist.delegate

    property bool enableScrollbar:true


    Rectangle{
        id:metadataSection
        width: parent.width
        height: scaleY(45)
        color: "transparent"

        anchors.verticalCenter: parent.verticalCenter
        clip:false
        DropShadow{
            id:headerDrop
            sourceItem:fill
            height: fill.height
            width: fill.width
           // anchors.verticalCenter: opacityMask.verticalCenter
            distance:4
            color:"black"
            blur:1
            opacity: .25
        }
        Rectangle{
            anchors.fill: parent
            color:"black"
            border.color: "black"
            border.width: 2
            Image {
                id: fill
                source: manager.imagePath+"ui3/appBackground.png"
                anchors.fill:parent
                opacity: .85
            }

        }

        PlaylistTemplateCoverFlow{
            id:playlist
            anchors.centerIn: parent
            visible:false
        }

        NonEPGPlaylist{
            anchors.left: parent.left
            anchors.top: parent.top
        }



        NowPlayingImage{
            id:npImage
            anchors.centerIn: metadataSection
            anchors.horizontalCenterOffset: scaleX(-10)
        }

        Loader{
            id:mediaTypeMetaData
            anchors.left: npImage.right
            anchors.leftMargin: scaleX(-1)
            height: parent.height*.95
        }

        DropShadow{
            id:metadata_drop
            sourceItem:mediaTypeMetaData
            height: mediaTypeMetaData.height
            width: mediaTypeMetaData.width
            distance:2
            color:"black"
            anchors.left: mediaTypeMetaData.left
        }


    }

    StyledText {
        id: updating_time
        text: dceTimecode.qsCurrentTime
        fontSize:32
        anchors.left: parent.left
        anchors.top: metadataSection.bottom
    }

    StyledText {
        id: totalTime
        text: dceTimecode.qsTotalTime
        fontSize:32
        anchors.right: parent.right
        anchors.top: metadataSection.bottom
    }

    Loader{
        id:mediaScrollerTarget
        anchors.top: metadataSection.bottom
        anchors.topMargin: scaleY(1)
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
