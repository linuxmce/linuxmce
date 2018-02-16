import QtQuick 2.2
import org.linuxmce.enums 1.0
import "../components"
import "../."

Item{
    id:hdr
    property string label:"ipsum"
    property double headerOpacity:1.0
    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
    }
    height: appStyle.appNavigation_panelHeight
    Rectangle{
        anchors.fill: parent
        color:appStyle.appcolor_background_medium
        opacity: headerOpacity
    }
    StyledText{
        anchors.centerIn: parent
        text:label
    }
}
