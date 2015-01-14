import QtQuick 2.2
import "../components"
StyledScreen{
    navigation: "FloorplanNav.qml"

    TabletFloorplanLayout{
        anchors.fill: parent
        id: lightsFloorplan
        Component.onCompleted: {
            hideInfoPanel();
            floorplan_devices.clearAllSelections()
        }

    }
}


