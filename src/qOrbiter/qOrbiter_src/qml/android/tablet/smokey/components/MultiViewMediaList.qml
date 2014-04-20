import QtQuick 1.1
import org.linuxmce.enums 1.0
import "../components"

Item{
    id:multi_view_list
    anchors{
        left:parent.left
        right:parent.right
        top:parent.top
        bottom:parent.bottom
    }

    property int currentCellHeight:20
    property int currentCellWidth:20

    property int itemBuffer:25
    clip:true

    Component{
        id:audioItem
        AudioDelegate{}
    }

    Component{
        id:videoItem
        VideoDelegate{}
    }

    property variant currentDelegate:multi_view_list.state==="video"? videoItem :audioItem
    Component.onCompleted: {
        media_grid.model=manager.getDataGridModel("MediaFile", 63)
        media_grid.positionViewAtIndex(item, ListView.Beginning)
    }

    Connections {
        target: manager.getDataGridModel("MediaFile", 63)
        onScrollToItem: {
            console.log("scroll to item : " + item);
            media_grid.positionViewAtIndex(item, ListView.Beginning);
        }
    }
    ListView{
        id:media_list
        anchors{
            left:parent.left
            right:parent.right
            bottom:parent.bottom
            top:parent.top
            margins: scaleX(2)
        }
        delegate:currentDelegate
        visible: current_view_type===2
        spacing:scaleY(2)
        clip:true
    }

    GridView{
        id:media_grid
        anchors{
            left:parent.left
            right:parent.right
            bottom:parent.bottom
            top:parent.top
        }
        cellHeight: currentCellHeight
        cellWidth:currentCellWidth

        model:manager.getDataGridModel("MediaFile", 63)
        visible:true //current_view_type===1
        delegate:currentDelegate
    }

    PathView{
        id:media_path
        anchors.fill: parent
        // model:manager.getDataGridModel("MediaFile", 63)
        visible:current_view_type===3
    }
    states: [
        State {
            name: "audio"
            when:manager.q_mediaType == Mediatypes.STORED_AUDIO
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: scaleY(24)
                currentCellWidth:scaleX(19)
            }

        },
        State {
            name: "video"
            when:manager.q_mediaType == Mediatypes.STORED_VIDEO
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: scaleY(24)
                currentCellWidth:scaleX(19)
            }
        },
        State {
            name: "default"
            when:manager.q_mediaType != Mediatypes.STORED_VIDEO && manager.q_mediaType != Mediatypes.STORED_AUDIO
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: scaleY(24)
                currentCellWidth:scaleX(19)
            }
        },
        State {
            name: "video-default"
            extend:"video"
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: scaleY(24)
                currentCellWidth:scaleX(19)
            }
        },
        State {
            name: "tv"
            when:manager.q_subType==Subtypes.TVSHOWS
            extend:"video"
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: scaleY(20)
                currentCellWidth:scaleX(19)
            }
        },
        State {
            name: "movies"
            when: manager.q_subType==Subtypes.MOVIES
            extend:"video"
            PropertyChanges {
                target: multi_view_list
                currentCellHeight: scaleY(24)
                currentCellWidth:scaleX(16)
            }
        }
    ]

}
