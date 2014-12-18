import QtQuick 2.2
import "../components"

TabletFloorplanLayout{
    id: telecom
    Component.onCompleted: {
        hideInfoPanel();
        setNavigation("FloorplanNav.qml");
        floorplan_devices.clearAllSelections()
    }

}
