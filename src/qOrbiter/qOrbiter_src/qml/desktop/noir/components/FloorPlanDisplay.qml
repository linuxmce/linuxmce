import QtQuick 1.1

Rectangle {
    objectName: "floorplan_display"
    width: style.orbiterW
    height: style.orbiterH
    color: style.darkhighlight
    property int scaleFactor:floorplanimage.scale

    function placeSprites(x,y, num, state, devtype)
    {
        var i;
        var pX = x; //x point
        var pY = y; //y point
        var c;
        console.log("Creating Sprite")
        c = Qt.createComponent("FpSprite.qml");
        if(c.status === Component.Loading)
        {   console.log("Component Loading")
           finishPlacingSprites(c,pX,pY, num, state, devtype)
        }
        else if (c.status === Component.Error)
        {
            console.log("Component Error")
        }
        else if (c.status === Component.Ready)
        {
            console.log("Component Ready!")
            var sprite = c.createObject(floorplanimage, {"x": pX, "y": pY, "deviceNum": num, "deviceType": devtype});
        }
    }

    function finishPlacingSprites(c,x,y,num, state, devtype)
    {
        console.log("Finishing Creation")
        if(c.status === Component.Ready )
        {
            var sprite = c.createObject(floorplanimage, {"x": x, "y": y, "deviceNum": num, "deviceType": devtype});
        }
        else
        {
            finishPlacingSprites(c,x,y,num, state, devtype)
        }

    }


    Connections{
        target: floorplan_devices
        onFloorPlanImageChanged: {
            floorplanimage.source = "image://listprovider/floorplan/"+floorplan_devices.currentPage

        }
    }
    Component.onCompleted: floorplan_devices.setCurrentPage(1)

    Rectangle{
        id:mainRect
        objectName: "main_rect"
        height: scaleY(80)
        width:scaleX(80)
        border.color: style.highlight1
        border.width: scaleX(1) *.25
        radius: 10
        anchors.centerIn: parent
        color:style.lighthighlight

        Image {
            objectName: "floorplan_image"
            id: floorplanimage
            source: ""
            anchors.centerIn: parent
            scale: floorplanimage.height > floorplanimage.width ? .5 : .75

        }

        Rectangle
        {
            height: childrenRect.height + 5
            width: scaleX(18)
            color: style.lighthighlight
            anchors.right: parent.right
            Text{
                id:fplabel
                text: qsTr("Floorplans")
                height: scaleY(5)
            }

            ListView{
                id:floorplanlist
                height: scaleY(10)
                width: scaleX(18)
                model:floorplan_pages
                clip:true
                anchors.top: fplabel.bottom
                anchors.horizontalCenter: parent.horizontalCenter

                delegate:Rectangle{
                    height: scaleY(9)
                    width: scaleX(16)
                    color: style.accentcolor
                    border.color: style.darkhighlight
                    Text {
                        id: desc
                        text: m_description
                    }

                    MouseArea{
                        anchors.fill: parent
                        onPressed:{
                            floorplan_devices.setCurrentPage(m_page)
                        }
                    }
                }
            }
        }
    }
    ListView
    {
        id:floorplanDevices
        height: scaleY(20)
        width: scaleX(80)
        model:floorplan_devices
        clip:true
        orientation: ListView.Horizontal
        anchors.top: mainRect.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        delegate:
            Rectangle{
            id:fp_device_rect
            height: scaleY(20)
            width: scaleY(20)

            color: "lightgrey"


            Column{
                Text {
                    id: fpDevice_name
                    text: "I am " + name
                }
                Text {
                    id: fpDevice_type
                    text: "I am type" + type
                }
                Text {
                    id: fp_type
                    text: "Floorplan type" + floorplantype
                }
                Text {
                    id: fpDevice_no
                    text: "I am Dev#" + deviceno
                }
                Text {
                    id: fpDevice_pos
                    text: "Position" + floorplan_devices.getDeviceX(deviceno) + "," + floorplan_devices.getDeviceY(deviceno)
                    // onTextChanged: placeSprites(floorplan_devices.getDeviceX(deviceno),floorplan_devices.getDeviceY(deviceno),deviceno)
                }
            }

            MouseArea{
                anchors.fill: parent
                onClicked: manager.getFloorplanDeviceCommand(type)
            }
        }
    }
}

