import QtQuick 2.2
import "."
Item {
    anchors.fill: parent


    Text {
        id: pres
        text: orbiterWindow.b_connectionPresent
        color:"white"
        anchors{
            top:parent.top
            horizontalCenter: parent.horizontalCenter
        }
    }

    Text {
        id: loadingLael
        width:parent.width*.50
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        text: qsTr("LinuxMCE is Loading device %1, Please Wait").arg(manager.iPK_Device)
        color:"white"
        font.pixelSize: Style.fontSize_large
        anchors.centerIn: parent
    }

    Text {
        id: name
        text: manager.dceResponse
        anchors{
            bottom:parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }


}

