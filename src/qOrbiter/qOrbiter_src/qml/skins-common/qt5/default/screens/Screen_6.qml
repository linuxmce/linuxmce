import QtQuick 2.2
import "../components"

StyledScreen {
    navigation: "FloorplanNav.qml"

    TabletFloorplanLayout{
        id: securityfloorplan
        Component.onCompleted: {
            hideInfoPanel();
            floorplan_devices.clearAllSelections()
        }
    }
}
