import QtQuick 1.0


MouseArea{
    property int speed
    anchors.fill: parent                
    onClicked: manager.setPlaybackSpeed(speed)
}
