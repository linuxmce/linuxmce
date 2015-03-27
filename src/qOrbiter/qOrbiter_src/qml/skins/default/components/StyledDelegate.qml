import QtQuick 2.3
import "../components"
import "../"
import org.linuxmce.screens 1.0
Item{
    id:listViewLargeDelegate
    height: Style.scaleY(8)
    width: parent.width
    Rectangle{
        anchors.fill: parent
        color:Style.appcolor_background_list
    }    
}
