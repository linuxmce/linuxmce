import QtQuick 2.2
import "../components"
StyledScreen{
screen:qsTr("Manage Drives", "Manage Drives Screen Label")
    TabletFloorplanLayout{
        id: lightsFloorplan
        Component.onCompleted: {
            hideInfoPanel();
            setNavigation("FloorplanNav.qml");floorplan_devices.clearAllSelections()
        }

    }
}
