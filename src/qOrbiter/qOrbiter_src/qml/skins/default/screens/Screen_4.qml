import QtQuick 2.2
import "../components"
import "../."


StyledScreen {
    navigation: "FloorplanNav.qml"
    property alias floorplan: climateFloorplan
    onScreenOpen: floorplan_devices.clearAllSelections()
    onScreenClosing: floorplan_devices.setCurrentFloorPlanType(-1)
    TabletFloorplanLayout{
        id: climateFloorplan
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        Component.onCompleted: {
            floorplan_devices.setCurrentFloorPlanType(3)
            manager.getFloorplanDevices(3)
        }
    }
}
