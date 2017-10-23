import QtQuick 2.2
import "../components"
import "../"
import org.linuxmce.screens 1.0
Item{
    id:listViewLargeDelegate
    height: appStyle.scaleY(8)
    width: parent.width
    property string label
    Rectangle{
        anchors.fill: parent
        color:appStyle.appcolor_background_list
        opacity: appStyle.appList_opacity
    }    

    StyledButton{
        buttonText:label
    }
}
