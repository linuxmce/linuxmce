import QtQuick 1.1
Item{
    id:videoDelegate
    clip:true
    height: currentCellHeight
    width:currentCellWidth

    Rectangle{
        id:bgExtent
        anchors.fill: parent
        anchors.margins: 5

        opacity: path !=="" ? .70 : .85
        color:trap.pressed ? "darkgreen" : "black"

    }
    Image{
        id:imdbImg
        fillMode: Image.PreserveAspectFit
        source:path !=="" ? "http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+path : ""
        anchors.fill: bgExtent
        anchors.margins: 10
        smooth: true
        asynchronous: true
    }

    Rectangle{

        anchors{
            bottom:bgExtent.bottom
            left:bgExtent.left
            right:bgExtent.right
            top:titleBlock.top

        }

        color:"black"
        opacity:.65
    }

    StyledText{
        id:titleBlock
        //   text:multi_view_list.state
        text: name
        //text:manager.q_attributeType_sort
        anchors{
            left:bgExtent.left
            right:bgExtent.right
            bottom:bgExtent.bottom
        }
        wrapMode: Text.WrapAnywhere
        fontSize: 32
        visible:true //path==="" ? true : false
        isBold: false
        color: "White"
        height: titleBlock.lineCount * 50
    }

    MouseArea{
        id:trap
        anchors.fill: bgExtent
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
