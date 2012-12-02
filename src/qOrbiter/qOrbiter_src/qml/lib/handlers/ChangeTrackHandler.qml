import QtQuick 1.0
import "../../lib/handlers"

MouseArea{
    property string trackDirection
    anchors.fill: parent                
    onClicked: manager.newTrack(trackDirection)
}
