import QtQuick 2.2
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


