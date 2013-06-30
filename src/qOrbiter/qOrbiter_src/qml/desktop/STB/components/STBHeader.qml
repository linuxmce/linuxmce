import QtQuick 1.1
import Qt.labs.shaders 1.0
import "../../lib/components"


Item{
    id:hdr
    anchors.top: parent.top
    width: qmlroot.width
    height:scaleY(8)

    Rectangle{
        anchors.fill: parent
        color:appStyle.lightHighlightColor
        border.width: hdr.activeFocus ? 2 : 0
        focus:false
        border.color:"red"
        opacity: hdr.activeFocus ? 1 : .65
    }

    Row{
        height: parent.height
        width: parent.width
        spacing: 5
        Text {
            id: name
            text: "Orbiter "+ manager.m_dwPK_Device + " is connected to "+ manager.m_ipAddress
        }
    }


    Keys.onTabPressed:swapFocus()
    onActiveFocusChanged: {
        if(activeFocus)
            console.log("Header gained focus")
        else
            console.log("Header lost focus")
    }
}
