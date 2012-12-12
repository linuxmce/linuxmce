import QtQuick 1.0
MouseArea{
    property int mode
    anchors.fill: parent
    onClicked: {
        manager.toggleDisplay(mode)
    }
}
