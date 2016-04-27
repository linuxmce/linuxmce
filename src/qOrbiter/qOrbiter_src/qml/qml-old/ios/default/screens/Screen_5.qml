import QtQuick 2.2
import "../components"
TabletFloorplanLayout{
    id: mediaFloorplan
    Component.onCompleted: {
        hideInfoPanel();
        setNavigation("FloorplanNav.qml");
        floorplan_devices.clearAllSelections()
    }

}
