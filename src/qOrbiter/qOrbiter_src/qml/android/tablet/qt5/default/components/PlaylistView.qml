import QtQuick 2.0

Item{
    id:standard_playlist
    height:parent.height
    width:parent.width
    property alias model:playlist_data.model
    property alias delegate:playlist_data.delegate

    Connections{
        target: mediaplaylist
        onActiveItemChanged:{
            playlist_data.positionViewAtIndex(mediaplaylist.currentIndex, ListView.Beginning)
        }
    }

        MouseArea{
            anchors.fill: parent
            onReleased: media_playback_base.state="metadata"
        }
    ListView{
        id:playlist_data
        height:parent.height
        width:parent.width*.75
        model:mediaplaylist
    }


}
