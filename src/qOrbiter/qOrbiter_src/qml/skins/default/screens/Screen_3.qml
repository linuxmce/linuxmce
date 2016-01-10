import QtQuick 2.2
import "../components"
import "../."


StyledScreen {
    navigation: "FloorplanNav.qml"
    property alias floorplan: mediaFloorplan
    onScreenOpen: floorplan_devices.clearAllSelections()
    onScreenClosing: floorplan_devices.setCurrentFloorPlanType(-1)
    TabletFloorplanLayout {
        id:mediaFloorplan
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: /*manager.isProfile ? buttonRow.top : */parent.bottom
        anchors.right: /*manager.isProfile ?*/ parent.right //: buttonRow.right

        Component.onCompleted: {
            floorplan_devices.setCurrentFloorPlanType(5)
            manager.getFloorplanDevices(5)

        }

    }
}
