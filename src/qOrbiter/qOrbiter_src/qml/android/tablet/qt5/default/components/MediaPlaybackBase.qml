import QtQuick 2.0
Item{
    id:media_playback_base
    height: manager.appHeight
    width: manager.appWidth

    Component.onCompleted: manager.setBoundStatus(true)
    property alias metadataComponent:mediaTypeMetaData.sourceComponent
    property alias scrollBarComponent:mediaScrollerTarget.sourceComponent
    property alias controlComponent: controlsLoader.sourceComponent
    property alias playlistSource:playlist.model
    property alias playlistDelegate:playlist.delegate
    property bool enableScrollbar:true

//    Item{
//        id: options_display
//    }

//    Loader{
//        id:mediaTypeMetaData
//    }

//    Item{
//        id:now_playing_image_container
//    }
//    Loader{
//        id:mediaScrollerTarget
//    }

//    Loader{
//        id:controlsLoader
//    }

//    PlaylistView{
//        id:playlist
//        anchors.left: parent.left

//    }

//    NowPlayingImage{
//        anchors.centerIn: parent
//    }


}
