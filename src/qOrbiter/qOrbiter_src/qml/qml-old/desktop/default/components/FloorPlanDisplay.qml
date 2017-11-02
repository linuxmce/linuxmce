import QtQuick 1.1

Item {
    objectName: "floorplan_display"
    id:floorplandisplay
    width: childrenRect.width
    height: childrenRect.height
    Component.onCompleted:{ floorplan_devices.setCurrentPage(1)}
    property int scaleFactor:floorplanimage.scale


    function setCommandType(){

        if(floorplan_devices.getCurrentFloorPlanType()=== 2)
            return lightingCommands
    }

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



    Rectangle{
        id:mainRect
        objectName: "main_rect"
        height: scaleY(80)
        width:scaleX(80)
        border.color: skinstyle.highlight1
        border.width: scaleX(1) *.25
        radius: 5
        anchors.centerIn: parent
        color:skinstyle.lighthighlight

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
            color: skinstyle.lighthighlight
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
                visible: floorplan_pages.count !==0
                anchors.top: fplabel.bottom
                anchors.horizontalCenter: parent.horizontalCenter

                delegate:Rectangle{
                    height: scaleY(9)
                    width: scaleX(16)
                    color: skinstyle.accentcolor
                    border.color: skinstyle.darkhighlight
                    Text {
                        id: desc
                        text: m_description
                    }
                    MouseArea{
                        anchors.fill: parent
                        onPressed:{
                            floorplan_devices.itemSelected = false
                            floorplan_devices.selectedDevice= 0
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
        height: mainRect.height -12
        width: mainRect.width -2
        model:floorplan_devices
        clip:true
        orientation: ListView.Vertical
        anchors.centerIn: mainRect
        visible:true
        spacing:scaleY(1)
        delegate:
            Item{
            id:fp_device_rect
            height: scaleY(20)
            width: parent.width
            Component.onCompleted:  manager.getFloorplanDeviceCommands(deviceno);

            Rectangle{
                id:phil
                anchors.fill: parent
                color:mickey.pressed ?"yellow" :"white"
                opacity: .75
                MouseArea{
                    id:mickey
                    anchors.fill: parent
                    onClicked: manager.getFloorplanDeviceCommands(deviceno)
                }
            }


            Rectangle{
                id:standin
                height: parent.height
                width: scaleX(10)
                border.color: "black"
                border.width: 1
                Text {
                    text: "Floorplan type \n" + floorplantype
                    anchors.centerIn: parent
                    font.pointSize: 14
                }
            }

            Column{
                height: parent.height
                width: parent.width*66
                anchors.left: standin.right
                Text {
                    id: fpDevice_name
                    text: name
                    font.pointSize:scaleY(3)
                }
                //                Text {
                //                    id: fpDevice_type
                //                    text: "I am type" + type
                //                }
                //                Text {
                //                    id: fp_type
                //                    text: "Floorplan type" + floorplantype
                //                }
                //                Text {
                //                    id: fpDevice_no
                //                    text: "I am Dev#" + deviceno
                //                }
                //                Text {
                //                    id: fpDevice_pos
                //                    text: "Position" + floorplan_devices.getDeviceX(deviceno) + "," + floorplan_devices.getDeviceY(deviceno)
                //                    // onTextChanged: placeSprites(floorplan_devices.getDeviceX(deviceno),floorplan_devices.getDeviceY(deviceno),deviceno)
                //                }
            }



            ListView{
               anchors.bottom: parent.bottom
               anchors.left: standin.right
                width: parent.width
                height: parent.height/2
                model:commandlist["commands"]
                orientation:ListView.Horizontal
                spacing:10
                delegate: Rectangle{
                    id:thisone
                    height:50
                    width: 76
                    color: sublist_hit.containsMouse ? "lightblue": sublist_hit.pressed ? "green": "darkblue"
                    StyledText{
                        text: commandlist["commands"][index].command_name
                        anchors.centerIn: parent
                    }
                    MouseArea{
                        id:sublist_hit
                        anchors.fill: thisone
                        onClicked: console.log("Grebbed")

                    }

                }

            }
        }
    }
}

