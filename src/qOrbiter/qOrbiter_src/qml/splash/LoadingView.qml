import QtQuick 2.2
import "."
Item {
    anchors.fill: parent

    Text {
        id: loadingLael
        width:parent.width*.50
        height: parent.height*.75
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        text: qsTr("LinuxMCE is Loading device %1, Please Wait").arg(manager.iPK_Device)
        color:"white"
        font.pixelSize: theme.fontSize_large
        anchors.centerIn: parent
    }


}

