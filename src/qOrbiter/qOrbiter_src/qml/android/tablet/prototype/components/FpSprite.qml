// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    id:sprite_root
    width: itemW
    height: itemH
    objectName:deviceNum

    property color activeColor: "grey"
    property color inactivecolor: "green"
    property double devicelevel:0.0
    property string state: "unknown"
    property string deviceName: ""
    property string deviceNum:""
    property string deviceType: ""
    property string imgUrl: ""
    property bool selected:floorplan_devices.getDeviceSelection(deviceNum)
    property int itemH:scaleY(5)
    property int itemW: scaleY(5)
    property double iconScale: 1.5

    Connections{
        target: floorplan_devices
        onDataChanged: selected = floorplan_devices.getDeviceSelection(deviceNum)
    }

    scale: iconScale
    function updateFpItem()
    {
        floorplan_devices.setDeviceSelection(deviceNum)
        sprite.border.width = selected ? 1 : 0
    }

    Image {
        id: fpDevice_image
        source: "../img/floorplanitems/"+deviceType+".png"
        sourceSize:Qt.size(scaleY(5), scaleY(5))
        anchors.centerIn: parent
        cache: false
    }



    Rectangle{
        id:sprite
        height: parent.height - 1
        width: parent.width - 1
        color: "transparent"
        radius:5
        opacity: .25
        border.color:selected ? "red" : "black"
        border.width:sprite.border.width = selected ? 4 : 2
        Component.onCompleted: PropertyAnimation {target:sprite ; property:"opacity"; to: .50; duration:1500 }
        scale: iconScale
    }

    Text {
        id: device_number
        text: deviceNum
        anchors.centerIn: parent
        font.pixelSize: scaleY(3)
        color:selected ? "pink" : "white"
    }

    Connections{
        target:floorplan_devices
        onChangePage:sprite_root.destroy()
    }

    MouseArea{
        anchors.fill: sprite_root
        onClicked: updateFpItem()
    }
}
