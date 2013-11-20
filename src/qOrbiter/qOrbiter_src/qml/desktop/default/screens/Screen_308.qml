import QtQuick 1.1
import "../components"
MediaPlaybackBase {
   id:networkMediaPlayerScreen
   metadataComponent: AudioMetaData{ }
   scrollBarComponent: MediaScrollBar{}
   controlComponent: VideoControls{}
}
