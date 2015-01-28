import QtQuick 2.2
import "../components"

StyledScreen {
    navigation: "FloorplanNav.qml"
    property alias floorplan: climateFloorplan

    TabletFloorplanLayout{
        id: climateFloorplan
        Component.onCompleted: {
            hideInfoPanel();
            floorplan_devices.clearAllSelections()
        }
    }
}
