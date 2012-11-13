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
