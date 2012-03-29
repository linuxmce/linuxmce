import QtQuick 1.1

Rectangle {
    width: style.orbiterW
    height: style.orbiterH
    color: style.darkhighlight

    function placeSprites(x,y, name)
    {
        var i;
        var pX = x; //x point
        var pY = y; //y point
        console.log("Want to create " + name + " at " + x+","+y)
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
        height: scaleY(80)
        width:scaleX(80)
        border.color: style.highlight1
        border.width: scaleX(1) *.25
        radius: 10
        anchors.centerIn: parent
        color:style.lighthighlight

        Image {
            id: floorplanimage
            source: ""
            anchors.fill: parent
            height:scaleY(45)
            width: scaleX(45)
            fillMode: Image.PreserveAspectFit
            anchors.left: parent.left
            anchors.leftMargin: scaleY(2)
        }

        Rectangle
        {
            height: childrenRect.height + 5
            width: childrenRect.width + 5
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

                }
            }
            Connections{
                target: floorplan_devices
                onDataChanged:placeSprites(floorplan_devices.getDeviceX(deviceno),floorplan_devices.getDeviceY(deviceno),name)
            }

            MouseArea{
                anchors.fill: parent
                onClicked: dcerouter.getFloorplanDeviceCommand(type)
            }
        }
    }
}

