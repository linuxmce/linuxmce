import QtQuick 2.2
import "../components"

StyledScreen {
    navigation: "FloorplanNav.qml"
    property alias floorplan: telecomFloorplan

    TabletFloorplanLayout{
        id: telecomFloorplan
        Component.onCompleted: {
            hideInfoPanel();
            setNavigation();
            floorplan_devices.clearAllSelections()
        }

    }
}
