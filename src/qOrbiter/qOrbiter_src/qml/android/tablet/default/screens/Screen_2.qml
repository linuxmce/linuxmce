import QtQuick 1.1
import "../components"
StyledScreen{

    TabletFloorplanLayout{
        id: lightsFloorplan
        Component.onCompleted: {
            hideInfoPanel();
            setNavigation("FloorplanNav.qml");floorplan_devices.clearAllSelections()
        }

    }
}


