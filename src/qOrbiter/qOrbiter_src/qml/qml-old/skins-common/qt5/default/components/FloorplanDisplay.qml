import QtQuick 2.2

Item {
    objectName: "floorplan_display"
    id:floorplandisplay
    width:parent.width
    height: parent.height
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
        height: floorplandisplay.height
        width:floorplandisplay.width
        border.color: "black"
        border.width: scaleX(1) *.25
        radius: 5
        anchors.centerIn: parent
        color:"black"

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
            color: "black"
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
                    color: appstyle.accentcolor
                    border.color: appstyle.darkhighlight
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

}

