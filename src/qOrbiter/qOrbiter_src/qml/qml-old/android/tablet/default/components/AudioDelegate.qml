import QtQuick 1.1
Item{
    id:audioDelegate
    height: currentCellHeight
    width: currentCellWidth
    clip:true

    Rectangle{
        id:fillah
        anchors.fill: parent
        opacity: .65
        color:trap.pressed ? "darkgreen" : "black"
        anchors.margins: 10
    }

    Image{
        id:img
        source:path !=="" ? "http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+path : ""
       anchors.centerIn: fillah
        width: height
        height: fillah.height
        anchors.margins: 5
        fillMode: Image.PreserveAspectCrop
        smooth: true
        asynchronous: true
    }

    Rectangle{
        id:subFiller
        anchors{
            left:fillah.left
            right:fillah.right
            bottom:fillah.bottom
        }
        height:textBox.height
        color:"darkgreen"
        opacity: .65
    }

    StyledText{
        id:textBox
        text: name
        anchors.bottom: fillah.bottom
        color: "White"
        fontSize: cellFontSize
        isBold: true
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        width: fillah.width
        height: parent.height /3
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
