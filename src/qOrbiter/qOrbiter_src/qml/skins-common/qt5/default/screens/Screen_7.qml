import QtQuick 2.2
import "../components"

StyledScreen {
    navigation: "FloorplanNav.qml"

    TabletFloorplanLayout{
        id: telecom
        Component.onCompleted: {
            hideInfoPanel();
            setNavigation();
            floorplan_devices.clearAllSelections()
        }

    }
}
