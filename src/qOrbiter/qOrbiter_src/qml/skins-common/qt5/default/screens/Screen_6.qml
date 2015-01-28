import QtQuick 2.2
import "../components"

StyledScreen {
    navigation: "FloorplanNav.qml"
    property alias floorplan: securityFloorplan

    TabletFloorplanLayout{
        id: securityFloorplan
        Component.onCompleted: {
            hideInfoPanel();
            floorplan_devices.clearAllSelections()
        }
    }
}
