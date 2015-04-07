import QtQuick 1.1
import "../components"

TabletFloorplanLayout{
    id: lightsFloorplan
    Component.onCompleted: {
        hideInfoPanel();
        setNavigation("FloorplanNav.qml");floorplan_devices.clearAllSelections()
    }

}
