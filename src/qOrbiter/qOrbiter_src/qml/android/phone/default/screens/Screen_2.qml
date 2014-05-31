import QtQuick 1.0
import "../components"
StyledScreen{

    PhoneFloorplanLayout{
        id:lights
        Component.onCompleted: {
            floorplan_devices.clearAllSelections()
            setNavigation("FloorplanNav.qml")
        }
    }
}


