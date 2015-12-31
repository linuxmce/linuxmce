import QtQuick 1.1

Item {
    width: parent.width
    height: parent.height

    Column{
                anchors.centerIn: parent
                width: childrenRect.width
                height: childrenRect.height
                spacing:10

                Text {
                    text:"Api Level::"+system.apiLevel
                    font.pixelSize: 36
                    color: system.redStandard
                    font.capitalization: Font.SmallCaps
                    font.weight: Font.Light
                }
                Button {
                    label:"Device Name::"+system.deviceName

                    labelColor: system.purpleStandard
                    onActivated: system.updateBuildInformation()
                }
                Text {
                    text:"Device Brand::"+system.deviceBrand
                    font.pixelSize: 36
                    color: system.greenStandard
                }
                Text {
                    text:"Device Manufacturer::"+system.deviceManufacturer
                    font.pixelSize: 36
                    color: system.orangeStandard
                }
                Text {
                    text:"Mount Status::"+system.mountStatus
                    font.pixelSize: 36
                    color: system.orangeStandard
                }
                Button {
                    label:"Mount Location::"+system.externalStorageLocation
                    labelColor: system.orangeStandard
                    onActivated: system.updateExternalStorageLocation()
                }
            }
}
