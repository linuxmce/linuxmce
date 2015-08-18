import QtQuick 2.2
import org.linuxmce.enums 1.0
import QtGraphicalEffects 1.0
import "../."
Item{
    id:videoDelegate
    clip:true
    height: currentCellHeight
    width:currentCellWidth
    focus:true
    property bool active: activeFocus ||trap.pressed

    Rectangle{
        id:bgExtent
        anchors.fill: parent
        anchors.margins: 5
        opacity: path !=="" ? .70 : .85
        color:trap.pressed ? Style.appcolor_background_medium : "black"
        border.color: "white"
        border.width: active ? 2 : 0
    }
    Image{
        id:imdbImg
        fillMode: Image.PreserveAspectCrop
        source:path !=="" ? "http://"+manager.m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+path : ""
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
        text: name
        anchors{
            left:bgExtent.left
            right:bgExtent.right
            bottom:bgExtent.bottom
        }
        wrapMode: Text.Wrap
        fontSize: Style.appFontSize_description
        visible:true //path==="" ? true : false      
        color: "White"
        height: titleBlock.lineCount * 50
    }

    Keys.onEnterPressed: {
        if(name==="back (..)"){
            manager.goBackGrid()
        } else {
            manager.addRestoreIndex(model.index);
            manager.setStringParam(4, id);
        }
    }

    Keys.onReturnPressed: {
        if(name==="back (..)"){
            manager.goBackGrid()
        } else {
            manager.addRestoreIndex(model.index);
            manager.setStringParam(4, id);
        }

    }
    Keys.onEscapePressed: {
        manager.goBackGrid()
    }
    Keys.onBackPressed: {
        manager.goBackGrid()
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
