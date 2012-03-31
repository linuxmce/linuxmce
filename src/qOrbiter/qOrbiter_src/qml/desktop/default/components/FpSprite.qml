// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    id:sprite_root
    width: itemW
    height: itemH

    property color activeColor: "grey"
    property color inactivecolor: "green"
    property double devicelevel:0.0
    property string state: "unknown"
    property string deviceName: ""
    property string deviceNum:""
    property string deviceType: ""
    property string imgUrl: ""
    property bool selected: false
    property int itemH:scaleY(3)
    property int itemW: scaleY(3)
    property double iconScale: 1.5

    scale: iconScale
    function updateFpItem()
    {
        selected = !selected
        sprite.border.width = selected ? 1 : 0
    }

    Rectangle{
        id:sprite
        height: parent.height - 1
        width: parent.width - 1
        color: selected ? inactivecolor: activeColor
        opacity: 0
        border.color: "white"
        border.width:sprite.border.width = selected ? 1 : 0
        Component.onCompleted: PropertyAnimation {target:sprite ; property:"opacity"; to: 1; duration:1500 }
        scale: iconScale
    }
    Text {
        id: device_number
        text: deviceNum
        anchors.centerIn: parent
        font.pixelSize: scaleY(2)
    }

    Image {
        id: fpDevice_image
        source: ""
    }

    Connections{
        target:floorplan_devices
        onChangePage:sprite_root.destroy()
    }

    MouseArea{
        anchors.fill: parent
        onClicked: updateFpItem()
    }
}
