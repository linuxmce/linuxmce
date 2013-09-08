import QtQuick 1.0
import "../../../../skins-common/lib/handlers"
Item{
    id:standard_playlist
    height:manager.appHeight
    width:manager.appWidth
    property alias model:playlist_data.model
    property alias delegate:playlist_data.delegate

    Component.onCompleted: {
        manager.getStoredPlaylist()
    }

    Connections{
        target: mediaplaylist
        onActiveItemChanged:{
            nonepgplaylistview.positionViewAtIndex(mediaplaylist.currentIndex, ListView.Beginning)
        }
    }

    MouseArea{
        anchors.fill: parent
        onReleased: media_playback_base.state="metadata"
    }
    GridView{
        id:playlist_data
        height:parent.height
        width:parent.width
        model:mediaplaylist
        anchors.centerIn: parent

        cellHeight:scaleY(22)
        cellWidth:scaleX(32)
        delegate: Item{
            width: scaleX(30)
            height:scaleY(20)
            clip:true
            property bool isPlaying:index === dcenowplaying.m_iplaylistPosition
            Rectangle{
                anchors.fill: parent
                color: isPlaying ? "green" : "black"
                opacity: .65
                border.color: "white"
                border.width: 1
            }

            StyledText{
                text:isPlaying ? qsTr("Now Playing \n" + name ) : name
                color:"white"
                fontSize: 36
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            StyledText{
                anchors.bottom: parent.bottom
                text:qsTr("#")+index
            }

            PlaylistClickedHandler{

            }
        }

    }

}
