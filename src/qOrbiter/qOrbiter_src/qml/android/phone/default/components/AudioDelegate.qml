import QtQuick 1.1
Item{
    id:audioDelegate
    height: currentCellHeight
    width:currentCellWidth
    clip:true

    Rectangle{
        id:fillah
        anchors.fill: parent
        opacity: .65
        color:trap.pressed ? "darkgreen" : "black"
        anchors.margins: 10

    }

    Image{
        source:path !=="" ? "http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+path : ""
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        height: parent.height*.75
        anchors.margins: 5
        fillMode: Image.PreserveAspectCrop
        smooth: true
        asynchronous: true
    }

    Rectangle{
        id:subFiller
        anchors{
            left:textBox.left
            right:textBox.right
            bottom:textBox.bottom
        }
        height:textBox.height
        color:"darkgreen"
        opacity: .65
    }

    StyledText{
        id:textBox
        text: name
        anchors.bottom: parent.bottom
        fontSize: 24
        color: "White"
        isBold: true
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        width: fillah.width
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
