import QtQuick 1.1

import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../effects"

MediaPlaybackBase{
    id:storedVideoRemote
    metadataComponent: VideoMetaData{ }
    scrollBarComponent: MediaScrollBar{}
    controlComponent: VideoControls{}

    playlistDelegate: Rectangle{
        height: scaleY(30)
        width: scaleX(18)
        color: "transparent"
        border.color: "white"
        border.width: 1
        Image {
            id: playlistimage
            fillMode: Image.PreserveAspectCrop
            source:  index === dcenowplaying.m_iplaylistPosition ? playlistimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp: ""
            anchors.fill: parent
            opacity: .5            

        }
        StyledText {
            id: position
            text: qsTr("Item #") + index

            color: "aliceblue"
            font.pixelSize: scaleY(2.25)
            font.bold: true
            anchors.bottom: parent.bottom
            opacity: .75
        }

        StyledText {
            text:  index === dcenowplaying.m_iplaylistPosition ? "Now Playing - " + name : name
            font.family: "DroidSans"
            color: "aliceblue"
            width: parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            font.pixelSize: scaleY(2.15)
            //font.bold: true

        }

        Image {
            id: overlay
            source: "../img/icons/header.png"
            anchors.fill: parent
            opacity:index === dcenowplaying.m_iplaylistPosition ? .25 :  .15
        }

        MouseArea{
            anchors.fill: parent
            onClicked: manager.changedPlaylistPosition(index)
        }
    }

}
