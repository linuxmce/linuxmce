import QtQuick 2.0
import "../components"

Item{
    id:multi_view_list
    height: parent.height
    width: parent.width
    property int itemBuffer:25
    Component.onCompleted: {
        media_list.model=manager.getDataGridModel("MediaFile", 63)
        media_list.positionViewAtIndex(item, ListView.Beginning)
    }

    Connections {
        target: manager.getDataGridModel("MediaFile", 63)
        onScrollToItem: {
            console.log("scroll to item : " + item); media_list.positionViewAtIndex(item, ListView.Beginning);
        }
    }
    ListView{
        id:media_list
        anchors{
            left:parent.left
            right:parent.right
            bottom:parent.bottom
            top:parent.top
        }
        visible: current_view_type===1
        spacing:scaleY(2)

        clip:true

        delegate: Item{
            id:mediaViewDelegate
            height: scaleY(25)
            width: parent.width
            clip:true

            Image{
                source:path !=="" ? "http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+path : ""
                anchors.left: parent.left
                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop
                smooth: true
                asynchronous: true
            }
            Rectangle{
                anchors.fill: parent
                opacity: .65
                color:trap.pressed ? "darkgreen" : "black"

            }

            StyledText{
                text: name
                anchors.centerIn: parent
                fontSize: 36
                color: "White"
            }

            MouseArea{
                id:trap
                anchors.fill: parent
                onReleased: manager.setStringParam(4, id);
            }
        }
    }
    
    GridView{
        id:media_grid
        anchors.fill: parent
        //  model:manager.getDataGridModel("MediaFile", 63)
        visible:current_view_type===2
    }
    
    PathView{
        id:media_path
        anchors.fill: parent
        // model:manager.getDataGridModel("MediaFile", 63)
        visible:current_view_type===3
    }
}
