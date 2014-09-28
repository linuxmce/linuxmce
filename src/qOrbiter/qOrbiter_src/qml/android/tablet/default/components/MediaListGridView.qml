import QtQuick 1.0
import "../components"

GridView{
    id:media_list
    anchors.fill: parent
    visible: current_view_type===1

    model: manager.getDataGridModel("MediaFile", 63)
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
            onReleased: {manager.setStringParam(4, id); depth++ }
        }
    }
}
