import QtQuick 2.3
import "../components"
import "../"
import org.linuxmce.screens 1.0
Item{
    id:genericListContainer
    width: Style.listViewWidth_medium
    property alias model:view.model
    property string label:"Ipsum"
    property alias delegate:view.delegate

    Rectangle{
        id:hdr
        anchors{
            left: parent.left
            right:parent.right
            top:parent.top
        }
        height: Style.appNavigation_panelHeight
        color:Style.appcolor_background_medium
    }
    StyledText{
        id:label
        anchors.centerIn: hdr
        text:genericListContainer.label
        font.pointSize: Style.appFontSize_title
        color:"white"
    }
    ListView{
        id:view
        clip:true
        anchors{
            left:parent.left
            top:hdr.bottom
            bottom:parent.bottom
            right:parent.right
        }
    }
    
}
