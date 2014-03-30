import QtQuick 2.0
import "../components"

Item{
    id:multi_view_list
    height: parent.height
    width: parent.width
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
        anchors.fill: parent
        model:manager.getDataGridModel("MediaFile", 63)
        visible:current_view_type===1
        delegate:currentDelegate
        cellHeight: scaleY(24)
        cellWidth:scaleX(19)

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
            when:manager.q_mediaType === 4

        },
        State {
            name: "video"
            when:manager.q_mediaType === 5

        }
    ]
}
