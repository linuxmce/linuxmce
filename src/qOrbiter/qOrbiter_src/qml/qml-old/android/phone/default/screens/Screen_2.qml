import QtQuick 1.0
import "../components"
StyledScreen{
    id:styledScreen

    PhoneFloorplanLayout{
        id:lights
        anchors.fill: parent
        Component.onCompleted: {
            floorplan_devices.clearAllSelections()
            setNavigation("FloorplanNav.qml")
        }
    }
}


