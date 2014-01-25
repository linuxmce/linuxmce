import QtQuick 1.0
import "../components"

PhoneFloorplanLayout{
    id:lights
    Component.onCompleted: {
        floorplan_devices.clearAllSelections()
        setNavigation("FloorplanNav.qml")
    }
}
