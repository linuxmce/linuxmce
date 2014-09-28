import QtQuick 1.1
import "../components"
TabletFloorplanLayout{
    id: mediaFloorplan
    Component.onCompleted: {
        hideInfoPanel();
        setNavigation("FloorplanNav.qml");
        floorplan_devices.clearAllSelections()
    }

}
