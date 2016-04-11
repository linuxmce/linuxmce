import QtQuick 1.1
Item{
    id:videoDelegate  
    clip:true
    height: currentCellHeight
    width:currentCellWidth
    Rectangle{
        anchors.fill: parent
        opacity: path !=="" ? .65 : 85
        color:trap.pressed ? "darkgreen" : "black"

    }
    Image{
        id:imdbImg
        source:path !=="" ? "http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+path : ""
        anchors.fill: parent
        anchors.margins: 10
        smooth: true
        asynchronous: true
    }


    StyledText{
    //   text:multi_view_list.state
      text: name
     //text:manager.q_attributeType_sort
        anchors.verticalCenter: parent.verticalCenter
        wrapMode: Text.WrapAnywhere
        fontSize: 36
        visible:path==="" ? true : false
        isBold: true
        color: "White"
        anchors{
            left:parent.left
            right:parent.right
        }
    }

    MouseArea{
        id:trap
        anchors.fill: parent
        onReleased: {
            if(name==="back (..)"){
                manager.goBackGrid()
            } else {
                manager.addRestoreIndex(model.index);
                manager.setStringParam(4, id);
            }
        }
    }
}
