import QtQuick 1.0

MouseArea{
    property string level:"10"
    anchors.fill: parent
    onClicked: manager.adjustLights(level)
}
