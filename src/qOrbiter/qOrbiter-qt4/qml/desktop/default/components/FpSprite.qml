// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    id:sprite_root
    width: itemW
    height: itemH
    objectName:deviceNum
    ListModel{
        id:commandModel

        ListElement{
            dcecommand:"on"
            dcecmdgrp:1
        }
    }

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
    property ListModel commandModel

    scale: iconScale
    function updateFpItem()
    {
        selected = !selected
        sprite.border.width = selected ? 1 : 0
    }

    Image {
        id: fpDevice_image
        source: "../img/floorplanitems/"+deviceType+".png"
        sourceSize:Qt.size(scaleY(5), scaleY(5))
        anchors.centerIn: parent
        cache: false
    }

    Text {
        id: device_number
        text: deviceNum
        anchors.centerIn: parent
        font.pixelSize: scaleY(2)
    }

    Rectangle{
        id:sprite
        height: parent.height - 1
        width: parent.width - 1
        color: "transparent"
        opacity: 0
        border.color: "white"
        border.width:sprite.border.width = selected ? 1 : 0
        Component.onCompleted: PropertyAnimation {target:sprite ; property:"opacity"; to: 1; duration:1500 }
        scale: iconScale
    }

    Connections{
        target:floorplan_devices
        onChangePage:sprite_root.destroy()
    }

    ListView{
        id:commandPopup
        height: selected === true ? 100 : 0
        width: selected === true? 50 : 0
        model: commandModel
        anchors.bottom: sprite.top
        anchors.horizontalCenter: parent.horizontalCenter
        delegate: Rectangle{
            visible: selected === true ? true : false
            height: 50
            width: 50
            color: delegateHit.pressed ? "white" : "lightgrey"
            Text {
                id: cmdLabel
                text: dcecommand
            }

            MouseArea{
                id:delegateHit
                anchors.fill: commandPopup
                onClicked: console.log("I want to send command:"+dcecommand)

            }

        }
    }

    MouseArea{
        anchors.fill: parent
        onClicked: updateFpItem()
    }
}
