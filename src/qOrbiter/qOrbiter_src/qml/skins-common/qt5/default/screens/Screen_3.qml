import QtQuick 2.2
import "../components"

StyledScreen {
    navigation: "FloorplanNav.qml"

    TabletFloorplanLayout{
        id: mediaFloorplan
        Component.onCompleted: {
            hideInfoPanel();
            setNavigation();
            floorplan_devices.clearAllSelections()
        }
    }
}
