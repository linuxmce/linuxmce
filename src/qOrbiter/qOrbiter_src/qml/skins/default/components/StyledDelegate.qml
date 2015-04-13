import QtQuick 2.2
import "../components"
import "../"
import org.linuxmce.screens 1.0
Item{
    id:listViewLargeDelegate
    height: Style.scaleY(8)
    width: parent.width
    property string label
    Rectangle{
        anchors.fill: parent
        color:Style.appcolor_background_list
        opacity: Style.appList_opacity
    }    

    StyledButton{
        buttonText:label
    }
}
