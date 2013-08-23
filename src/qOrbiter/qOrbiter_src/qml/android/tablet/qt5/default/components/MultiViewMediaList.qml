import QtQuick 2.0
import "../components"

Item{
    id:multi_view_list
    height: parent.height
    width: parent.width
    property int itemBuffer:25
    ListView{
        id:media_list
        anchors.fill: parent
        visible: current_view_type===1
        spacing:scaleY(2)
        model:dataModel
        clip:true

        delegate: Item{
            id:mediaViewDelegate
            height: scaleY(25)
            width: parent.width

            Image{
                id:img
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
                fontWeight: Font.Normal
                width: parent.width - img.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
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
        model:dataModel
        visible:current_view_type===2
    }
    
    PathView{
        id:media_path
        anchors.fill: parent
        model:dataModel
        visible:current_view_type===3
    }
}
