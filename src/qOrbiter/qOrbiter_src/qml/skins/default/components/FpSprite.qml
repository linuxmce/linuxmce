// import QtQuick 1.1 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2
import org.linuxmce.floorplans 1.0
import "../."
Item {
    id:sprite_root
    width: itemW
    height: itemH
    objectName:deviceNum
    property FloorplanDevice currentDevice:floorplan_devices.getDevice(deviceNum)
    property color activeColor: "grey"
    property color inactivecolor: "green"
    property double devicelevel:0.0
    property string state: "unknown"
    property string deviceName:currentDevice.deviceName// floorplan_devices.getText(Number(deviceNum))
    property string deviceNum:""
    property string deviceType: ""
    property string imgUrl: ""
    property bool selected:currentDevice.selected
    property int itemH:Style.scaleY(5)
    property int itemW: Style.scaleY(5)
    property double iconScale: 1.5
    onSelectedChanged: {
        console.log(deviceNum+" changed to "+selected)

    }

    Connections{
        target: floorplan_devices
        onSelectedDevicesChanged: {
            selected = floorplan_devices.getDeviceSelection(deviceNum)
        }
    }

    scale: iconScale
    function updateFpItem(){
        floorplan_devices.setDeviceSelection(deviceNum)

    }

    Image {
        id: fpDevice_image
        source:deviceType==="52" ? "" : "../images/floorplan/"+deviceType+".png"
        sourceSize:Qt.size(Style.scaleY(6), Style.scaleY(6))
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
    StyledText {
        id: deviceNumLabel
        text: currentDevice.deviceName  +"\n"+currentDevice.deviceState
        anchors.top: fpDevice_image.bottom
        font.pixelSize: Style.appFontSize_description
        visible: true
        color:"black"
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
        onClicked: {
         //  currentDevice.selected=!sprite_root.selected
             updateFpItem()
        }
    }
}
