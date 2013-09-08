import QtQuick 1.1

Item {
    objectName: "floorplan_display"
    id:floorplandisplay
    width:manager.appWidth
    height: manager.appHeight
    Component.onCompleted:{ floorplan_devices.setCurrentPage(1)}
    property int scaleFactor:floorplanimage.scale
    property alias bg:phil
    property bool useList:true

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
            console.log("Component Error"+ c.errorString())
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
        id:phil
        anchors.fill: parent
        border.color: "black"
        border.width: scaleX(1) *.25
        radius: 5
        anchors.centerIn: parent
        color:"black"
        opacity:.25
    }


    Item{
        id:mainRect
        objectName: "main_rect"
        height: floorplandisplay.height
        width:floorplandisplay.width

        Image {
            objectName: "floorplan_image"
            id: floorplanimage
            fillMode: Image.PreserveAspectCrop
            source: ""
            anchors.centerIn: parent
            scale: floorplanimage.height > floorplanimage.width ? .65 : .65
        }

    }

    Item{
        height: childrenRect.height + 5
        width: scaleX(35)
        anchors.right: parent.right
        Text{
            id:fplabel
            text: qsTr("Floorplans")
            height: scaleY(5)
            visible:floorplanlist.visible
        }

        ListView{
            id:floorplanlist
            height: scaleY(10)
            width: scaleX(35)
            model:floorplan_pages
            clip:true
            visible: floorplan_pages.count !==0 ? useList ? true : false : false
            anchors.top: fplabel.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            delegate:Rectangle{
                height: scaleY(9)
                width: scaleX(35)
                color: skinStyle.accentcolor
                border.color: skinStyle.darkhighlight
                Text {
                    id: desc
                    text: m_description
                }
                MouseArea{
                    anchors.fill: parent
                    onPressed:{
                        floorplan_devices.clearAllSelections()
                        floorplan_devices.setCurrentPage(m_page)
                    }
                }
            }
        }
    }

}

