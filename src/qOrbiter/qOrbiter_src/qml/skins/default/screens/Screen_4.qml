import QtQuick 2.2
import "../components"

StyledScreen {
    navigation: "FloorplanNav.qml"
    property alias floorplan: climateFloorplan
  onScreenClosing: floorplan_devices.setCurrentFloorPlanType(-1)
  onScreenOpening:  floorplan_devices.clearAllSelections()
    TabletFloorplanLayout{
        id: climateFloorplan
        Component.onCompleted: {
            floorplan_devices.setCurrentFloorPlanType(4)
          //  manager.getFloorplanDevices(4)

        }
    }
}
