import QtQuick 2.2
import "../components"
StyledScreen {
    navigation: "FloorplanNav.qml"
    property alias floorplan: securityFloorplan
    onScreenClosing: floorplan_devices.setCurrentFloorPlanType(-1)
    TabletFloorplanLayout{
        id: securityFloorplan
        Component.onCompleted: {
            floorplan_devices.setCurrentFloorPlanType(1)
         // manager.getFloorplanDevices(1)

        }
    }
}
