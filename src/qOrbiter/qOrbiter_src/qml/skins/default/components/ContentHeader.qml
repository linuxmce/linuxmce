import QtQuick 2.2
import org.linuxmce.enums 1.0
import "../components"
import "../."

Item{
    id:hdr
    anchors{
        top:parent.top
        left:parent.left
        right:parent.resources
    }
    height: Style.appNavigation_panelHeight
}
