// import QtQuick 1.1 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2

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
    property int itemH:scaleY(3)
    property int itemW: scaleY(3)
    property double iconScale: 1.5
    onSelectedChanged: {
        console.log(deviceNum+" changed to "+selected)
        sprite.border.width = selected ? 3 : 0
    }

    Connections{
        target: floorplan_devices
        onSelectedDevicesChanged: {selected = floorplan_devices.getDeviceSelection(deviceNum)}
    }

    scale: iconScale
    function updateFpItem()
    {
        floorplan_devices.setDeviceSelection(deviceNum)
        sprite.border.width = selected ? 3 : 0
    }

    Image {
        id: fpDevice_image
        source:"../img/floorplanitems/"+deviceType+".png"
        sourceSize:Qt.size(scaleY(6), scaleY(6))
        anchors.centerIn: parent
	anchors.fill: parent
        cache: false
    }

    function getColor(deviceNum) {
        var t = floorplan_devices.getText(deviceNum)
        t = t.substring(0, t.indexOf("|"))
        if (t === 'ON')
            return "#00FFFF"
        else if (t === 'OFF')
            return "#000000"
    }

/*    ColorOverlay {
        anchors.fill: fpDevice_image
        source: bug
//        visible: getColor(deviceNum) != 0
        color: getColor(deviceNum)
    }
*/
    Text {
        id: device_number
        text: floorplan_devices.getText(deviceNum) + " " + floorplan_devices.getDeviceStatus(deviceNum)
        anchors.centerIn: parent
        font.pixelSize: scaleY(2)
    }

    Rectangle{
        id:sprite
        height: parent.height - 1
        width: parent.width - 1
        color: "transparent"
        opacity: 0
        border.color: "red"
        border.width:selected ? 2 : 0
        Component.onCompleted: PropertyAnimation {target:sprite ; property:"opacity"; to: 1; duration:1500 }
        scale: iconScale
    }

    Connections{
        target:floorplan_devices
        onChangePage:sprite_root.destroy()
    }

    MouseArea{
        anchors.fill: sprite_root
        onClicked: {updateFpItem()}
    }
}
